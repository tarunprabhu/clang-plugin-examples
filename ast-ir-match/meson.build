#
#  Copyright  2022  Tarun Prabhu
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

# Build a common shared library for the singleton and the AstIrContext,
# a shared library for the clang plugin and another and another for the LLVM
# passes. There is no need to have these be separate. They could all be
# combined into a single shared object and it would still work, but this way
# is a bit cleaner and allows the plugin and the passes to be used separately
# if required. It may also save a bit of time when compiling if the plugins
# and passes are doing more complicated things.

lib_common = static_library('AstIrMatchCommon',
                            ['src/AstIrContext.cpp',
                             'src/Singleton.cpp'],
                            install: false,
                            include_directories: incdirs,
                            dependencies: extlibs)

shared_library('AstIrMatchPlugin',
               ['src/Plugin.cpp'],
               name_prefix: '',
               include_directories: incdirs,
               dependencies: extlibs,
               link_with: [lib_common])

shared_library('AstIrMatchPasses',
               ['src/FunctionSourceLocPass.cpp'],
               name_prefix: '',
               include_directories: incdirs,
               dependencies: extlibs,
               link_with: [lib_common])
