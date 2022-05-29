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
#include <clang/Frontend/CompilerInstance.h>

#include <limits>

using namespace clang;

Visitor::Visitor(CompilerInstance& ci, Pragmas& pragmas)
    : ci(ci), astContext(ci.getASTContext()), srcMgr(ci.getSourceManager()),
      pragmas(pragmas), enterDecl(nullptr), exitDecl(nullptr) {
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

Stmt* Visitor::getParent(Stmt* stmt) {
  ParentMapContext parentMap(this->astContext);
  DynTypedNodeList parents = parentMap.getParents(*stmt);

  // The documentation says that statements in templates can have multiple
  // parents. Not really sure under exactly what circumstances this happen,
  // but until I can figure that out, require the statement to have only one
  // parent.
  if (parents.size() != 1)
    this->raiseMultipleParentsError(stmt);

  const Stmt* parent = parents[0].get<Stmt>();

  // -------------------------------------------------------------------------
  //
  //                         BIO-HAZARD WARNING
  //
  // This is technically legal, but really, really shouldn't be!
  //
  // The ParentMap context class only returns a const Stmt - presumably because
  // calling getParent on ParentMapContext is expensive and it, therefore,
  // caches the results after the first call. However, the ParentMapContext
  // object is local to this function, so the fact that it is cached is not
  // relevant. Any other calls to this method from a different node will
  // end up recalculating the parents.
  //
  // FIXME: A better approach may be to keep track of the parents ourselves
  // while visiting statements and loops.
  //
  return const_cast<Stmt*>(parent);
}

bool Visitor::shouldDemarcate(Stmt* stmt) {
  FullSourceLoc loc(stmt->getBeginLoc(), this->srcMgr);
  const FileEntry* file = loc.getFileEntry();
  unsigned lno = loc.getLineNumber();
  unsigned plno = this->pragmas.findNearestAndPop(file, lno);
  return plno != Pragmas::invalid;
}

DeclRefExpr* Visitor::getDeclRefExpr(FunctionDecl* fn) {
  ASTContext& ast = this->astContext;
  SourceLocation loc = fn->getBeginLoc();

  return DeclRefExpr::Create(ast,
                             NestedNameSpecifierLoc(),
                             SourceLocation::getFromRawEncoding(0),
                             fn,
                             false,
                             loc,
                             ast.VoidTy,
                             ExprValueKind::VK_LValue);
}

Stmt* Visitor::getCall(FunctionDecl* fn, SourceLocation loc) {
  ASTContext& ast = this->astContext;
  return CallExpr::Create(ast,
                          this->getDeclRefExpr(fn),
                          {},
                          ast.VoidTy,
                          ExprValueKind::VK_PRValue,
                          loc,
                          FPOptionsOverride());
}

FunctionDecl* Visitor::getDecl(DeclContext* declContext,
                               SourceLocation loc,
                               IdentifierInfo& ident) {
  ASTContext& ast = this->astContext;
  DeclarationName name(&ident);
  return FunctionDecl::Create(ast,
                              declContext,
                              loc,
                              loc,
                              name,
                              ast.VoidTy,
                              nullptr,
                              StorageClass::SC_None);
}

Stmt* Visitor::getEnterCall(DeclContext* declContext, SourceLocation loc) {
  ASTContext& ast = this->astContext;
  IdentifierTable& idents = ast.Idents;
  IdentifierInfo& ident = idents.get("__enterLoop");

  if (not this->enterDecl)
    this->enterDecl = this->getDecl(declContext, loc, ident);
  return this->getCall(this->enterDecl, loc);
}

Stmt* Visitor::getExitCall(DeclContext* declContext, SourceLocation loc) {
  ASTContext& ast = this->astContext;
  IdentifierTable& idents = ast.Idents;
  IdentifierInfo& ident = idents.get("__exitLoop");
  DeclarationName name(&ident);

  if (not this->exitDecl)
    this->exitDecl = this->getDecl(declContext, loc, ident);
  return this->getCall(this->exitDecl, loc);
}

void Visitor::demarcate(Stmt* stmt) {
  ASTContext& ast = this->astContext;
  SourceLocation beg = stmt->getBeginLoc();
  SourceLocation end = stmt->getEndLoc();
  Stmt* parent = this->getParent(stmt);

  // FIXME: Should check which language this is and use a different
  // DeclContext. C++ should use ExternCContext while C can just use
  // TranslationUnitDecl. Since this is just an example, it doesn't really
  // matter, but in an actual application, this will probably be a library
  // call (even if only to a sentinel) that needs to be caught in the LLVM-IR.
  // If the correct context is not used, then the name will probably get
  // mangled which is undesirable.
  DeclContext* declContext = this->astContext.getTranslationUnitDecl();

  Stmt* enterCall = this->getEnterCall(declContext, beg);
  Stmt* exitCall = this->getExitCall(declContext, end);

  for (auto it = parent->child_begin(); it != parent->child_end(); it++) {
    if (*it == stmt) {
      *it = CompoundStmt::Create(ast, {enterCall, stmt, exitCall}, beg, end);
      break;
    }
  }
}

void Visitor::maybeDemarcate(Stmt* stmt) {
  if (this->shouldDemarcate(stmt))
    this->demarcate(stmt);
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
  this->maybeDemarcate(stmt);

  return true;
}

bool Visitor::VisitDoStmt(DoStmt* stmt) {
  this->maybeDemarcate(stmt);

  return true;
}

bool Visitor::VisitWhileStmt(WhileStmt* stmt) {
  this->maybeDemarcate(stmt);

  return true;
}
