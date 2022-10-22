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

#ifndef CLANG_PLUGIN_EXAMPLES_INSTRUMENT_INSTR_H
#define CLANG_PLUGIN_EXAMPLES_INSTRUMENT_INSTR_H

#include <vector>

namespace instr {

class Directive;
class Clause;
class InstrContext;

using Clauses = std::vector<Clause*>;

enum class Style {
  Decl,
  Qual,
  Full,
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_INSTRUMENT_INSTR_H
