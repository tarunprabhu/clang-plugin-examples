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

#include "Handler.h"

#include <clang/Lex/Preprocessor.h>

using namespace clang;

// Here, "demarcate" is the sentinel of the pragma that will be matched.
DemarcatePragmaHandler::DemarcatePragmaHandler(Pragmas& pragmas)
    : PragmaHandler("demarcate"), pragmas(pragmas) {
  ;
}

// This way, the pragma is only matched against the sentinel, but the
// rest of the pragma is not examined. For something like OpenMP which has
// a much richer pragma language, the rest of the line would need to be
// parsed. That may be demonstrated in a different example plugin.
void DemarcatePragmaHandler::HandlePragma(Preprocessor& pp,
                                          PragmaIntroducer,
                                          Token& tok) {
  SourceManager& srcMgr = pp.getSourceManager();
  FullSourceLoc loc(tok.getLocation(), srcMgr);
  this->pragmas.push(loc.getFileEntry(), loc.getLineNumber());
}
