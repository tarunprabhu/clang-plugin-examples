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

#ifndef CLANG_PLUGIN_EXAMPLES_DEMARCATOR_INSTR_CONTEXT_H
#define CLANG_PLUGIN_EXAMPLES_DEMARCATOR_INSTR_CONTEXT_H

#include "Clause.h"
#include "Directive.h"

#include <clang/Basic/FileEntry.h>

#include <map>
#include <memory>
#include <queue>

namespace instr {

// Class for data that will be passed between the plugin, consumer, visitor
// and the pragma handler. This is the owner of all the annotation objects,
// and is used to associate pragmas with AST nodes.
class InstrContext {
private:
  std::map<const clang::FileEntry*, std::queue<Directive*>> lines;
  std::vector<std::unique_ptr<Directive>> directives;
  std::vector<std::unique_ptr<Clause>> clauses;

public:
  static const unsigned invalid;

private:
  bool empty(const clang::FileEntry* file) const;
  unsigned peek(const clang::FileEntry* file) const;
  unsigned pop(const clang::FileEntry* file);

public:
  Directive* add(Directive* dr);
  Clause* add(Clause* cl);
  unsigned findNearestAndPop(const clang::FullSourceLoc& loc);
};

} // namespace instr

#endif // CLANG_PLUGIN_EXAMPLES_DEMARCATOR_INSTR_CONTEXT_H
