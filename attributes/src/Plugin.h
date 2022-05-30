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

#ifndef CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_PLUGIN_H
#define CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_PLUGIN_H

#include <clang/Frontend/FrontendAction.h>

// This is the main plugin class. It does nothing much beyond returning a
// specialized ASTConsumer object.
class Plugin : public clang::PluginASTAction {
protected:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance& ci,
                    clang::StringRef file) override;

  virtual bool ParseArgs(const clang::CompilerInstance& ci,
                         const std::vector<std::string>& args) override;

  // By implementing this method, we are ensuring that the plugin is run
  // automatically. The return type will determine when it is run.
  virtual PluginASTAction::ActionType getActionType() override;
};

#endif // CLANG_PLUGIN_EXAMPLES_ATTRIBUTES_PLUGIN_H
