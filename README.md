# Language of Sparse Matrix Script

This is a small script language as a final project for "Data Structure" at University of Chinese Academy of Science.

## How to build this

```commandline
mkdir build
cd build
cmake ..
make
```

Then you can see an executable file `lspi` at `build/`

## How to use it

Current usage:

`lspi [file] [visualizer-option]`

Execute `file` as a LSPScript program. `visualizer-option` now only supports `--visualize-ast`, and this will output a
file `output.dot` under the same directory.

Use tools like Graphviz to visualize the `output.dot` file.

## Grammar of LSPScript

LSPScript is very simple language currently with a grammar similar to C. The scalar variables are declared with
restricted
word `var` instead of type name and types of variables will be handled automatically at runtime.

LSPScript supports declaring a sparse matrix using `spm<m, n>`, where `m` is the number of rows of the matrix and n is
the number of columns of the matrix.

Both `m` and `n` must be constants. You can use a triplet list to initialize a sparse matrix, as is showed in example
codes.

For now, LSPScript doesn't support any other types, and it doesn't support functions or complex matrix operations
either. These features may be provided in future updates.

