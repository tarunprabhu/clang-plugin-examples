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

#include "StmtAttr.h"
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
    = {{ParsedAttr::AS_GNU, "mystmtattr"},
       {ParsedAttr::AS_CXX11, "mystmtattr"},
       {ParsedAttr::AS_CXX11, "myns::mystmtattr"}};

MyStmtAttrInfo::MyStmtAttrInfo() : attrContext(getSingletonAttrContext()) {
  // Not sure what the attribute kind should be here. I assume that
  // NoSemaHandler means that Sema does not handle this attribute, but I
  // can't find confirmation of it in the source one way or another.
  this->AttrKind = AttributeCommonInfo::NoSemaHandlerAttribute;

  // The number of required arguments for the attribute.
  this->NumArgs = 0;

  // The number of optional arguments for the attribute.
  this->OptArgs = 1;

  // Set to 1 if this attribute can only be applied to types.
  this->IsType = 0;

  // Set to 0 if this attribute can only be applied to Decl's.
  // Set to 1 if this attribute can be applied to Stmt's.
  this->IsStmt = 1;

  // The various spellings of this attribute. This allows the attribute to
  // be specified using different syntaxes. For instance:
  //
  //     GNU        __attribute__((mystmtattr))
  //     C++11      [[mystmtattr]]
  //     Declspec   __declspec(mystmtattr)
  //     Keyword    __mystmtattr
  //     ...
  //
  // I have not managed to get all of them to work, presumably because I am
  // not declaring them correctly, or maybe it needs additional flags to be
  // passed to clang - I am not sure.
  this->Spellings = spellings;
}

// Check if the attribute can be applied to the given Stmt.
// In this case, the attribute is only allowed on loops.
// This should return true if the attribute can be applied to the statement,
// or raise a diagnostic and return false otherwise.
//
// ------------------------- GENERAL PERVERSITY AHEAD ------------------------
//
// 30 May, 2022. (LLVM 13.0)
//
// Although the documentation requires this function to return
// true if the attribute can be applied to the given statement, this always
// return false. If this returns true, it results in Sema attempting to
// process this attribute. Currently, there doesn't seem to be a way for Sema
// to handle custom statement attributes and this ends up throwing a
// compile-time error.
//
// Because of this, the attribute is immediately associated with Stmt if it
// can be.
// ---------------------------------------------------------------------------
//
bool MyStmtAttrInfo::diagAppertainsToStmt(Sema& sema,
                                          const ParsedAttr& attr,
                                          const Stmt* stmt) const {
  if (isa<ForStmt>(stmt) or isa<DoStmt>(stmt) or isa<WhileStmt>(stmt)) {
    this->attrContext.add(stmt, &attr);

    // If not sure why this is returning false, read the comment titled
    // "General Perversity" associated with this function.
    return false;
  }

  DiagnosticsEngine& diags = sema.getDiagnostics();

  // The id will be cached by the DiagnosticsEngine and reused if required,
  // so this operation will not result in an inordinate number of ids being
  // created.
  unsigned id
      = diags.getCustomDiagID(DiagnosticsEngine::Warning,
                              "'%0' attribute can only be applied to loops.");
  diags.Report(attr.getLoc(), id) << attr;
  return false;
}
