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

using namespace clang;

// The consumer class does nothing, but prints each virtual method as it is
// called while processing a source file.
class Consumer : public ASTConsumer {
public:
  virtual ~Consumer() = default;

  virtual void Initialize(ASTContext&) {
    llvm::outs() << "Initialize"
                 << "\n";
  }

  virtual bool HandleTopLevelDecl(DeclGroupRef g) {
    llvm::outs() << "HandleTopLevelDecl"
                 << "\n";
    return ASTConsumer::HandleTopLevelDecl(g);
  }

  virtual void HandleInlineFunctionDefinition(FunctionDecl*) {
    llvm::outs() << "HandleInlineFunctionDefinition"
                 << "\n";
  }

  virtual void HandleInterestingDecl(DeclGroupRef) {
    llvm::outs() << "HandleInterestingDecl"
                 << "\n";
  }

  virtual void HandleTranslationUnit(ASTContext&) {
    llvm::outs() << "HandleTranslationUnit"
                 << "\n";
  }

  virtual void HandleTagDeclDefinition(TagDecl*) {
    llvm::outs() << "HandleTagDeclDefinition"
                 << "\n";
  }

  virtual void HandleTagDeclRequiredDefinition(const TagDecl*) {
    llvm::outs() << "HandleTagDeclRequiredDefinition"
                 << "\n";
  }

  virtual void HandleCXXImplicitFunctionInstantiation(FunctionDecl*) {
    llvm::outs() << "HandleCXXImplicitFunctionInstantiation"
                 << "\n";
  }

  virtual void HandleTopLevelDeclInObjCContainer(DeclGroupRef) {
    llvm::outs() << "HandleTopLevelDeclInObjCContainer"
                 << "\n";
  }

  virtual void HandleImplicitImportDecl(ImportDecl*) {
    llvm::outs() << "HandleImplicitImportDecl"
                 << "\n";
  }

  virtual void CompleteTentativeDefinition(VarDecl*) {
    llvm::outs() << "CompleteTentativeDefinition"
                 << "\n";
  }

  virtual void CompleteExternalDeclaration(VarDecl*) {
    llvm::outs() << "CompleteExternalDeclaration"
                 << "\n";
  }

  virtual void AssignInheritanceModel(CXXRecordDecl*) {
    llvm::outs() << "AssignInheritanceModel"
                 << "\n";
  }

  virtual void HandleCXXStaticMemberVarInstantiation(VarDecl*) {
    llvm::outs() << "HandleCXXStaticMemberVarInstantiation"
                 << "\n";
  }

  virtual void HandleVTable(CXXRecordDecl*) {
    llvm::outs() << "HandleCXXStaticMemberVarInstantiation"
                 << "\n";
  }
};

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class Plugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance&,
                                                 StringRef) override {
    return std::make_unique<Consumer>();
  }

  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    for (const std::string& arg : args)
      if (arg == "-help")
        llvm::errs() << "\nThis is an example plugin that traces the calls "
                        "made to an ASTConsumer's methods as it processes "
                        "a source file. The function names will be printed to "
                        "stdout as they are called.";
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
    X("trace-consumer", "Traces an ASTConsumer as it proceses a source file.");
