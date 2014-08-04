# libstephen To-Do List

I already find `libstephen` to be a useful tool for my general purpose C
programming.  I use it extensively in my regular expression and parser project,
[`cky`](https://bitbucket.org/brenns10/cky).  The data structure support and
memory allocation tracking features are invaluable every day necessities for
writing C code that I'm comfortable with.  However, there is always room for
improvement.  Most definitely, my library is far from perfect, and I want to
bring it as close as possible to perfection.  As such, here are some of my plans
for the future.

## Better Error Handling

This is a big one.  I currently have a rather half-assed attempt at error
handling right now.  It consists of an error variable with a limited amount of
error flags, and no more information.  It requires the user to be extremely
proactive (`if (CHECK(ERROR_X)) { /* handle error */}`).  I need an error
handling mechanism that is easier to use, includes more information, and can
drop a stack trace when necessary.  Of course, I can't implement try/catch
blocks (well, it's possible with setjmp/longjmp... but terrifying), but I can
make things much easier.

## Wrapped Standard Library Calls

Along with better error handling would be wrapped stdlib calls.  They could
convert stdlib errors into `libstephen` errors, and possible handle some of them
(like printing a stack trace, exiting, etc.).

## Nice String Features

### String Builders

I'd like a way to easily construct strings.  A string builder that handles the
automatic copying and reallocation (similar to an array list, but more efficient
for chars, and with more string-oriented functionality) would be really welcome.

### String Handlers

It would also be nice to have functions that work on `char *` strings, that
provide functionality similar to some of Python's string handling capabilities.
For instance, substrings, a split function, etc.

## "Pickling" Support

This is a far-off possibility.  But, it would be neat to allow data structures
to be "pickled" to a binary file.  It would take a lot of doing, but it may be
possible to follow all pointers via a sort of breadth-first search, and reassign
them to locations in files, and then save all the data to a file.  This is a bit
crazy, but it /might/ be possible.