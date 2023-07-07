/*****************************************************************************
TITLE: ASSEMBLER
AUTHOR:   M Shanmukha Priya (2101CS40)
Declaration of Authorship
This file, asm.cpp, is part of the miniproject of CS209/CS210 at the
department of Computer Science and Engg, IIT Patna .
*****************************************************************************/
#include <bits/stdc++.h>

using namespace std;

// Define structure for symbols
struct symbol
{
    string name;
    int address;
    bool set;
    int set_value;
    int used; // later used to determine a warning if the label is unused
};

struct format
{
    int poc;
    int line;
    string instr;
};
// Symbol table
vector<symbol> sym_table;
vector<string> errors;
vector<format> newfile; // line number and the string value
// vector<string> outfile;

// Utility function to deal with white spaces
static inline string &trim(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(),
                    not1(ptr_fun<int, int>(isspace)))
                .base(),
            s.end());
    s.erase(s.begin(), find_if(s.begin(), s.end(),
                               not1(ptr_fun<int, int>(isspace))));
    return s;
}

// Utility function to check of a string is a number
bool is_number(const std::string &s)
{

    int i = 0;
    int f = 0;
    if (s[0] == '-' || s[0] == '+')
    {
        i += 1;
        for (int k = i; k < s.length(); k++)
        {
            if (!std::isdigit(s[k]))
            {
                break;
            }
            f += 1;
        }
    }
    else if (s[0] == '0' && s[1] == 'x') // checking for hexadecimal
    {
        i = 2;
        for (int k = i; k < s.length(); k++)
        {
            if (!std::isxdigit(s[k]))
            {
                break;
            }
            f += 1;
        }
    }
    else
    {
        for (int k = i; k < s.length(); k++)
        {
            if (!std::isdigit(s[k]))
            {
                break;
            }
            f += 1;
        }
    }

    // cout << f<<" "<< s.length()-i;
    bool a = f && f == s.length() - i;
    return a;
}

// Utility function for String in any base to decimal
int tonum(string s)
{
    s = trim(s);
    if (s.find("0x") == 0)
    {
        return stoul(s, nullptr, 16);
    }
    else if (s.find("0") == 0)
    {
        return stoul(s, nullptr, 8);
    }
    else if (s.find("-") == 0)
    {
        return -stoul(s.substr(1, s.length()), nullptr, 10);
    }
    else if (s.find("+") == 0)
    {
        return stoul(s.substr(1, s.length()), nullptr, 10);
    }
    else
    {
        return stoul(s, nullptr, 10);
    }
}
// Utility function to check for elements in symbol table
bool contains(string name)
{
    auto iter = find_if(sym_table.begin(), sym_table.end(),
                        [&](const symbol &ts)
                        { return ts.name == name; });
    return iter != sym_table.end();
}

// Utility function to convert int to hex and fill in 0s accordingly to make it length 8
string int_to_hex(int i)
{
    stringstream stream;
    stream << setfill('0') << setw(8)
           << hex << i;
    return stream.str();
}

// Utility function for checking correct label name format
// labels should start with an alphabet and should consist only of alphanumeric characters
int isValidLabel(string label)
{
    if (!((label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z')))
        return false;

    for (int i = 0; i < label.length(); i++)
    {
        if (!(isdigit(label[i]) || (label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z')))
        {
            return false;
        }
    }

    return true;
}

map<string, string> codes;
void codes_init()
{
    codes["ldc"] = "00";
    codes["adc"] = "01";
    codes["ldl"] = "02";
    codes["stl"] = "03";
    codes["ldnl"] = "04";
    codes["stnl"] = "05";
    codes["add"] = "06";
    codes["sub"] = "07";
    codes["shl"] = "08";
    codes["shr"] = "09";
    codes["adj"] = "0a";
    codes["a2sp"] = "0b";
    codes["sp2a"] = "0c";
    codes["call"] = "0d";
    codes["return"] = "0e";
    codes["brz"] = "0f";
    codes["brlz"] = "10";
    codes["br"] = "11";
    codes["HALT"] = "12";
    codes["data"] = "13";
    codes["SET"] = "14";
}
map<string, int> type;
void Instruction_types()
{
    type["data"] = 1;
    type["ldc"] = 1;
    type["adc"] = 1;
    type["ldl"] = 1;
    type["stl"] = 1;
    type["ldnl"] = 1;
    type["stnl"] = 1;
    type["add"] = 3;
    type["sub"] = 3;
    type["shl"] = 3;
    type["shr"] = 3;
    type["adj"] = 1;
    type["a2sp"] = 3;
    type["sp2a"] = 3;
    type["call"] = 2;
    type["return"] = 3;
    type["brz"] = 2;
    type["brlz"] = 2;
    type["br"] = 2;
    type["HALT"] = 3;
    type["SET"] = 1;
}

void first_pass()
{
    int loc = 0;
    int line = 0;
    string instr = "";
    for (int i = 0; i < newfile.size(); i += 1)
    {
        loc = newfile[i].poc;
        line = newfile[i].line;
        instr = newfile[i].instr;
        if (instr.find(':') != string::npos) // if this is a label
        {
            string lab = instr.substr(0, instr.size() - 1);
            lab = trim(lab);
            if (contains(instr))
            {
                // cout << "ERROR- Duplicate Label at line " << line << endl;
                errors.push_back("ERROR- Duplicate Label at line " + to_string(line));
            }
            if (i + 1 < newfile.size())
            {
                string i2 = newfile[i + 1].instr;
                if (i2.substr(0, 3) == "SET")
                {
                    string v = i2.substr(i2.find(' '));
                    v = trim(v);

                    if (is_number(v))
                    {
                        sym_table.push_back({lab, loc, 1, stoi(v), -1});
                    }
                    else
                    {
                        errors.push_back("Invalid usage of SET statement at line: " + to_string(line));
                    }
                    i += 1;
                }
                else
                {
                    sym_table.push_back({lab, loc, 0, -1, -1});
                }
            }
            else
            {
                errors.push_back("ERROR- empty label at line: " + to_string(line));
            }
        }
    }
}

int f = 0;
void second_pass(fstream &listingFile, fstream &objfile)
{
    for (int i = 0; i < newfile.size(); i += 1)
    {
        string machine_code = "";
        string instr = newfile[i].instr;
        int line = newfile[i].line;
        int loc = newfile[i].poc;
        if (instr.find(':') != string::npos) // we have a label
        {
            int colon = instr.find(":", 0);
            if (!isValidLabel(instr.substr(0, colon))) // print an error if invalid label
            {
                errors.push_back("WARNING- Incorrect label format" + instr.substr(0, colon) + " at line " + to_string(line));
            }

            listingFile << int_to_hex(loc) + "          " + instr + '\n';
        }
        else
        {
            if (instr.find(' ') != string::npos)
            {
                string first = instr.substr(0, instr.find(' '));
                first = trim(first);
                if (codes[first] != "") // valid instruction
                {
                    string second = instr.substr(instr.find(' '));
                    int address = -1;
                    second = trim(second);
                    if (type[first] == 2) // branch statement
                    {
                        for (int i = 0; i < sym_table.size(); i++)
                        {
                            if (sym_table[i].name == second)
                            {
                                address = sym_table[i].address;
                                sym_table[i].used = 1; // to give unused label warning
                            }
                        }
                        if (address != -1) // if found in the symbols table
                        {
                            machine_code = int_to_hex(address - (loc + 1)) + codes[first];
                            machine_code = machine_code.substr(2);

                            listingFile << int_to_hex(loc) + ' ' + machine_code + ' ' + instr + '\n';
                        }
                        else
                        {

                            listingFile << int_to_hex(loc) + " Error " + instr + '\n';
                            errors.push_back("ERROR- Undefined label " + second + " at line " + to_string(line));
                        }
                    }
                    else
                    {
                        if (type[first] == 1)
                        {
                            if (is_number(second)) // if it's a number
                            {
                                machine_code = int_to_hex(tonum(second)) + codes[first];
                                machine_code = machine_code.substr(2);
                                listingFile << int_to_hex(loc) + ' ' + machine_code + ' ' + instr + '\n';
                            }
                            else // it can also be a set label
                            {
                                for (int i = 0; i < sym_table.size(); i++) // checking for set label
                                {
                                    if (sym_table[i].name == second)
                                    {
                                        // cout<<sym_table[i].set;
                                        if (sym_table[i].set == 1)
                                        {
                                            address = sym_table[i].set_value;
                                        }
                                        else
                                        {
                                            address = sym_table[i].address;
                                        }
                                        sym_table[i].used = 1; // to give unused label warning
                                    }
                                }
                                cout << second << ' ' << address << ' ' << loc << "Hi buddy" << '\n';
                                if (address != -1)
                                {
                                    machine_code = int_to_hex(address) + codes[first];
                                    machine_code = machine_code.substr(2);
                                    listingFile << int_to_hex(loc) + ' ' + machine_code + ' ' + instr + '\n';
                                }
                                else
                                {
                                    errors.push_back("ERROR- Undefined label " + second + " at line " + to_string(line));
                                }
                            }
                        }
                        else // type[first] =3
                        {
                            errors.push_back("ERROR- Too many arguments for the given instruction at line " + to_string(line));
                        }
                    }
                }
                else
                {
                    errors.push_back("ERROR- invalid instruction at line " + to_string(line));
                }
            }
            else // single instructions
            {
                if (type[instr] == 3) // valid instruction
                {
                    machine_code = "000000" + codes[instr];
                    listingFile << int_to_hex(loc) + ' ' + "000000" + codes[instr] + ' ' + instr + '\n';
                }
                else
                {
                    if (codes[instr] != "")
                    {
                        errors.push_back("ERROR- Missing operand at line " + to_string(line));
                    }
                    else
                    {
                        errors.push_back("ERROR- Invalid instruction at line " + to_string(line));
                    }
                }
            }
        }

        // cout<<machine_code<<'\n';

        if (machine_code != "")
        {

            uint32_t temp = stoull("0x" + trim(machine_code), nullptr, 16);
            objfile.write((char *)&temp, sizeof(temp));
        }
    }
}

int main(int argc, char *argv[])
{
    // Initalize Machine operation table
    codes_init();
    Instruction_types();

    if (argc != 2)
    {
        std::cout << "Invalid Command, Check Format : ./a filename.asm" << '\n';
    }

    // Argument for input file
    string in_file = argv[1];

    // Argument for output file
    string listing_file = in_file.substr(0, in_file.find(".", 0)) + ".L";

    // Argument for log file
    string log_file = in_file.substr(0, in_file.find(".", 0)) + ".log";

    // Argument for object file
    string obj_file = in_file.substr(0, in_file.find(".", 0)) + ".o";

    fstream input_file;
    input_file.open(in_file, ios::in);

    // we are removing the comments and blank lines from the given input file and are pushing these new lines into a data structure
    // this new data structure keeps a track of the line number in the original input file and the Program Counter.
    //  this additional information is helpful while throwing errors and generating the machine code.
    string line;
    int loc = 0;
    int line_count = 1;
    while (getline(input_file, line))
    {
        // Pre-process the line, trim extra spaces
        string instr;
        instr = line.substr(0, line.find(";", 0));
        instr = trim(instr);

        // Skip empty lines
        if (instr == "")
        {
            line_count++;
            continue;
        }
        else
        {
            if (instr.find(':') != string::npos)
            {
                if (instr.find(':') != instr.length() - 1)
                {
                    string first_half = instr.substr(0, instr.find(':') + 1);
                    first_half = trim(first_half);
                    newfile.push_back({loc, line_count, first_half});
                    string second_half = instr.substr(instr.find(':') + 1);
                    second_half = trim(second_half);
                    newfile.push_back({loc, line_count, second_half});
                }
                else
                {
                    newfile.push_back({loc, line_count, instr});
                    loc -= 1;
                }
            }
            else
            {
                newfile.push_back({loc, line_count, instr});
            }
        }
        line_count += 1;
        loc += 1;
    }

    // for (int i=0;i<newfile.size();i+=1)
    // {
    //    cout<<newfile[i].poc<<newfile[i].line<<newfile[i].instr<<'\n';
    // }

    fstream listingFile, logfile, objfile;

    // Open a file to perform a write operation using a file object.
    listingFile.open(listing_file, ios::out);
    logfile.open(log_file, ios::out);
    objfile.open(obj_file, ios::out | ios::binary);

    // cout<<"Hello before first pass"<<'\n';

    // Pass-1 of assembly
    first_pass();
    //  for(int i=0; i < sym_table.size(); i++)
    //  {

    //     cout << sym_table[i].name << ' '<<sym_table[i].address<< ' '<<sym_table[i].set<< ' '<<sym_table[i].set_value<<' '<<sym_table[i].used<<'\n';
    //  }

    // Pass-2 of assembly
    second_pass(listingFile, objfile);

    for (int i = 0; i < sym_table.size(); i++)
    {
        if (sym_table[i].used == -1)
        {
            logfile << "WARNING- unused label: " + sym_table[i].name << '\n';
        }

        cout << sym_table[i].name << ' ' << sym_table[i].address << ' ' << sym_table[i].set << ' ' << sym_table[i].set_value << ' ' << sym_table[i].used << '\n';
    }

    //    cout<<sym_table.size()<<"Symbol  table";

    if (errors.size() == 0)
    {
        logfile << "Program compiled successfully without any errors.";
    }

    for (int i = 0; i < errors.size(); i += 1)
    {
        logfile << errors[i] << '\n';
    }
    // for (int i=0;i< outfile.size(); i+=1)
    // {
    //     listingFile<<outfile[i];
    // }

    // Close files
    listingFile.close();
    logfile.close();
    objfile.close();
}