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

#include "Plugin.h"
#include "Consumer.h"
#include "Singleton.h"

#include <memory>

using namespace clang;

std::unique_ptr<ASTConsumer> Plugin::CreateASTConsumer(CompilerInstance& ci,
                                                       StringRef) {
  return std::make_unique<Consumer>(ci, getSingletonAttrContext());
}

// Return true if the arguments were parsed successfully, false otherwise.
// This will be called before the consumer is created, so the arguments
// obtained as a result of the parsing can be sent on to the consumer.
bool Plugin::ParseArgs(const CompilerInstance&,
                       const std::vector<std::string>& args) {
  for (const std::string& arg : args) {
    if (arg == "-help") {
      llvm::errs() << "\nThis is an example plugin that associates custom "
                   << "attributes on functions and statements."
                   << "\n\n";
    } else {
      llvm::errs() << "Unknown argument: " << arg << "\n";
      return false;
    }
  }
  return true;
}

// This will be run before the main action. If the main action is codegen (in
// clang-speak, codegen is understood to be LLVM-IR generation), this will be
// run before LLVM-IR is generated.
PluginASTAction::ActionType Plugin::getActionType() {
  return PluginASTAction::ActionType::AddBeforeMainAction;
}
