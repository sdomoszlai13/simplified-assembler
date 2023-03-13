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


char typeInString(const string& s){

    // Function to detect what kind of data is stored in a string
    // (to which data type the string can be safely converted)

	regex float_rx {"-?(\\d+)?\\.\\d+"};
	regex float_sci_rx {"-?(\\d+)?\\.\\d+e(\\+|-)?\\d+|-?\\d+e-\\d+"};
	regex int_rx {"-?\\d+"};
	regex str_rx {"\\\".+\\\""};

	if (regex_match(s, float_rx) || regex_match(s, float_sci_rx)){
		return 'f';
	}

	else if (regex_match(s, int_rx)){
		return 'i';
	}

	else {
		return 's';
	}
}


void mov(const string& line){

    // Mov function

    string arg_1 = line.substr(4, 2); // in a valid mov, arg_1 is always stored here
	string arg_2 = line.substr(8);
	regex reg {"(a|b|c)x"};

    if (regex_match(arg_2, reg)){
        arg_2 = regs[arg_2];
    }

	regs[arg_1] = arg_2;
}


void push(const string& line){

    // Push function

    if (stack_ptr == &data_stack[STACK_SIZE]-1){
        throw out_of_range ("Stack full! Can't push. Halting interpreter.\n");
    }


    string str = line.substr(5);
    regex reg {"(a|b|c)x"};

    if (regex_match(str, reg)){
        stack_ptr++;
        *stack_ptr = regs[str];
    }

    else{
        cout << str << "\n";
        stack_ptr++;
        *stack_ptr = str;
    }
}


void pop(const string& line){

    // Pop function

    if (stack_ptr == &data_stack[0]){
        throw out_of_range ("Stack empty! Can't pop. Halting interpreter.\n");
    }

    string arg_1 = line.substr(4, 2);

	regs[arg_1] = *stack_ptr;
	if (stack_ptr != &data_stack[0])
        stack_ptr--;
}


void add(const string& line){

	// Addition function

    string arg_1 = line.substr(4, 2);
	string arg_2 = line.substr(8);
	regex reg {"(a|b|c)x"};

	char typeof_arg_1 = typeInString(regs[arg_1]);
	char typeof_arg_2;

	if (regex_match(arg_2, reg)){
        typeof_arg_2 = typeInString(regs[arg_2]);
        arg_2 = regs[arg_2];
	}

	else{
        typeof_arg_2 = typeInString(arg_2);
	}


	if (typeof_arg_1 == 's' && typeof_arg_2 == 's'){
		regs[arg_1] = regex_replace(regs[arg_1] + arg_2, regex("\\\"\\\""), "");
	}

	else if (typeof_arg_1 == 's' || typeof_arg_2 == 's'){
		throw invalid_argument("\nException: can't add string to numeric value!\n");
	}

	else if (typeof_arg_1 == 'i' && typeof_arg_2 == 'i'){
		regs[arg_1] = std::to_string(stoi(regs[arg_1]) + stoi(arg_2));
	}

	else{
		regs[arg_1] = to_string(stof(regs[arg_1]) + stof(arg_2));
	}
}


void sub(const string& line){

    // Subtraction function

    string arg_1 = line.substr(4, 2);
	string arg_2 = line.substr(8);
	regex reg {"(a|b|c)x"};

	char typeof_arg_1 = typeInString(regs[arg_1]);
	char typeof_arg_2;

	if (regex_match(arg_2, reg)){
        typeof_arg_2 = typeInString(regs[arg_2]);
        arg_2 = regs[arg_2];
	}

	else{
        typeof_arg_2 = typeInString(arg_2);
	}


	if (typeof_arg_1 == 's' || typeof_arg_2 == 's'){
		throw invalid_argument("\nException: can't subtract strings!\n");
	}

	else if (typeof_arg_1 == 'i' && typeof_arg_2 == 'i'){
		regs[arg_1] = std::to_string(stoi(regs[arg_1]) - stoi(arg_2));
	}

	else{
		regs[arg_1] = to_string(stof(regs[arg_1]) - stof(arg_2));
	}
}


int commandProcessor(const string& line){

    // Check if next line of file is valid assembler code. This is done with regex_match.
    // If the line is valid code, the function corresponding to the command is called.
    //
    // Most of the error handling is done here. Invalid commands are caught by using
    // proper regexs (everything not matching any regex is an invalid command).
    // Invalid arguments are detected by the called functions that can throw
    // the invalid_argument exception
    //
    // Arithmetic operations of floats and ints as well as addition of strings is achieved
    // by the arithmetic operators that are overloaded for primitive types by default

    regex valid_mov {"mov (a|b|c)x, (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(\\\".+\\\")|(-?\\d+e-\\d+))"};
    regex valid_push {"push (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(\\\".+\\\")|(-?\\d+e-\\d+))"};
    regex valid_pop {"pop (a|b|c)x"};
    regex valid_add {"add (a|b|c)x, (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(\\\".+\\\")|(-?\\d+e-\\d+))"};
    regex valid_sub {"sub (a|b|c)x, (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(-?\\d+e-\\d+))"};
    regex valid_div {"div (a|b|c)x, (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(-?\\d+e-\\d+))"};
    regex valid_mul {"mul (a|b|c)x, (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(-?\\d+e-\\d+))"};
    regex valid_print {"print (((a|b|c)x)|(-?(\\d+)?(\\.\\d+(e(\\+|-)?\\d+)?)?)|(\\\".+\\\")|(-?\\d+e-\\d+))"};
    regex valid_command {"(mov|mov .+)|(push|push .+)|(pop|pop .+)|(add|add .+)|(sub|sub .+)|(div|div .+)|(mul|mul .+)|(print|print .+)"};


    // MOV
    if (regex_match(line, valid_mov)){
        try{
            mov(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    // PUSH
    else if (regex_match(line, valid_push)){
        try{
            push(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }

        catch(out_of_range& i){
            cerr << i.what() << "\n";
            return -1;
        }
    }


    // POP
    else if (regex_match(line, valid_pop)){//check if it's a valid 'pop' command
        try{
            pop(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }

        catch(out_of_range& i){
            cerr << i.what() << "\n";
            return -1;
        }
    }


    // ADD
    else if (regex_match(line, valid_add)){//check if it's a valid 'add' command
        try{
            add(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    // SUBTRACT
    else if (regex_match(line, valid_sub)){
        try{
            sub(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    // DIVIDE
    else if (regex_match(line, valid_div)){
        try{
            div(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    // MULTIPLY
    else if (regex_match(line, valid_mul)){
        try{
            mul(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    // PRINT
    else if (regex_match(line, valid_print)){
        try{
            print(line);
            return 0;
        }

        catch(invalid_argument& e){
            cerr << e.what() << "\n";
            return -1;
        }
    }


    else if (line == ""){
        return 0;
    }


    else{
        if (regex_match(line, valid_command)){
            cout << "\nInvalid argument(s) - halting interpreter.\n";
        }
        else{
            cout << "\nInvalid command - halting interpreter.\n";
        }
        return -1;
    }
}


int main(int argc, char* argv[]){

    try{
        std::string input_file_name {argv[1]}; // Initialize string with input file name (second command line argument)
        regex valid_file_name {".+\\.txt"}; //Check if file name ends with .txt (with regex)

        if (!regex_match(input_file_name, valid_file_name))
            throw invalid_argument("\nError: file " + input_file_name + ": " + "file extension not allowed. Only .txt files can be read.\n");

        string next_line;
        ifstream if_stream (input_file_name);

        // Check if file exists
        if (!if_stream.good())
            throw invalid_argument("\nError: can't read from " + input_file_name + ": there is no such file.\n");

        int error_present {0}; // variable to detect error thrown by commandProcessor()
        int line_num {1}; // variable for output of line number where error occurred

        while (getline(if_stream, next_line)){ // read next line until end of file is reached
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