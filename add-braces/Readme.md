# AddBraces

This contains a clang plugin that adds braces to loops and conditional 
statements that do not have them. This will not add braces to switch statements.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the plugin file `AddBracesPlugin.so`.

# Usage

These must be passed to clang using -fplugin. The only way to see that the 
compound statements have been added is to examine the dumped AST. This is 
rather unfortunate. 

An example invocation would be as follows:

```
    clang -fplugin=/path/to/AddBracesPlugin.so \
          -fsyntax-only -Xclang -ast-dump \
         ...
```

where `...` are additional flags and/or source files.
