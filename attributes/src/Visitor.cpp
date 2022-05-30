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

#include <clang/Sema/ParsedAttr.h>
#include <clang/Frontend/CompilerInstance.h>

using namespace clang;

Visitor::Visitor(CompilerInstance& ci, AttrContext& attrContext)
    : srcMgr(ci.getSourceManager()), attrContext(attrContext) {
  ;
}

void Visitor::printAttrs(Stmt* stmt) {
  if (this->attrContext.has(stmt)) {
    llvm::errs() << "Attrs for " << stmt->getStmtClassName() << " at "
                 << stmt->getBeginLoc().printToString(this->srcMgr) << "\n";
    for (const ParsedAttr* attr : attrContext.get(stmt))
      llvm::errs() << "    " << attr->getNormalizedFullName() << "\n";
    llvm::errs() << "\n";
  }
}

void Visitor::printAttrs(NamedDecl* decl) {
  if (this->attrContext.has(decl)) {
    llvm::errs() << "Attrs for " << decl->getNameAsString() << "\n";
    for (const ParsedAttr* attr : attrContext.get(decl))
      llvm::errs() << "    " << attr->getNormalizedFullName() << "\n";
    llvm::errs() << "\n";
  }
}

bool Visitor::VisitDoStmt(DoStmt* doStmt) {
  this->printAttrs(doStmt);

  return true;
}

bool Visitor::VisitForStmt(ForStmt* forStmt) {
  this->printAttrs(forStmt);

  return true;
}

bool Visitor::VisitWhileStmt(WhileStmt* whileStmt) {
  this->printAttrs(whileStmt);

  return true;
}

bool Visitor::VisitFunctionDecl(FunctionDecl* decl) {
  this->printAttrs(decl);

  return true;
}
