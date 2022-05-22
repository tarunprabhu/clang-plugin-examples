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

#ifndef CLANG_PLUGIN_EXAMPLES_AST_IR_CONTEXT_H
#define CLANG_PLUGIN_EXAMPLES_AST_IR_CONTEXT_H

#include <clang/AST/Decl.h>

#include <map>
#include <vector>

namespace clang {
    class CompilerInstance;
}

// This class is intended to contain references to the AST nodes to be used
// by any LLVM passes.
class AstIrContext {
private:
    // Keep a pointer to the compiler instance because all the other objects
    // (SourceManager, ASTContext etc.) are assessible from it.
    clang::CompilerInstance& ci;

    // The FunctionDecl's are marked const to highlight that the AST should
    // not be modified.
    std::map<std::string, const clang::FunctionDecl*> decls_;

    // Plugin option that should also affect the LLVM pass.
    bool printBasename_;

public:
    AstIrContext(clang::CompilerInstance& ci, bool printBasename);
    ~AstIrContext() = default;

    clang::CompilerInstance& getCompilerInstance();
    clang::ASTContext& getAstContext();
    clang::SourceManager& getSourceManager();

    void addDecl(const std::string& mangled, const clang::FunctionDecl* decl);
    const clang::FunctionDecl* getDecl(clang::StringRef mangled) const;

    bool printBasename() const;
};

#endif // CLANG_PLUGIN_EXAMPLES_AST_IR_MATCH_H
