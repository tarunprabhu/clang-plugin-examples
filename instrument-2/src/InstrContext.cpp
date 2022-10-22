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

#include "InstrContext.h"

#include <llvm/Support/raw_ostream.h>

#include <limits>

using namespace clang;

namespace instr {

bool InstrContext::empty(const FileEntry* file) const {
  if (this->lines.find(file) != this->lines.end())
    return this->lines.at(file).size() == 0;
  return true;
}

unsigned InstrContext::peek(const FileEntry* file) const {
  return this->lines.at(file).front()->getLineNumber();
}

unsigned InstrContext::pop(const FileEntry* file) {
  unsigned front = this->lines.at(file).front()->getLineNumber();
  this->lines.at(file).pop();
  return front;
}

Directive* InstrContext::add(Directive* dr) {
  FullSourceLoc loc = dr->getLoc();
  const FileEntry* fileEntry = loc.getFileEntry();

  this->directives.emplace_back(dr);
  this->lines[fileEntry].push(dr);

  return dr;
}

Clause* InstrContext::add(Clause* cl) {
  this->clauses.emplace_back(cl);
  return cl;
}

unsigned InstrContext::findNearestAndPop(const FullSourceLoc& loc) {
  const FileEntry* file = loc.getFileEntry();
  unsigned lno = loc.getLineNumber();

  // There may be orphaned pragmas that are not associated with any loop
  // with another pragma being nearer the current loop.
  unsigned nearest = InstrContext::invalid;
  while ((not this->empty(file)) and (this->peek(file) < lno))
    nearest = this->pop(file);

  return nearest;
}

const unsigned InstrContext::invalid = std::numeric_limits<unsigned>::max();

} // namespace instr
