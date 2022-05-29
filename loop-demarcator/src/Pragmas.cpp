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

#include "Pragmas.h"

#include <llvm/Support/raw_ostream.h>

#include <limits>

using namespace clang;

bool Pragmas::empty(const FileEntry* file) const {
  if (this->lines.find(file) == this->lines.end())
    return true;
  return this->lines.at(file).size() == 0;
}

unsigned Pragmas::peek(const FileEntry* file) const {
  return this->lines.at(file).front();
}

unsigned Pragmas::pop(const FileEntry* file) {
  unsigned front = this->lines.at(file).front();
  this->lines.at(file).pop();
  return front;
}

void Pragmas::push(const FileEntry* file, unsigned line) {
  this->lines[file].push(line);
}

unsigned Pragmas::findNearestAndPop(const FileEntry* file, unsigned lno) {
  // There may be orphaned pragmas that are not associated with any loop
  // with another pragma being nearer the current loop.
  unsigned nearest = Pragmas::invalid;
  while ((not this->empty(file)) and (this->peek(file) < lno))
    nearest = this->pop(file);

  return nearest;
}

const unsigned Pragmas::invalid = std::numeric_limits<unsigned>::max();
