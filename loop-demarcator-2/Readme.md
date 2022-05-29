# Loop Demarcator - II

This contains a clang plugin that adds sentinel functions around loops that are
associated with custom pragma, `#pragma demarcate`.

The modified code will be printed after all the header files have been included.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the plugin file `LoopDemarcator2Plugin.so`.

# Usage

These must be passed to clang using -fplugin. An example invocation would be as
follows:

```
    clang -fplugin=/path/to/LoopDemarcator2Plugin.so ...
```

where `...` are additional flags and/or source files.

# Comparison to Loop Demarcator

The difference between this and LoopDemarcator is that this uses the Rewriter
class to modify the source being compiled but does not modify the actual AST. 
This can also only realistically be used for a source-to-source transformation.
