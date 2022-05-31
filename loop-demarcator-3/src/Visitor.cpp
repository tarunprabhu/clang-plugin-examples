/*
  Copyright  2022  Tarun Prabhu

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "Visitor.h"

#include <clang/AST/ParentMapContext.h>
#include <clang/AST/Type.h>
#include <clang/Frontend/CompilerInstance.h>

#include <limits>

using namespace clang;

Visitor::Visitor(CompilerInstance& ci)
    : ci(ci), astContext(ci.getASTContext()), srcMgr(ci.getSourceManager()),
      lang(LangStandard::getLangStandardForKind(ci.getLangOpts().LangStd)
               .getLanguage()),
      enterDecl(nullptr), exitDecl(nullptr) {
  ;
}

void Visitor::raiseMultipleParentsError(Stmt* stmt) {
  DiagnosticsEngine& diags = ci.getDiagnostics();

  // This currently returns unsigned, but at some point, it may have been
  // something different.
  unsigned id = diags.getCustomDiagID(DiagnosticsEngine::Error,
                                      "Statement has more than one parent.");
  diags.Report(stmt->getBeginLoc(), id);
}

const Stmt* Visitor::getParent(Stmt* stmt) {
  ParentMapContext parentMap(this->astContext);
  DynTypedNodeList parents = parentMap.getParents(*stmt);

  // The documentation says that statements in templates can have multiple
  // parents. Not really sure under exactly what circumstances this happen,
  // but until I can figure that out, require the statement to have only one
  // parent.
  if (parents.size() != 1)
    this->raiseMultipleParentsError(stmt);

  return parents[0].get<Stmt>();
}

DeclRefExpr* Visitor::getDeclRefExpr(FunctionDecl* fn) {
  ASTContext& ast = this->astContext;
  SourceLocation loc = fn->getBeginLoc();

  return DeclRefExpr::Create(ast, NestedNameSpecifierLoc(), SourceLocation(),
                             fn, false, loc, fn->getType(),
                             ExprValueKind::VK_LValue);
}

Stmt* Visitor::getCall(FunctionDecl* fn, SourceLocation loc) {
  ASTContext& ast = this->astContext;

  Expr* callee = ImplicitCastExpr::Create(
      ast, ast.getPointerType(fn->getType()), CK_FunctionToPointerDecay,
      this->getDeclRefExpr(fn), nullptr, VK_PRValue, FPOptionsOverride());

  return CallExpr::Create(ast, callee, {}, fn->getCallResultType(),
                          ExprValueKind::VK_PRValue, loc, FPOptionsOverride());
}

FunctionDecl* Visitor::getDecl(SourceLocation loc, IdentifierInfo& ident) {
  ASTContext& ast = this->astContext;
  DeclarationName name(&ident);
  QualType fty
      = ast.getFunctionType(ast.VoidTy, {}, FunctionProtoType::ExtProtoInfo());

  // Pick the right context for the decl because that will ensure that the
  // resulting decl doesn't get mangled. Not sure what the purpose of an
  // externCContextDecl is because it doesn't work. A LinkageSpecDecl has to
  // be created explicitly for the function to have the correct "extern C"
  // linkage in C++.
  DeclContext* declContext = ast.getTranslationUnitDecl();
  if (this->lang == Language::CXX) {
    declContext
        = LinkageSpecDecl::Create(ast, ast.getTranslationUnitDecl(), loc, loc,
                                  LinkageSpecDecl::lang_c, false);
  }

  return FunctionDecl::Create(
      ast,                   // The AST context
      declContext,           // The context in which to create the function
      loc,                   // Location of the function body
      loc,                   // Location of the function name
      name,                  // Name of the function
      fty,                   // Function type
      nullptr,               // Type source info
      StorageClass::SC_None, // Storage class
      false,                 // isInlineSpecified
      false);                // hasWrittenPrototype
}

Stmt* Visitor::getEnterCall(SourceLocation loc) {
  if (not this->enterDecl) {
    ASTContext& ast = this->astContext;
    IdentifierInfo& ident = ast.Idents.get("__enterLoop");

    this->enterDecl = this->getDecl(loc, ident);
  }
  return this->getCall(this->enterDecl, loc);
}

Stmt* Visitor::getExitCall(SourceLocation loc) {
  if (not this->exitDecl) {
    ASTContext& ast = this->astContext;
    IdentifierInfo& ident = ast.Idents.get("__exitLoop");

    this->exitDecl = this->getDecl(loc, ident);
  }
  return this->getCall(this->exitDecl, loc);
}

void Visitor::demarcate(Stmt* stmt) {
  // Don't demarcate loops that are not in the file being compiled. This will
  // eliminate loops that are contained in any included files.
  SourceManager& srcMgr = this->srcMgr;
  const FileEntry* file
      = FullSourceLoc(stmt->getBeginLoc(), srcMgr).getFileEntry();

  // Not sure under what conditions file will be null. Probably when the
  // statement being visited is in a constructor that has been automatically
  // generated.
  if (not file or not srcMgr.isMainFile(*file))
    return;

  ASTContext& ast = this->astContext;
  SourceLocation beg = stmt->getBeginLoc();
  SourceLocation end = stmt->getEndLoc();

  // -------------------------------------------------------------------------
  //
  //                         BIO-HAZARD WARNING
  //
  // This is technically legal, but really, really shouldn't be!
  //
  // The ParentMap context class only returns a const Stmt - presumably because
  // calling getParent on ParentMapContext is expensive and it, therefore,
  // caches the results after the first call. However, a new ParentMapContext
  // is created each time getParent() is called, so we don't have to worry
  // about getting invalid parents. However, that does mean that getParent() is
  // pretty expensive (O(N) in the number of AST nodes).
  //
  // FIXME: A better approach may be to keep track of the parents ourselves
  // while visiting statements and loops.
  //
  Stmt* parent = const_cast<Stmt*>(this->getParent(stmt));

  Stmt* enterCall = this->getEnterCall(beg);
  Stmt* exitCall = this->getExitCall(end);

  for (auto it = parent->child_begin(); it != parent->child_end(); it++) {
    if (*it == stmt) {
      *it = CompoundStmt::Create(ast, {enterCall, stmt, exitCall}, beg, end);
      break;
    }
  }
}

// TODO: Should try to see what happens if a pragma is put inside a
// template body which may be instantiated several times. I think the
// idea here is to allow the visitor to traverse over those as well, but
// that hasn't been tested and I am not sure if it works the way I think
// it does.
bool Visitor::shouldVisitTemplateInstantiations() const {
  return true;
}

bool Visitor::VisitForStmt(ForStmt* stmt) {
  this->demarcate(stmt);

  return true;
}

bool Visitor::VisitDoStmt(DoStmt* stmt) {
  this->demarcate(stmt);

  return true;
}

bool Visitor::VisitWhileStmt(WhileStmt* stmt) {
  this->demarcate(stmt);

  return true;
}
