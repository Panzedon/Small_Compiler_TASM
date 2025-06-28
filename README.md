I’ll tell you in advance, the project is very crude and there’s a lot of work to be done.

And so, this version is very different from the original course work, it has already been done so that it would be possible to add support for new commands or new operands for existing commands. 
In this version (for now), it also creates a listing file, while saving bytes for strings to fill the object file (this is for the future). 
A little later I will add comments for structures and functions.

At the same time, I plan to output part of the contents of main.cpp to another file in order to make a separate function to call the “compiler” itself

test1.asm is the assembly code that the compiler analyzes and produces a listing file analysis.lst as output

main.cpp is the main file that calls the FirstView and SecondView functions (from the corresponding files) and to parse a string into tokens.

lexical.cpp and lexical.h - functions for parsing strings into lexemes, analyzing them, and at the same time setting the type for each lexeme are described there.


first_view - functions for the first view are also described there.

second_view for the second view to work.

error_handler - here is a class for outputting errors to the listing file

commands_types - almost all the data for the compiler to work is described here - opcodes for registers, all sorts of templates for different addressing types for analysis. 

inline const std::unordered_map<Commands, std::unordered_map<std::vector<TypeOperand>, CommandInfo>> kCommands - at the moment, this is the most important structure where you can access data for commands, depending on the operands