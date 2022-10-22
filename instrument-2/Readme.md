# Instrument Plugin

This contains a clang plugin and associated LLVM passes to instrument functions 
and regions. This will insert code to record entries and exits from functions 
and regions. A small library will also be built that provides the instrumentation
functions.

This is intended to demonstrate how a clang plugin and LLVM passes can be used
in tandem to implement an annotation (`#pragma`)based language without modifying
clang or LLVM.

# Building

Building the plugin will generate the following files:

| File | Purpose |
| ---- | ------- |
| InstrumentPlugin.so | The Clang plugin to parse the `#pragma`'s |
| InstrumentPasses.so | The LLVM passes to add the instrumentation code |
| libInstrument.so | The shared library containing the instrumentation code |

# Usage

An example invocation would be as follows:

```
    clang -fplugin=/path/to/InstrumentPlugin.so \
          -fpass-plugin=/path/to/InstrumentPasses.so \
          ... \
          -Wl,-rpath=/path/to/dir/containing/libInstrument.so \
          /path/to/libInstrument.so
```

where `...` are additional flags and source files.

# Instrumentation language

The instrumentation is enabled using custom pragmas. Each pragma has a 
sentinel `instrument` followed by a kind and several, possibly zero, 
clauses. The supported kinds and the clauses for each are listed below.

    * `#pragma instrument function`
    
    This is used to instrument a function. The supported clauses are as follows:
    
        - style(decl | qual | full)
    
        The style describes how the function name is printed. `decl` will print 
        only  the function/method name. `qual` name will print the class name in 
        addition to the function name. `full` will print the fully qualified 
        name which is only relevant for template instantiated functions and will 
        include all the specialized types.

        - args (arg, ... |)
    
        A possibly non-empty list of the function arguments that must be printed
        when the  function is entered. Each element of the list must be the name 
        of a function parameter. They need not be in the same order as they 
        appear in  the funtion parameter list. To suppress printing the 
        arguments use the  args clause with an empty list. If the clause is not
        specified, all the  function arguments will be printed in the order in 
        which they are declared.

    * `#pragma instrument region`
    
    This is used to instrument a region. The pragma must be immediately followed
    by a block statement (a block statement is a possibly empty sequence of 
    statements surrounded by braces). The supported clauses are as follows:
    
        - name (<string-literal>)
        
        The name will be printed when control enters and exits a region. The name
        must be a string literal.
        
    * `#pragma instrument loop`
    
    This is used to instrument a loop. `for`, `while` and `do` loops are 
    supported. 
    
        - name (<string-literal>)
        
        The name will be printed when control enters and exists a loop. The name
        must be a string literal.
        

    * `#pragma instrument line`
    
    This is used to instrument a single line. This merely adds instrumentation 
    that indicates that the line containing the directive was reached. 

# Example

The code below shows how each of the directives can be used

```
    #include <iostream>

    template <typename T>
    void init(T a[3][3], T b[3][3], T c[3][3]) {
        for (unsigned i = 0; i < 3; i++) {
            for (unsigned j = 0; j < 3; j++) {
                a[i][j] = i * 3 + j + 1;
                b[i][j] = 9 - (i * 3 + j);
                c[i][j] = 0;
            }
        }
    }

    #pragma instrument function mode(full) args()
    template <typename T>
    void matmul(const T a[3][3], const T b[3][3], T c[3][3]) {
        #pragma instrument loop name("i")
        for (unsigned i = 0; i < 3; i++)
            #pragma instrument loop name("j")
            for (unsigned j = 0; j < 3; j++)
                #pragma instrument loop name ("k")
                for (unsigned k = 0; k < 3; k++)
                    c[i][j] += a[i][k] * b[k][j];
    }
    
    int main(int argc, char* argv[]) {
        int a[3][3];
        int b[3][3];
        int c[3][3];
        
        #pragma instrument line
        init(a, b, c);
        
        matmul(a, b, c);
        
        #pragma instrument region name("result")
        {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++)
                    std::cout << c[i][j] << " ";
                std::cout << "\n";
            }
        }
        
        return 0;
    }
```

