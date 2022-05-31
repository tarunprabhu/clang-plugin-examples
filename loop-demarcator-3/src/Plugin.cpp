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

#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include "Visitor.h"

using namespace clang;

// The consumer class does nothing, but prints each virtual method as it is
// called while processing a source file.
class Consumer : public ASTConsumer {
private:
  Visitor visitor;

public:
  explicit Consumer(CompilerInstance& ci) : visitor(ci) {
    ;
  }

  virtual ~Consumer() = default;

  // This will get called as soon as each decl is visited. Because of the way
  // the main MultiplexConsumer is set up, any changes that are made to the
  // AST here are visible to the CodeGenerator consumer that runs after this
  // (if the main action is a codegen i.e. LLVM-IR gen action).
  //
  // HandleTranslationUnit is only called after everything has been parsed.
  // Any changes made there will not impact the LLVM-IR.
  virtual bool HandleTopLevelDecl(DeclGroupRef g) {
    if (g.isSingleDecl())
      if (FunctionDecl* f = dyn_cast<FunctionDecl>(g.getSingleDecl()))
        visitor.TraverseDecl(f);
    return ASTConsumer::HandleTopLevelDecl(g);
  }
};

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class Plugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci,
                                                 StringRef) override {
    return std::make_unique<Consumer>(ci);
  }

  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    for (const std::string& arg : args)
      if (arg == "-help")
        llvm::errs() << "\nAdds sentinel functions around all loops.";
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

// Register the plugin. This makes the plugin name available for use on the
// command line. By overriding the getActionType() method in the specialized
// PluginASTAction, the plugin can be run automatically.
static FrontendPluginRegistry::Add<Plugin>
    X("loop-demarcator-3", "Demarcates all loops with a sentinel.");
