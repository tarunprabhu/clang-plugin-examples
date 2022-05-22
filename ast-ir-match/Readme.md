# AstIrMatch

This contains a Clang plugin and an LLVM pass. The plugin collects the AST
objects for each function that is defined in the source file being compiled.
The LLVM pass associates the AST object with its corresponding LLVM-IR
function and prints the source location of the function from the data present 
in the AST.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the following files:

| File | Purpose |
| ---- | ------- |
| AstIrMatchPlugin.so | This contains the Clang plugin |
| AstIrMatchPasses.so | This contains the LLVM passes that will be run |

# Usage

These must be passed to clang using -fplugin and -fpass-plugin respectively.
An example invocation would be as follows:

```
    clang -fplugin=/path/to/AstIrMatchPlugin.so \
          -fpass-plugin = /path/to/AstIrMatchPasses.so \
         ...
```

where `...` are additional flags and/or source files.

Another useful parameter to pass is -disable-O0-optnone. This is because
clang annotates all functions with optnone at optimization level O0. This can
result in some passes not running. The -disable-O0-optnone flag disables
this behavior. The way it would be used is shown below

```
   clang -fplugin=/path/to/AstIrMatchPlugin.so \
         -fpass-plugin = /path/to/AstIrMatchPasses.so \
         -Xclang -disable-O0-optnone \
         ...
```

In order to pass additional arguments to the plugin, use
`-plugin-arg-AstIrMatch`. The documentation says that
`-fplugin-arg-AstIrMatch-<arg>` should work too, but for some reason, it is
not working for this plugin. It is probably a mistake in the way the plugin
is written but I am not sure what it is. An example command line is:

```
   clang -fplugin=/path/to/AstIrMatchPlugin.so \
         -Xclang -plugin-arg-AstIrMatch -Xclang -arg \
         -fpass-plugin = /path/to/AstIrMatchPasses.so \
         -Xclang -disable-O0-optnone \
         ...
```

If the argument takes an argument it must be specified using `-arg=val`.
If the syntax `-arg val` is desired, each of those should be separated with
calls to `-Xclang -plugin-arg-AstIrMatch`. The plugin needs to be able to
handle such cases in the `ParseArg()` method.

```
   clang -fplugin=/path/to/AstIrMatchPlugin.so \
         -Xclang -plugin-arg-AstIrMatch -Xclang -arg=val \
         -fpass-plugin = /path/to/AstIrMatchPasses.so \
         -Xclang -disable-O0-optnone \
         ...
```

```
   clang -fplugin=/path/to/AstIrMatchPlugin.so \
         -Xclang -plugin-arg-AstIrMatch -Xclang -arg \
         -Xclang -plugin-arg-AstIrMamtch -Xclang val \
         -fpass-plugin = /path/to/AstIrMatchPasses.so \
         -Xclang -disable-O0-optnone \
         ...
```

# Notes

This is really intended to be a demonstration of how to write a Clang plugin
and LLVM passes in tandem. The code is commented to a greater extent than this 
Readme and that is what should be of more interest.
