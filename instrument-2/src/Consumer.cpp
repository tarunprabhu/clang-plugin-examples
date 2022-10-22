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

namespace instr {

Consumer::Consumer(CompilerInstance& ci)
    : visitor(ci, instrContext) {
  Preprocessor& pp = ci.getPreprocessor();
  pp.AddPragmaHandler(new Handler(pp, instrContext));
}

void Consumer::HandleTranslationUnit(ASTContext& astContext) {
  // If there are parse errors in the file, they will be recorded in the
  // diagnostics. Since this will not attempt to fix those, don't go any
  // further here.
  if (astContext.getDiagnostics().getNumErrors())
    return;

  TranslationUnitDecl* tu = astContext.getTranslationUnitDecl();

  visitor.TraverseDecl(tu);
}

} // namespace instr
