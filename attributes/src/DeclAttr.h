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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_DECLATTR_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_DECLATTR_H

#include "AttrContext.h"

#include <clang/Sema/ParsedAttr.h>

struct MyDeclAttrInfo : public clang::ParsedAttrInfo {
private:
  // This is a reference to the global singleton AttrContext object - or, if
  // we have found a way to avoid the use a global singleton, then the local
  // singleton object.
  AttrContext& attrContext;

public:
  MyDeclAttrInfo();

  // Check if the attribute can reasonably be applied to the given Decl.
  bool diagAppertainsToDecl(clang::Sema& sema,
                            const clang::ParsedAttr& attr,
                            const clang::Decl* decl) const override;

  // Handle the attribute and associate it with the decl.
  clang::ParsedAttrInfo::AttrHandling
  handleDeclAttribute(clang::Sema& sema,
                      clang::Decl* decl,
                      const clang::ParsedAttr& attr) const override;
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_DECLATTR_H
