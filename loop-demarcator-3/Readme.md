# Loop Demarcator - III

This adds sentinel functions around all loops. The sentinel functions are 
reflected in the IR that is generated. Apart from the absence of pragmas, the 
main difference between this and the other loop demarcators is that in those, 
the sentinel functions that are added are not reflected in the IR and those 
methods are only suitable for source-to-source transformations.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the plugin file `LoopDemarcator3Plugin.so`.

# Usage

These must be passed to clang using -fplugin. An example invocation would be as
follows:

```
    clang -fplugin=/path/to/LoopDemarcator3Plugin.so ...
```

where `...` are additional flags and/or source files.

# Notes

If this file is used with linking, it will almost certainly fail with 
undefined function errors since the sentinel functions `__enterLoop()` and 
`__exitLoop()` will not have been defined (unless you define/provide them). 
