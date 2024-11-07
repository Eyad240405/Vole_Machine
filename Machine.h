#ifndef UNTITLED_MACHINE_H
#define UNTITLED_MACHINE_H
#include <iostream>
#include <string>
#include <fstream>
#include <bitset>
#include <iomanip>
#include <cmath>
#include <cstdint>

using namespace std;

class Register {
private:
    int reg[16]={0}; // Array to hold 16 register values
public:
    // Methods to get and set register values
    int getcell(int address);
    void setcell(int address, int val);
    void display();
};

class ALU {
public:
    string hexToDecimal(const std::string& hexStr);
    string decimalToHex(int decimalValue);
    bool isValidHex(const string& hexStr);
    void addTwoComplement(int index1, int index2, int index3, Register &reg);
    uint32_t floatAdd(uint32_t a, uint32_t b);
    int binaryToDecimal(const string &binaryStr);
    string decimalToBinary(int decimalValue);
};

class Memory {
private:
    string memory[256]; // Memory array of 256 cells
public:
    // Constructor to initialize memory with "00"
    Memory();
    // Methods to write, get a cell value, set a cell value, and display memory
    void write();
    string getCell(int address) const;
    void setCell(int address, const string& val);
    void display() const;
    void Screen();
};

class CU {
public:
    void load(int idxReg, int intMem, Register &reg, Memory &memory);
    void load(int idxReg, int val, Register &reg);
    void store(int idxReg, int idxMem, Register &reg, Memory &memory);
    void move(int idxReg1, int idxReg2, Register &reg);
    void jump(int idxReg, int idxMem, Register &reg, int &PC);
    void halt(Memory& memory, Register& reg);
};

class CPU {
private:
    int programCounter;
    string instruction;
    Memory memory;
    Register reg;
    ALU alu;
    CU cu;
public:
    CPU();
    void runNextStep(Memory& memory);
    void fetch(Memory& memory);
    void execute(Register& reg, Memory& memory, string& instruction);
    void loadProgramFromFile(const string &filename);
};
#endif
