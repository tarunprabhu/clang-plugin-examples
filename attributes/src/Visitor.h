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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_VISITOR_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_VISITOR_H

#include <clang/AST/RecursiveASTVisitor.h>

#include "AttrContext.h"

namespace clang {
class CompilerInstance;
class SourceManager;
} // namespace clang

class Visitor : public clang::RecursiveASTVisitor<Visitor> {
private:
  clang::SourceManager& srcMgr;

  // This is a reference to the global singleton AttrContext object.
  // We keep a reference in case some way is found to avoid global singletons.
  AttrContext& attrContext;

private:
  void printAttrs(clang::Stmt* stmt);
  void printAttrs(clang::NamedDecl* decl);

public:
  explicit Visitor(clang::CompilerInstance& ci, AttrContext& attrContext);

  bool VisitDoStmt(clang::DoStmt* doStmt);
  bool VisitForStmt(clang::ForStmt* forStmt);
  bool VisitWhileStmt(clang::WhileStmt* whileStmt);

  bool VisitFunctionDecl(clang::FunctionDecl* decl);
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_VISITOR_H
