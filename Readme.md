# clang-plugin-examples

This contains examples of clang plugins that are used in different ways. It is 
intended to be a demonstration of what can be done with clang plugins. It is 
not intended to be complete and parts of this repository may end up being 
obsoleted. In all cases, refer to the official clang documentation for the 
latest information.

# Requirements

This has only been tested with Clang/LLVM 13.0. At the time of writing, 
Clang/LLVM 15.0 has additional changes that may necessitate changes to some 
of these plugins.

    - Clang/LLVM 13.0
    - Meson >= 0.55

# Obtaining

Clone this repository into some directory `/path/to/source`

```
cd /path/to/source
git clone git@github.com:tarunprabhu/clang-plugin-examples
```

# Building

First, create a build directory `build-dir` somewhere.

```
mkdir /path/to/build-dir
```

Then configure the build directory from within the top-level source directory.
Additional options may be passed to Meson if required. See the Meson 
documentation for details.

```
cd /path/to/source/clang-plugin-examples
meson /path/to/build-dir
```

In order to use LLVM installed to a non-standard location, use the 
`with-llvm-config` option and pass the full path to the `llvm-config` executable.

```
meson -Dwith-llvm-config=/path/to/llvm-config /path/to/build-dir
```

Finally, switch to the build directory and compile the plugins.

```
cd /path/to/build-dir
meson compile
```
