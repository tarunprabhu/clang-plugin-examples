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

#include <clang/Lex/Preprocessor.h>

#include "Handler.h"
#include "Parser.h"

using namespace clang;

namespace instr {

// This pragma handler will only match those pragmas with an "instrument"
// sentinel.
Handler::Handler(Preprocessor& pp, InstrContext& instrContext)
    : PragmaHandler(Parser::tokSentinel), instrContext(instrContext),
      parser(pp, instrContext) {
  ;
}

void Handler::HandlePragma(Preprocessor& pp, PragmaIntroducer, Token& tok) {
  this->parser.prepareToParse();
  if (Directive* dr = parser.parseDirective(tok))
    this->instrContext.add(dr);
}

} // namespace instr
