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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_ATTRCONTEXT_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_ATTRCONTEXT_H

#include <map>
#include <vector>

namespace clang {
class Decl;
class ParsedAttr;
class Stmt;
} // namespace clang

// Maintains maps from Decl's and Stmt's to any attributes associated with
// them. Currently, it stores clang's ParsedAttr objects, but it may be better
// to save a custom attribute object instead that allows more access to any
// arguments.
class AttrContext {
public:
  using Attrs = std::vector<const clang::ParsedAttr*>;

private:
  std::map<const clang::Decl*, Attrs> declAttrs;
  std::map<const clang::Stmt*, Attrs> stmtAttrs;

public:
  void add(const clang::Decl* decl, const clang::ParsedAttr* attr);
  void add(const clang::Stmt* stmt, const clang::ParsedAttr* attr);

  bool has(const clang::Decl* decl) const;
  bool has(const clang::Stmt* stmt) const;

  const Attrs& get(const clang::Decl* decl) const;
  const Attrs& get(const clang::Stmt* stmt) const;
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_ATTRCONTEXT_H
