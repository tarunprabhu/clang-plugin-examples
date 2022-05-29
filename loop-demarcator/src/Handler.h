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

#ifndef CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_HANDLER_H
#define CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_HANDLER_H

#include <clang/Lex/Pragma.h>

#include "Pragmas.h"

// Forward declarations of clang classes. Some of the clang header files
// are very large and can cause a noticeable increase in compile time. Only
// include the actual files when necessary and use forward declarations as
// much as possible - especially in header files.
namespace clang {
class Preprocessor;
class PragmaIntroducer;
class Token;
} // namespace clang

// The pragma handler. This looks for pragmas with the demarcate sentinel
// and records their locations.
class DemarcatePragmaHandler : public clang::PragmaHandler {
private:
  Pragmas& pragmas;

public:
  DemarcatePragmaHandler(Pragmas& pragmas);
  void HandlePragma(clang::Preprocessor& pp,
                    clang::PragmaIntroducer,
                    clang::Token& tok);
};

#endif // CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_HANDLER_H
