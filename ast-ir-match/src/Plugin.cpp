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

#include <clang/AST/Mangle.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>

#include <llvm/Support/raw_ostream.h>

#include <memory>

#include "AstIrContext.h"
#include "Singleton.h"

using namespace clang;

// The visitor visits the FunctionDecl's and saves those that are defined
// in the source file that is currently being processed.
class Visitor : public RecursiveASTVisitor<Visitor> {
private:
  std::string srcFile;
  SourceManager& srcMgr;
  MangleContext& mangler;
  AstIrContext& astIrContext;

private:
  std::string getMangledName(FunctionDecl* decl) {
    if (decl->getNameAsString() == "main")
      return "main";

    std::string s;
    llvm::raw_string_ostream ss(s);
    this->mangler.mangleName(GlobalDecl(decl), ss);
    return ss.str();
  }

  StringRef getFileName(FunctionDecl* decl) {
    FullSourceLoc loc(decl->getBeginLoc(), this->srcMgr);
    if (const FileEntry* entry = loc.getFileEntry())
      return entry->getName();
    return "<unknown>";
  }

public:
  explicit Visitor(CompilerInstance& ci, StringRef srcFile, bool printBasename)
      : srcFile(srcFile), srcMgr(ci.getSourceManager()),
        mangler(*ci.getASTContext().createMangleContext()),
        astIrContext(createSingletonAstIrContext(ci, printBasename)) {
    ;
  }

  virtual ~Visitor() = default;

  bool VisitFunctionDecl(FunctionDecl* decl) {
    if (decl->hasBody() and (getFileName(decl) == this->srcFile))
      if (not(isa<CXXConstructorDecl>(decl) or isa<CXXDestructorDecl>(decl)))
        this->astIrContext.addDecl(getMangledName(decl), decl);

    return true;
  }
};

// The consumer does nothing but traverse the top-level decl to get at all
// the decls below.
class Consumer : public ASTConsumer {
private:
  CompilerInstance& ci;
  Visitor visitor;

public:
  explicit Consumer(CompilerInstance& ci, StringRef srcFile, bool printBasename)
      : ci(ci), visitor(ci, srcFile, printBasename) {
    ;
  }

  virtual ~Consumer() = default;

  virtual void HandleTranslationUnit(ASTContext& context) {
    // If there are parse errors in the file, they will be recorded in the
    // diagnostics. Since this will not attempt to fix those, don't go any
    // further here.
    if (this->ci.getDiagnostics().getNumErrors())
      return;

    visitor.TraverseDecl(context.getTranslationUnitDecl());
  }
};

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class PluginAstIrMatch : public PluginASTAction {
private:
  // Parameters set depending on command-line options passed to the plugin.
  bool printBasename;

public:
  explicit PluginAstIrMatch() : printBasename(false) {
    ;
  }

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& compiler,
                                                 StringRef srcFile) override {
    // Since this is done before codegen, clang might delete the ASTContext to
    // reduce memory usage. But we need it around because the IR passes that
    // run afterwards will need it.
    compiler.getFrontendOpts().DisableFree = true;

    llvm::errs() << "Source: " << srcFile << "\n";

    return std::make_unique<Consumer>(compiler, srcFile, printBasename);
  }

  // Arguments to the plugin can be passed using -plugin-arg.
  //
  //    clang -Xclang -plugin-arg-AstIrMatch -Xclang argName
  //          -Xclang -plugin-arg-AstIrMatch -Xclang argValue
  //
  // The documentation says that it should be possible to do this with
  //
  //    clang -fplugin-arg-AstIrMatch-argName=argValue
  //
  // But for some reason that did not work. It's probably a stupid mistake
  // somewhere and if I find it, I will fix it.
  virtual bool ParseArgs(const CompilerInstance&,
                         const std::vector<std::string>& args) override {
    llvm::errs() << "Plugin AstIrMatch got " << args.size() << " arguments."
                 << "\n";
    for (const std::string& arg : args)
      if (arg == "-print-basename")
        this->printBasename = true;
      else if (arg == "-help")
        llvm::errs() << "\nThis is an example plugin to show how the AST "
                     << "can be kept alive and used by LLVM IR passes. It "
                     << "simply looks up the source location of functions "
                     << "defined in the source file being compiled and prints "
                     << "that location to the terminal."
                     << "\n\n"
                     << "The plugin can be passed the following optional "
                     << "arguments"
                     << "\n\n"
                     << "    -print-basename  Print only the basename of the "
                     << "source file where the function "
                     << "\n"
                     << "                     is defined instead of the "
                     << "print the entire path"
                     << "\n\n\n";
    return true;
  }

  // This will be run before the main action. If the main action is codegen (in
  // clang-speak, codegen is understood to be LLVM-IR generation), this will be
  // run before LLVM-IR is generated.
  virtual ActionType getActionType() override {
    return PluginASTAction::ActionType::AddBeforeMainAction;
  }
};

// Register the plugin. This can now be used with one of the following
//
//     clang -fplugin=/path/to/AstIrMatchPlugin.so
//
//   OR
//
//     clang -Xclang -plugin -Xclang /path/to/AstIrMatchPlugin.so
//
// See the ParseArgs function for command-line options that can be passed to
// this.The name of the shared object is set in meson.build. If the name is
// changed there, this will have to change.
//
static FrontendPluginRegistry::Add<PluginAstIrMatch>
    FunctionAction("AstIrMatch",
                   "Matches an LLVM-IR function to its corresponding AST node");
