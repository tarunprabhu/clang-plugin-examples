# Notes

These are notes that I made when building some of the plugins. It's mostly 
intended as a reference for myself, but it may be of use to anyone trying to 
do anything non-trivial with AST plugins.

# AST design

The AST is not designed to be easily transformable. This is by design and I am
fairly certain I read this somewhere --- if not in the documentation, then
probably on the clang-dev mailing list but I can't find a reference to it now. 

There are some things that can be set such as the body of a loop, the 
conditional expression in an if statement and the like. But these are very high
level. If attempting to insert statements, it is easy to end up in a situation
where changes have to be made recursively all the way to the top of the tree. 

A workaround that currently works but may not always is to use the StmtIterator
in a CompoundStmt. This allows a single statement to be replaced (see 
`loop-demarcator-3` plugin for an example of how this can be done). 

# Plugins vs. tooling

When using AST plugins, the CompilerInstance object creates a MultiplexConsumer.
The MultiplexConsumer is a consumer that wraps seversal ASTConsumer's and 
allows traversing each of them. Depending on the command-line flags passed, 
the MultiplexConsumer will contain a consumer from each of the plugins and 
a consumer for the code generator. The latter's structure and methods are 
not exposed. There is no way to get a list of the consumers contained within
the MultipleConsumer object. 

Because the actual CodeGenerator cannot be obtained, it cannot be made to use 
the modified ASTContext (if indeed, it has been been modified).
