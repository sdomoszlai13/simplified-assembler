# Basic Assembler

**This is a simple assembler implemented in C++ to make a quick comprehension of the assembly language possible. Input is given in a text file with assembly commands that are executed by this interpreter.**


The program takes a .txt file as a command line argument and executes the included assembly code line by line.
</br>
</br>

## Instructions to run:

Run `interpret.exe <filename>` in the command prompt, where <filename> ends with .txt.

Optional: set size of stack. This can be done in the source code (modify preprocessor directive). Default value is 100.
</br>


Only .txt files are accepted. If the name of the .txt file contains whitespaces, the whole file name must be enclosed in quotation marks ("file name.txt"). Empty lines are skipped, empty files are allowed. Leading or trailing whitespaces are not allowed and an exception is thrown.
</br>

**Commands known by the interpreter:**

* `mov <dst, src>`

* `push <src>`

* `pop <dst>`

* `add <dst, src>`

* `sub <dst, src>`

* `div <dst, src>`

* `mul <dst, src>`

* `print <src>`

**Legend**

* `<dst>`: register

* `<src>`: register, string literal or numerical value
</br>

Only lower case commands are accepted, otherwise the unknown command exception is thrown. Both floating-point numbers and integers can be entered in decimal format. Floating-point numbers can also be entered in scientific format.
</br>

Note: numbers like 4e-3 are floatin-point numbers, so they are accepted. Compiler needs to know the ISO C++11 standard.
