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

#include <llvm/IR/Function.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>

#include "AstIrContext.h"
#include "Config.h"
#include "Singleton.h"

using namespace llvm;

// This pass prints the source location of functions whose corresponding
// FunctionDecl's were recorded in the Plugin FunctionVisitor class.
class FunctionSourceLocPass : public PassInfoMixin<FunctionSourceLocPass> {
private:
  AstIrContext& astIrContext;

private:
  std::string formatPath(std::string path) {
    if (astIrContext.printBasename()) {
      size_t i = path.rfind('/');
      if (i != std::string::npos)
        return path.substr(i + 1);
    }
    return path;
  }

public:
  FunctionSourceLocPass(AstIrContext& astIrContext)
      : astIrContext(astIrContext) {
    ;
  }

  PreservedAnalyses run(Module& mod, ModuleAnalysisManager&) {
    llvm::errs() << "In FunctionSourceLoc LLVM pass"
                 << "\n";
    for (llvm::Function& f : mod.functions()) {
      StringRef fname = f.getName();
      if (const clang::FunctionDecl* decl = this->astIrContext.getDecl(fname))
        llvm::errs() << fname << " => "
                     << formatPath(decl->getBeginLoc().printToString(
                            this->astIrContext.getSourceManager()))
                     << "\n";
    }
    return PreservedAnalyses::all();
  }
};

static void registerPass(ModulePassManager& mpm,
                         PassBuilder::OptimizationLevel) {
  // The optimization level can be used to determine whether or not it is
  // appropriate to add this pass at a given optimization level. In this case,
  // it should always be run, so the argument is ignored.
  mpm.addPass(FunctionSourceLocPass(getSingletonAstIrContext()));
}

extern "C" ::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION,
          "FunctionSourceLocPass",
          CLANG_PLUGIN_EXAMPLES_VERSION,
          [](PassBuilder& pb) {
            pb.registerPipelineStartEPCallback(registerPass);
          }};
}
