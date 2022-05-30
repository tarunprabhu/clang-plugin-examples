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

# The languages here are both C and C++ because the dependency on LLVM
# requires both languages to be enabled.
project('clang-plugin-examples', ['c', 'cpp'],
        version: '0.5',
        license: 'Apache-2.0',
        default_options: [
          'cpp_std=c++14',
          'warning_level=3'
        ],
        meson_version: '>=0.55')

# The default value of with-llvm-config is 'llvm-config'. If the user specifies
# a binary to use, try to use it first. If the binary could not be found or if
# it does not seem to be a valid program, revert to the first llvm-config
# executable found from $PATH.
llvm_config_bin = get_option('with-llvm-config')
llvm_config = find_program([llvm_config_bin, 'llvm-config'], required: true)

llvm_version = run_command(llvm_config, '--version', check: true)\
                          .stdout().strip()
message('LLVM version: ' + llvm_version)

# TODO: At some point, make this a user-configurable option. Given how fast
# LLVM changes and breaks itself, there is every likelihood that changes will
# need to be made to the examples depending on the version.
if (not (llvm_version.version_compare('>=13') and
         llvm_version.version_compare('<14')))
  error('LLVM version not supported')
endif

llvm_incdir = run_command(llvm_config, '--includedir', check: true)\
                         .stdout().strip()
message('LLVM include dir: ' + llvm_incdir)

llvm_libdir = run_command(llvm_config, '--libdir', check: true)\
                         .stdout().strip()
message('LLVM lib dir: ' + llvm_libdir)

cxx = meson.get_compiler('cpp')
libclang = cxx.find_library('libclang', dirs: llvm_libdir, required: true)
libclang_cpp = cxx.find_library('libclang-cpp', dirs: llvm_libdir, required: true)
libllvm = cxx.find_library('libLLVM', dirs: llvm_libdir, required: true)

incdirs = include_directories([llvm_incdir, 'common'])
extlibs = [libclang, libclang_cpp, libllvm]

subdir('common')
subdir('add-braces')
subdir('ast-ir-match')
subdir('attributes')
# subdir('instrument')
subdir('loop-demarcator')
subdir('loop-demarcator-2')
subdir('loop-extractor')
subdir('trace-consumer')
