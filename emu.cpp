/*****************************************************************************
TITLE: EMULATOR
AUTHOR:   M Shanmukha Priya (2101CS40)
Declaration of Authorship
This file, emu.cpp, is part of the miniproject of CS209/CS210 at the
department of Computer Science and Engg, IIT Patna .
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std;
vector<int> objectFile;
int mainMemory[1 << 20];
int total = 0;
int stackLimit = 1 << 19;
int PC = 0, SP = 0, regA = 0, regB = 0;
map<string, string> codes;

string int_to_hex(int i)
{
    stringstream stream;
    stream << setfill('0') << setw(8)
           << hex << i;
    return stream.str();
}

void executeOpcode(int opcode, int operand)
{
    switch (opcode)
    {
    case 0:
        regB = regA;
        regA = operand;
        break;
    case 1:
        regA += operand;
        break;
    case 2:
        regB = regA;
        regA = mainMemory[SP + operand];
        break;
    case 3:
        mainMemory[SP + operand] = regA;
        regA = regB;
        break;
    case 4:
        regA = mainMemory[regA + operand];
        break;
    case 5:
        mainMemory[regA + operand] = regB;
        break;
    case 6:
        regA += regB;
        break;
    case 7:
        regA = regB - regA;
        break;
    case 8:
        regA = regB << regA;
        break;
    case 9:
        regA = regB >> regA;
        break;
    case 10:
        SP += operand;
        break;
    case 11:
        SP = regA;
        regA = regB;
        break;
    case 12:
        regB = regA;
        regA = SP;
        break;
    case 13:
        regB = regA;
        regA = PC;
        PC += operand;
        break;
    case 14:
        PC = regA;
        regA = regB;
        break;
    case 15:
        if (regA == 0)
            PC += operand;
        break;
    case 16:
        if (regA < 0)
            PC += operand;
        break;
    case 17:
        PC += operand;
        break;
    case 18:
    {
        cout << "Program finished successfully" << '\n';
        break;
    }

    default:
        cout << "Invalid opcode. Incorrect machine code. Aborting";
        exit(0);
    }
}
void memorydump(ofstream &trcfile, int lt)
{
    for (int i = 0; i < lt; i++)
    {
        trcfile << int_to_hex(i) << ' ' << int_to_hex(mainMemory[i]) << '\n';
        // prints the line number and the instruction in hex
    }
}

int argumentrun()
{ // loop to execute until halt command is encountered

    if (PC >= (int)objectFile.size())
    {
        cout << "Segmentation Fault " + '\n';
        return -1;
    }

    int32_t operand, opcode;
    int32_t instr = objectFile[PC];
    // masking first 6 bits
    opcode = instr & 0xff;
    // masking last 2 bits
    operand = instr & 0xffffff00;
    // Arithematic shift by 2
    operand >>= 8;
    // cout<<operand<<' '<<opcode<<'\n';
    if (opcode == 18)
    {
        PC += 1;
        total++;
        return 0; // back to main function
    }
    int old_PC = PC;
    if (opcode == 19 || opcode == 20) // case 19 and 20 aren't sent to the execute opcode function
    {
        PC += 1;
        return 1;
    }
    executeOpcode(opcode, operand);
    PC += 1;
    if (old_PC == PC)
    {
        cout << "Infinite loop detected" << endl;
        return -1;
    }

    total++;

    if (SP > stackLimit)
    {
        cout << "Stack overflow. Aborting.\n";
        return -1;
    }
    return 1;
}

bool advance(ofstream &trcfile, int f)
{
    if (argumentrun() > 0)
    {
        if (f == 0)
        {
            trcfile << "A = " << int_to_hex(regA) << ", B = " << int_to_hex(regB) << ", PC = " << int_to_hex(PC) << ", SP = " << int_to_hex(SP) << "\n";
        }
        return true;
    }
    else
    {
        return false;
    }
}
int main(int argc, char *argv[])
{
    int _temp;
    int i = 0;
    string in_file = argv[2];
    string trace_file = in_file.substr(0, in_file.find(".", 0)) + ".trace";
    fstream currfile(argv[2], ios::in | ios::binary);
    ofstream trcfile;
    string ch = argv[1];
    cout<<ch<<argv[2]<<" ";
    if (ch != "-trace" && ch != "-before" && ch != "-after" && ch != "-all")
    {
        cout << "Invalid Instruction. Instruction should be of format -trace, -after, -before, -all. \n";
        return 0;
    }
    trcfile.open(trace_file);
    while (currfile.read((char *)&_temp, sizeof(int)))
    {
        objectFile.push_back(_temp);
        int32_t operand, operation;
        int32_t instr = _temp;

        operation = instr & 0xff;

        operand = instr & 0xffffff00;

        operand >>= 8;
        if (operation == 19 || operation == 20)
        {
            mainMemory[i] = operand;
        }

        // Other instructions stored in memory
        else if (operation >= 0 && operation < 20)
        {
            mainMemory[i] = _temp;
        }

        // Identify invalid instruction
        else
        {
            cout << "Invalid instruction\n"
                 << operation << ' ' << operand;
            return (0);
        }
        i++;
    }

    for (int it = 0; it < i; it++)
    {
        cout << mainMemory[it] << '\n';
    }
    cout << i << '\n';

    int xx = objectFile.size();
    int f = 0; // a flag variable to either print or not to print the trace.

    if (ch == "-before")
    {
        trcfile << "Memory Dump before execution: " << '\n';
        memorydump(trcfile, xx);
    }
    else if (ch == "-trace")
    {
        while (true)
        {
            bool a = advance(trcfile, f);
            if (a == false)
            {
                cout << a;
                break;
            }
        }
        trcfile << "Total instructions executed: " << total << endl;
    }
    else if (ch == "-after")
    {
        f = -1;
        while (true)
        {
            bool a = advance(trcfile, f);
            if (a == false)
            {
                cout << a;
                break;
            }
        }
        trcfile << "Total instructions executed: " << total << endl;

        trcfile << "Memory Dump after execution:  " << '\n';
        memorydump(trcfile, xx);
    }
    else if (ch == "-all")
    {
        trcfile << "Memory Dump before execution: " << '\n';
        memorydump(trcfile, xx);
        while (true)
        {
            bool a = advance(trcfile, f);
            if (a == false)
            {
                cout << a;
                break;
            }
        }
        trcfile << "Total instructions executed: " << total << endl;
        trcfile << "Memory Dump after execution:  " << '\n';
        memorydump(trcfile, xx);
    }
    else
    {
        cout << "Invalid Instruction. Instruction should be of format -trace, -after, -before, -all. \n";
    }

    // cout << xx;

    currfile.close();
    trcfile.close();
    return 0;
}