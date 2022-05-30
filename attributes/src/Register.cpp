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

#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Lex/Pragma.h>

#include "DeclAttr.h"
#include "Plugin.h"
#include "StmtAttr.h"

using namespace clang;

// Register the attributes. It would be nicer if these could be added manually
// to the preprocessor/compiler instance the way that PragmaHandlers can. Until
// then, they have to be registered this way.
static ParsedAttrInfoRegistry::Add<MyDeclAttrInfo>
    MyDeclAttrX("mydeclattr", "Custom attribute for decls.");

static ParsedAttrInfoRegistry::Add<MyStmtAttrInfo>
    MyStmtAttrX("mystmtattr", "Custom attribute for stmts.");

// Register the AST action. This by itself will not result in the action being
// run automatically. That can only be done by implementing the getActionType()
// method in the specialized Plugin class.
static FrontendPluginRegistry::Add<Plugin>
    PluginX("my-attributes",
            "Explore the use of custom attributes in plugins.");
