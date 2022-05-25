# AddBraces

This contains a clang plugin that adds braces to loops and conditional 
statements that do not have them. This will not add braces to switch statements.
The modified code will be printed to stderr. 

The modified code will be printed after all the header files have been included.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the plugin file `AddBracesPlugin.so`.

# Usage

These must be passed to clang using -fplugin. An example invocation would be as
follows:

```
    clang -fplugin=/path/to/AddBracesPlugin.so ...
```

where `...` are additional flags and/or source files.
