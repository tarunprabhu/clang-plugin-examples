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

#ifndef CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_2_CONSUMER_H
#define CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_2_CONSUMER_H

#include <clang/AST/ASTConsumer.h>
#include <clang/Rewrite/Core/Rewriter.h>

#include "Pragmas.h"
#include "Visitor.h"

namespace clang {
class ASTContext;
class CompilerInstance;
} // namespace clang

// This consumer only calls the visitor to traverse the top-level Decl.
// It owns the Pragmas object that is shared the pragma handler and visitor. The
// pragma locations are recorded in that object by the pragma handler and are
// used in the visitor to associate them with them with loops.
class Consumer : public clang::ASTConsumer {
private:
  clang::CompilerInstance& ci;
  clang::Rewriter rewriter;
  Pragmas pragmas;
  Visitor visitor;

public:
  explicit Consumer(clang::CompilerInstance& ci);
  virtual ~Consumer() = default;

  virtual void HandleTranslationUnit(clang::ASTContext& context);
};

#endif // CLANG_PLUGIN_EXAMPLES_LOOP_DEMARCATOR_2_CONSUMER_H
