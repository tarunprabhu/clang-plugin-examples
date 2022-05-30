# Attributes

This contains a Clang plugin. There are custom attributes created that can be 
applied to functions and statements. The plugin will print information about 
the attributes associated with the corresponding decl or statement to `stderr`.

# Building

See the top-level source directory for build instructions.

Building the plugin will generate the following files:

| File | Purpose |
| ---- | ------- |
| AttributesPlugin.so | This contains the Clang plugin |

# Usage

This must be passed to clang using -fplugin.
An example invocation would be as follows:

```
    clang -fplugin=/path/to/AttributesPlugin.so \
         ...
```

where `...` are additional flags and/or source files.

# Attributes

There are two custom attributes

| Attribute | Targets |
| ---- | ------- |
| `mydeclattr` | Functions and methods |
| `mystmtattr` | `do`, `for`, `while` loops |

Two different syntaxes are supported. The GNU syntax requires all attributes 
to be enclosed in an `__attribute__` sentinel. The C++11 syntax uses the 
`[[<attr-name>]]` syntax. Namespaced attributes in C++11 are also supported.
The namespace in this case is `myns`.

# Example

```
[[myns::mydeclattr]]
void init(int a[3][3], int b[3][3], int c[3][3]) {
  [[mystmtattr]] 
  for (unsigned i = 0; i < 3; i++) {
    [[myns::mystmtattr]] 
    for (unsigned j = 0; j < 3; j++) {
      a[i][j] = i * 3 + j + 1;
      b[i][j] = 10 - a[i][j];
      c[i][j] = 0;
    }
  }
}

int main(int argc, char* argv[]) {
  int a[3][3];
  int b[3][3];
  int c[3][3];

  init(a, b, c);
}
```

# Notes

This has not been tested extensively. One possiblity that has not been tested
is the result of adding the `mydeclattr` attribute to a template function. 
The attribute ought to be applied to all instantiations of the template, but the
way the plugin is currently implemented, it may not be.
