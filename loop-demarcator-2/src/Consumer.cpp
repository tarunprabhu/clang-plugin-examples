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

#include "Consumer.h"
#include "Handler.h"

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Preprocessor.h>

using namespace clang;

Consumer::Consumer(CompilerInstance& ci)
    : ci(ci), rewriter(ci.getSourceManager(), ci.getLangOpts()),
      visitor(ci, pragmas, rewriter) {
  ci.getPreprocessor().AddPragmaHandler(
      new DemarcatePragmaHandler(this->pragmas));
}

void Consumer::HandleTranslationUnit(ASTContext& astContext) {
  // If there are parse errors in the file, they will be recorded in the
  // diagnostics. Since this will not attempt to fix those, don't go any
  // further here.
  if (this->ci.getDiagnostics().getNumErrors())
    return;

  visitor.TraverseDecl(astContext.getTranslationUnitDecl());

  SourceManager& srcMgr = this->ci.getSourceManager();
  this->rewriter.getEditBuffer(srcMgr.getMainFileID()).write(llvm::outs());
}
