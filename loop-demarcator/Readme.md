# Loop Demarcator

This contains a clang plugin that adds sentinel functions around loops that are
associated with custom pragma, `#pragma demarcate`.

The modified code will be printed after all the header files have been included.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the plugin file `LoopDemarcatorPlugin.so`.

# Usage

These must be passed to clang using -fplugin. An example invocation would be as
follows:

```
    clang -fplugin=/path/to/LoopDemarcatorPlugin.so ...
```

where `...` are additional flags and/or source files.
