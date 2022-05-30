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

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Stmt.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

using namespace clang;

class Visitor : public RecursiveASTVisitor<Visitor> {
private:
  CompilerInstance& ci;

private:
  // Wrap the statement only if it needs wrapping i.e. if it is not a
  // compound statement.
  CompoundStmt* maybeWrapStmt(Stmt* stmt) {
    if (not isa<CompoundStmt>(stmt))
      return CompoundStmt::Create(
          ci.getASTContext(), {stmt}, stmt->getBeginLoc(), stmt->getEndLoc());
    return nullptr;
  }

public:
  explicit Visitor(CompilerInstance& ci) : ci(ci) {
    ;
  }

  virtual ~Visitor() = default;

  bool VisitForStmt(ForStmt* forStmt) {
    if (Stmt* newBody = this->maybeWrapStmt(forStmt->getBody()))
      forStmt->setBody(newBody);
    return true;
  }

  bool VisitDoStmt(DoStmt* doStmt) {
    if (Stmt* newBody = this->maybeWrapStmt(doStmt->getBody()))
      doStmt->setBody(newBody);
    return true;
  }

  bool VisitWhileStmt(WhileStmt* whileStmt) {
    if (Stmt* newBody = this->maybeWrapStmt(whileStmt->getBody()))
      whileStmt->setBody(newBody);
    return true;
  }

  // Don't put braces around an else if the only statement within it is an
  // if. While it is not incorrect to do so, the idea is to put braces in
  // a more "natural" manner. Putting braces in an if-else-if ladder, and
  // turning this
  //
  //     if (cond0)
  //     else if (cond1)
  //     else if (cond2)
  //     else
  //
  // into something like this
  //
  //     if (cond0) {
  //     } else {
  //         if (cond1) {
  //         } else {
  //             if (cond2) {
  //             } else {
  //             }
  //         }
  //     }
  //
  // is, arguably, not terribly natural. The following is, again arguably,
  // better.
  //
  //     if (cond0) {
  //     } else if (cond1) {
  //     } else if (cond2) {
  //     } else {
  //     }
  //
  bool VisitIfStmt(IfStmt* ifStmt) {
    if (Stmt* then = ifStmt->getThen())
      if (Stmt* newThen = this->maybeWrapStmt(then))
        ifStmt->setThen(newThen);

    if (Stmt* els = ifStmt->getElse())
      if (not isa<IfStmt>(els))
        if (Stmt* newEls = this->maybeWrapStmt(els))
          ifStmt->setElse(newEls);

    return true;
  }
};

// The consumer class does nothing, but merely calls the visitor class to
// traverse the entire translation unit.
class Consumer : public ASTConsumer {
private:
  CompilerInstance& ci;
  Visitor visitor;

public:
  explicit Consumer(CompilerInstance& ci) : ci(ci), visitor(ci) {
    ;
  }

  virtual ~Consumer() = default;

  virtual void HandleTranslationUnit(ASTContext& context) {
    // If there are parse errors in the file, they will be recorded in the
    // diagnostics. Since this will not attempt to fix those, don't go any
    // further here.
    if (this->ci.getDiagnostics().getNumErrors())
      return;

    // This is a DeclContext corresponding to the top level of the source file
    // being compiled.
    TranslationUnitDecl* tu = context.getTranslationUnitDecl();

    visitor.TraverseDecl(tu);

    // This will dump everything in the top-level decl. This will include
    // everything that was pulled in from include files.
    tu->dumpDeclContext();
  }
};

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class Plugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& compiler,
                                                 StringRef) override {
    return std::make_unique<Consumer>(compiler);
  }

  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    for (const std::string& arg : args)
      if (arg == "-help")
        llvm::errs() << "\nThis is an example plugin to show how a plugin can "
                     << "be used to modify the AST. In this case, it will add "
                     << "braces to loops and conditional statements that do "
                     << "not have them. The modified source will be printed to "
                     << "stderr\n\n";
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
    X("add-braces",
      "Adds braces to loops and conditional statements if they "
      "are not present. Will add not braces to switch statements.");
