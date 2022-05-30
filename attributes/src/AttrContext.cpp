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

#include "AttrContext.h"

using namespace clang;

static const AttrContext::Attrs empty;

void AttrContext::add(const Decl* decl, const ParsedAttr* attr) {
  this->declAttrs[decl].push_back(attr);
}

void AttrContext::add(const Stmt* stmt, const ParsedAttr* attr) {
  this->stmtAttrs[stmt].push_back(attr);
}

bool AttrContext::has(const Decl* decl) const {
  return this->declAttrs.find(decl) != this->declAttrs.end();
}

bool AttrContext::has(const Stmt* stmt) const {
  return this->stmtAttrs.find(stmt) != this->stmtAttrs.end();
}

const AttrContext::Attrs& AttrContext::get(const Decl* decl) const {
  if (not this->has(decl))
    return empty;
  return this->declAttrs.at(decl);
}

const AttrContext::Attrs& AttrContext::get(const Stmt* stmt) const {
  if (not this->has(stmt))
    return empty;
  return this->stmtAttrs.at(stmt);
}
