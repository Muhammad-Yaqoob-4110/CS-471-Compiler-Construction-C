# Without Using Makefile
To create the object file of lexer
```
g++ -c ./src/lexer/lexer.cpp
```

To create the object file of parser
```
g++ -c ./src/parser/parser.cpp
```

To create the object file of assembly
```
g++ -c ./src/assembly/acg.cpp
```

To create the object file of intermediate code
```
g++ -c ./src/intermediate/icg.cpp
```

To create the object file of symbol Table
```
g++ -c ./src/symboltable/symbTable.cpp
```

To create the object file of main program
```
g++ -c ./main.cpp
```

To, create an exe file, We need to to link all the object files

```
g++ main.o lexer.o icg.o symbTable.o acg.o parser.o -o main.exe
```

To run the test the program, we need to provide a file name to our program.
```
./main.exe program.txt
```


# With Makefile
Crate a dircotry named bin in the root folder and then. Just write the make command, it will compile everything.
```
make
```
