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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_CONSUMER_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_CONSUMER_H

#include <clang/AST/ASTConsumer.h>

#include "AttrContext.h"
#include "Visitor.h"

// The consumer class does nothing, but merely calls the visitor class to
// traverse the entire translation unit.
class Consumer : public clang::ASTConsumer {
private:
  Visitor visitor;

public:
  explicit Consumer(clang::CompilerInstance& ci, AttrContext& attrContext);

  virtual void HandleTranslationUnit(clang::ASTContext& context);
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_CONSUMER_H
