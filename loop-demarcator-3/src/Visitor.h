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

#ifndef CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_VISITOR_H
#define CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>

namespace clang {
class CompilerInstance;
} // namespace clang

// The visitor class will visit all the AST nodes and is where the loops will
// be identified and associated with a pragma.
class Visitor : public clang::RecursiveASTVisitor<Visitor> {
private:
  clang::CompilerInstance& ci;
  clang::ASTContext& astContext;
  clang::SourceManager& srcMgr;
  clang::Language lang;

  clang::FunctionDecl* enterDecl;
  clang::FunctionDecl* exitDecl;

private:
  void raiseMultipleParentsError(clang::Stmt* stmt);
  void demarcate(clang::Stmt* stmt);
  const clang::Stmt* getParent(clang::Stmt* stmt);

  // Create a CallExpr where the given FunctionDecl is called with no
  // arguments. The SourceLocation should, ideally, be a reasonable location
  // at which the call is inserted, but it could also be an invalid location.
  clang::Stmt* getCall(clang::FunctionDecl* fn, clang::SourceLocation loc);

  // Wrap the FunctionDecl in a DeclRefExpr. This is necessary for it to be
  // used in a CallExpr.
  clang::DeclRefExpr* getDeclRefExpr(clang::FunctionDecl* fn);

  // Create a function declaration in the given DeclContext. The DeclContext
  // may be the top-level context obtained by getTranslationUnitDecl(), or it
  // could be a different decl context. Another useful one to use may be the
  // externCContext() which is a DeclContext for extern "C" declarations.
  // The SourceLocation may or may not be valid. If it is not valid, it could
  // cause problems in debugging if it were to ever trigger a compile error for
  // some reason.
  clang::FunctionDecl* getDecl(clang::SourceLocation loc,
                               clang::IdentifierInfo& ident);

  // Create a call to __enterLoop(). The DeclContext is used to obtain the
  // FunctionDecl for the __enterLoop function. The SourceLocation may or may
  // not be valid.
  clang::Stmt* getEnterCall(clang::SourceLocation loc);

  // Create a call to __exitLoop(). The DeclContext is used to obtain the
  // FunctionDecl for the __exitLoop function. The SourceLocation may or may
  // not be valid.
  clang::Stmt* getExitCall(clang::SourceLocation loc);

public:
  explicit Visitor(clang::CompilerInstance& ci);
  virtual ~Visitor() = default;

  bool shouldVisitTemplateInstantiations() const;
  bool VisitForStmt(clang::ForStmt* stmt);
  bool VisitDoStmt(clang::DoStmt* stmt);
  bool VisitWhileStmt(clang::WhileStmt* stmt);
};

#endif // CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_VISITOR_H
