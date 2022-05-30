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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_STMTATTR_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_STMTATTR_H

#include "AttrContext.h"

#include <clang/Sema/ParsedAttr.h>

class MyStmtAttrInfo : public clang::ParsedAttrInfo {
private:
  // This is a reference to the global singleton AttrContext object - or, if
  // we have found a way to avoid the use a global singleton, then the local
  // singleton object.
  AttrContext& attrContext;

public:
  MyStmtAttrInfo();

  // Check if the attribute can reasonably be applied to the given Decl.
  bool diagAppertainsToStmt(clang::Sema& sema,
                            const clang::ParsedAttr& attr,
                            const clang::Stmt* decl) const override;
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_STMTATTR_H
