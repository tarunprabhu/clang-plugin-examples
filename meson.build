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

# TODO: At some point, make this a user-configurable option. Given how fast
# LLVM changes and breaks itself, there is every likelihood that changes will
# need to be made to the examples depending on the version.
llvm = dependency('llvm', version: ['>=13'])

# Clang does not use pkgconfig and cmake cannot find it either.
# FIXME: This is not the right way of doing things because it will not be able
# to use an LLVM installation in a non-system path. At some point, should
# really try to support that.
libclang = meson.get_compiler('cpp').find_library('libclang')
libclang_cpp = meson.get_compiler('cpp').find_library('libclang-cpp')

incdir = include_directories([llvm.get_variable(configtool: 'includedir'),
                              'common'])

subdir('common')
# subdir('loop-demarcator')
subdir('ast-ir-match')
