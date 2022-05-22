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

#include "Singleton.h"

// Singleton AstIrContext object. There may be a cleverer way to do this
// without resorting to global variables, but if there is one, I couldn't find
// it. Making this thread-local and static should hopefully mitigate some of
// the unpleasantness of globals. Making this a pointer also minimizes the
// "global" footprint as it were.
thread_local static AstIrContext* astIrContext;

// Get a reference to the singleton AstIrContext. This should only be called
// once the AstIrContext has been created. That should be done in the plugin
// as early as possible.
AstIrContext& getSingletonAstIrContext() {
  return *astIrContext;
}

AstIrContext& createSingletonAstIrContext(clang::CompilerInstance& ci,
                                          bool printBasename) {
  astIrContext = new AstIrContext(ci, printBasename);
  return *astIrContext;
}
