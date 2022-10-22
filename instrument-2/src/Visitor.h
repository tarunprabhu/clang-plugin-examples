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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_VISITOR_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_VISITOR_H

#include "InstrContext.h"

#include <clang/AST/ParentMapContext.h>
#include <clang/AST/RecursiveASTVisitor.h>

namespace clang {
class ASTContext;
class CompilerInstance;
class SourceManager;
} // namespace clang

namespace instr {

// The visitor class will visit all the AST nodes and is where the loops will
// be identified and associated with a pragma.
class Visitor : public clang::RecursiveASTVisitor<Visitor> {
private:
  clang::CompilerInstance& ci;
  clang::ASTContext& astContext;
  clang::SourceManager& srcMgr;
  InstrContext& instrContext;
  clang::ParentMapContext parentMap;

private:
  void raiseMultipleParentsError(clang::Stmt* stmt);
  bool shouldDemarcate(clang::Stmt* stmt, Directive::Kind kind);
  void maybeDemarcate(clang::Stmt* stmt, Directive::Kind kind);
  void demarcate(clang::Stmt* stmt);
  clang::Stmt* getParent(clang::Stmt* stmt);

  clang::Stmt* getCall(clang::FunctionDecl* fn, clang::SourceLocation loc);
  clang::DeclRefExpr* getDeclRefExpr(clang::FunctionDecl* fn);
  clang::FunctionDecl* getDecl(clang::DeclContext* declContext,
                               clang::SourceLocation loc,
                               clang::IdentifierInfo& ident);
  clang::Stmt* getEnterCall(clang::DeclContext* declContext,
                            clang::SourceLocation loc);
  clang::Stmt* getExitCall(clang::DeclContext* declContext,
                           clang::SourceLocation loc);

public:
  explicit Visitor(clang::CompilerInstance& compiler,
                   InstrContext& instrContext);
  virtual ~Visitor() = default;

  bool shouldVisitTemplateInstantiations() const;
  bool VisitCompoundStmt(clang::CompoundStmt* stmt);
  bool VisitForStmt(clang::ForStmt* stmt);
  bool VisitDoStmt(clang::DoStmt* stmt);
  bool VisitWhileStmt(clang::WhileStmt* stmt);
  bool VisitFunctionDecl(clang::FunctionDecl* decl);
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_VISITOR_H
