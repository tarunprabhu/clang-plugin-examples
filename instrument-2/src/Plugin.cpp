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

#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include <llvm/Support/raw_ostream.h>

#include <memory>

#include "Consumer.h"
#include "Visitor.h"

using namespace clang;

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class Plugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci,
                                                 StringRef) override {
    return std::make_unique<instr::Consumer>(ci);
  }

  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    for (const std::string& arg : args) {
      if (arg == "-help") {
        llvm::errs() << "\nThis is an example plugin to show how a region "
                     << "can be demarcated using begin and end pragmas."
                     << ""
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
  //
  // By implementing this method, we are ensuring that the plugin is run
  // automatically.
  virtual ActionType getActionType() override {
    return PluginASTAction::ActionType::AddBeforeMainAction;
  }
};

static FrontendPluginRegistry::Add<Plugin>
    X("instrument",
      "Instrument functions, regions, lines and loops using custom pragmas.");
