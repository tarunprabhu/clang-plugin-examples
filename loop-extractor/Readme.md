# LoopExtractor

This contains a Clang plugin. The plugin consists of a PragmaHandler to process
pragma's of the form `#pragma extract`. These are associated with a loop 
(`for`, `while`, and `do` loops are recognized). The plugin will print the 
line numbers of the `pragma` and the loop with which they are 
associated.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the following files:

| File | Purpose |
| ---- | ------- |
| LoopExtractorPlugin.so | This contains the Clang plugin |

# Usage

This must be passed to clang using -fplugin.
An example invocation would be as follows:

```
    clang -fplugin=/path/to/AstIrMatchPlugin.so \
         ...
```

where `...` are additional flags and/or source files.

# Notes

This has not been tested extensively (for example when a `pragma` is inside 
code that is expanded from a template), so it might not work in all cases. 
It is intended to be an example of how to use pragma handlers to associate 
custom pragmas with AST nodes. 
