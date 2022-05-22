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

#include "AstIrContext.h"

#include <clang/Frontend/CompilerInstance.h>

AstIrContext::AstIrContext(clang::CompilerInstance& ci,
                           bool printBasename)
    : ci(ci), printBasename_(printBasename) {
  ;
}

bool AstIrContext::printBasename() const {
  return this->printBasename_;
}

clang::CompilerInstance& AstIrContext::getCompilerInstance() {
  return this->ci;
}

clang::ASTContext& AstIrContext::getAstContext() {
  return this->ci.getASTContext();
}

clang::SourceManager& AstIrContext::getSourceManager() {
  return this->ci.getSourceManager();
}

void AstIrContext::addDecl(const std::string& mangled,
                           const clang::FunctionDecl* decl) {
  this->decls_[mangled] = decl;
}

const clang::FunctionDecl*
AstIrContext::getDecl(clang::StringRef mangled) const {
  auto it = this->decls_.find(mangled.str());
  if (it != this->decls_.end())
    return it->second;
  return nullptr;
}
