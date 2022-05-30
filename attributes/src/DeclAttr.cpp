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

#include "DeclAttr.h"
#include "Singleton.h"

#include <clang/Basic/DiagnosticSema.h>
#include <clang/Sema/Sema.h>

using namespace clang;

// Various spellings for the attribute. This currently allows GNU and C++11
// (both with and without an explicit namespace). Other syntaxes sucha s
// __declspec are possible and will need to be added to this list if desired.
// The documentation claims that #pragma should also work, but I haven't found
// a way of making it work.
static constexpr ParsedAttrInfo::Spelling spellings[]
    = {{ParsedAttr::AS_GNU, "mydeclattr"},
       {ParsedAttr::AS_CXX11, "mydeclattr"},
       {ParsedAttr::AS_CXX11, "myns::mydeclattr"}};

MyDeclAttrInfo::MyDeclAttrInfo() : attrContext(getSingletonAttrContext()) {
  // Not sure what the attribute kind should be here. I assume that
  // NoSemaHandler means that Sema does not handle this attribute, but I
  // can't find confirmation of it in the source one way or another.
  this->AttrKind = AttributeCommonInfo::NoSemaHandlerAttribute;

  // The number of required arguments for the attribute.
  this->NumArgs = 0;

  // The number of optional arguments for the attribute.
  this->OptArgs = 1;

  // Set to true if this attribute can be applied to types.
  this->IsType = false;

  // Set to false if this attribute can only be applied to Decl's.
  // Set to true if this attribute can only be applied to Stmt's.
  this->IsStmt = false;

  // The various spellings of this attribute. This allows the attribute to
  // be specified using different syntaxes. For instance:
  //
  //     GNU        __attribute__((instrument))
  //     C++11      [[instrument]]
  //     Declspec   __declspec(mystmtattr)
  //     Keyword    __instrument
  //     ...
  //
  // I have not managed to get all of them to work, presumably because I am
  // not declaring them correctly, or maybe it needs additional flags to be
  // passed to clang - I am not sure.
  this->Spellings = spellings;
}

// Check if the attribute can reasonably be applied to the given Decl.
// In this case, we only check if the Decl is a FunctionDecl because the
// attribute can only be applied to a function. But something more
// sophisticated can be done here. This should return true if the attribute
// is valid for the Decl, report diagnostic message and return false otherwise.
bool MyDeclAttrInfo::diagAppertainsToDecl(Sema& sema,
                                          const ParsedAttr& attr,
                                          const Decl* decl) const {
  if (isa<FunctionDecl>(decl)) {
    this->attrContext.add(decl, &attr);
    return true;
  }

  DiagnosticsEngine& diags = sema.getDiagnostics();

  // We can use the built-in wrong decl warning because it is the most
  // appropriate and also produces the right message.
  diags.Report(attr.getLoc(), diag::warn_attribute_wrong_decl_type)
      << attr << ExpectedFunctionOrMethod;
  return false;
}

// Handle the attribute and associate it with the decl.
ParsedAttrInfo::AttrHandling
MyDeclAttrInfo::handleDeclAttribute(Sema& sema,
                                    Decl* decl,
                                    const ParsedAttr& attr) const {
  decl->addAttr(AnnotateAttr::Create(
      sema.Context, attr.getNormalizedFullName(), attr.getRange()));
  return AttrHandling::AttributeApplied;
}
