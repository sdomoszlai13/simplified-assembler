#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <map>
#include <stdexcept>

using namespace std;

#define STACK_SIZE 100


/*
 * SIMPLIFIED ASSEMBLER INTERPRETER
 *
 * This program reads instructions written in assembly language
 * line by line from a text file and executes the commands.
 *
 * Input: name of .txt file containing the assembly language code.
 * The file name must be given to the program as a command line argument.
 *
 */


// Setup virtual machine with stack size = 100*len(string) and registers ax, bx, cx.
// Everything on the stack and in the registers is stored as strings
vector<string> data_stack(STACK_SIZE + 1); // First string in stack can't be used properly
string* stack_ptr = &data_stack[0];
map<string, string> regs {{"ax", ""}, {"bx", ""}, {"cx", ""}};


int main(int argc, char* argv[]){

    try{
        std::string input_file_name {argv[1]}; // Initialize string with input file name (second command line argument)
        regex valid_file_name {".+\\.txt"}; //Check if file name ends with .txt (with regex);

        if (!regex_match(input_file_name, valid_file_name))
            throw invalid_argument("\nError: file " + input_file_name + ": " + "file extension not allowed. Only .txt files can be read.\n");

        string next_line;
        ifstream if_stream (input_file_name);

        // Check if file exists
        if (!if_stream.good())
            throw invalid_argument("\nError: can't read from " + input_file_name + ": there is no such file.\n");

        int error_present {0}; // variable to detect error thrown by commandProcessor()
        int line_num {1}; // variable for output of line number where error occurred

        while (getline(if_stream, next_line)){ //read next line until end of file is reached
            error_present = commandProcessor(next_line); // process current line
            if (error_present != 0){
                cout << "Error occurred in line " << line_num << ": " << next_line << "\n";
                return EXIT_FAILURE;
            }
            line_num++;
        }
    }


    catch(invalid_argument& e){
        cout << e.what();
        return EXIT_FAILURE;
    }

    catch(logic_error& e){
            cerr << "\nError: no file name provided as command line argument!\n";
            return EXIT_FAILURE;
    }

    return 0;
}