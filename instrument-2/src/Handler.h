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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_HANDLER_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_HANDLER_H

#include <clang/Lex/Pragma.h>

#include "InstrContext.h"
#include "Parser.h"

// Clang's include files are rather large and can slow down compilation. So
// don't include them unless really necessary.
namespace clang {
class Preprocessor;
class PragmaIntroducer;
class Token;
} // namespace clang

namespace instr {

// The pragma handler. This looks for pragmas with the instrument sentinel.
class Handler : public clang::PragmaHandler {
private:
  InstrContext& instrContext;
  Parser parser;

public:
  explicit Handler(clang::Preprocessor&, InstrContext& instrContext);

  void HandlePragma(clang::Preprocessor& pp,
                    clang::PragmaIntroducer,
                    clang::Token& tok);
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_HANDLER_H
