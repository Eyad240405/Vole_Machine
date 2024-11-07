#include "Machine.h"
using namespace std;

int Type = 1; // To know the type of run (whole or step by step)
int counter = 1; // Count the steps of the program
//===========ALU==========
// Method to convert from hexadecimal to decimal
string ALU::hexToDecimal(const string& hexStr) {
    int base = 1; // 16^0
    int decimal = 0;
    for (int i = hexStr.length() - 1; i >= 0; --i) {
        if (hexStr[i] >= '0' && hexStr[i] <= '9') {
            decimal += (hexStr[i] - '0') * base;
        } else if (hexStr[i] >= 'A' && hexStr[i] <= 'F') {
            decimal += (hexStr[i] - 'A' + 10) * base;
        }
        base *= 16;
    }
    return to_string(decimal);
}
// Method to check if the string is hexadecimal
bool ALU::isValidHex(const string& hexStr) {
    for (char c : hexStr) {
        if (!((c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'F'))) {
            return false;
        }
    }
    return true;
}
// Method to convert from decimal to hexadecimal
string ALU::decimalToHex(int decimalValue) {
    if (decimalValue == 0) return "00";
    string hexStr = "";
    while (decimalValue > 0) {
        int remainder = decimalValue % 16;
        if (remainder < 10) {
            hexStr += static_cast<char>(remainder + '0');
        } else {
            hexStr += static_cast<char>(remainder - 10 + 'A');
        }
        decimalValue /= 16;
    }
    if(isValidHex(hexStr))
        return hexStr;
}
// Method to add
void ALU::addTwoComplement(int index1, int index2, int index3, Register& reg) {
    int value1 = reg.getcell(index1);
    int value2 = reg.getcell(index2);
    // Convert to two's complement if necessary
    if (value1 < 0) value1 = (1 << 8) + value1; // Assuming 8-bit representation
    if (value2 < 0) value2 = (1 << 8) + value2; // Assuming 8-bit representation
    int result = value1 + value2;
    result &= 0xFF; // Handle overflow by masking to 8 bits
    if (result & (1 << 7)) result -= (1 << 8); // Convert back to signed integer if necessary
    reg.setcell(index3, result);
}
// Method to add using the floating point
uint32_t ALU::floatAdd(uint32_t a, uint32_t b) {
    // Extract the sign, exponent, and mantissa of both numbers
    int signA = (a >> 31) & 1;
    int exponentA = (a >> 23) & 0xFF;
    int mantissaA = a & 0x7FFFFF;
    int signB = (b >> 31) & 1;
    int exponentB = (b >> 23) & 0xFF;
    int mantissaB = b & 0x7FFFFF;
    // Add the hidden 1 in the mantissa for normalized values
    mantissaA |= 0x800000;
    mantissaB |= 0x800000;
    // Align exponents by shifting the mantissa of the smaller exponent
    if (exponentA > exponentB) {
        mantissaB >>= (exponentA - exponentB);
        exponentB = exponentA;
    } else {
        mantissaA >>= (exponentB - exponentA);
        exponentA = exponentB;
    } // Add/Subtract mantissas
     int mantissaResult;
    if (signA == signB) {
        mantissaResult = mantissaA + mantissaB;
    } else {
        mantissaResult = mantissaA - mantissaB;
    }
    // Normalize the result
    if (mantissaResult & 0x80000000) { // If there's a carry out
        mantissaResult >>= 1;
        exponentA++;
    } // Clear the hidden bit
    mantissaResult &= 0x7FFFFF;
    // Construct the result
    uint32_t result = (signA << 31) | (exponentA << 23) | mantissaResult;
    return result;
}
string ALU::decimalToBinary(int decimalValue){
    string binaryStr;
    while (decimalValue > 0) {
        binaryStr = (decimalValue % 2 == 0 ? "0" : "1") + binaryStr;
        decimalValue /= 2;
    }
    // Ensure binaryStr has at least one character
    return binaryStr.empty() ? "0" : binaryStr;
}
int ALU::binaryToDecimal(const string& binaryStr) {
    int decimalValue = 0;
    int base = 1;
    for (int i = binaryStr.length() - 1; i >= 0; --i) {
        if (binaryStr[i] == '1') {
            decimalValue += base;
        }
        base *= 2;
    }
    return decimalValue;
}

//===========Memory==========
// Constructor to initialize memory with "00"
Memory::Memory() {
    write();
}
// Method to initialize all memory cells to "00"
void Memory::write() {
    for (int r = 0; r < 256; r++) {
        memory[r] = "00";
    }
}
// Method to get the value of a specific memory cell
string Memory::getCell(int address) const {
    return memory[address];
}
// Method to set the value of a specific memory cell
void Memory::setCell(int address, const string& val) {
    memory[address] = val;
}
// Method to display all memory cells
void Memory::display() const {
    int i = 0;
    cout << "Memory content:" << endl;
//  To display the memory as a matrix
    for (int r = 0; r < 16; r++) {
        for(int c = 0; c < 16; c++){
            cout << setw(2) << setfill('0') << memory[i] << " ";
            i++;
        }
        cout << endl;
    }
    cout << endl << "==================================" << endl;
}
void Memory::Screen(){
    cout << "Screen as hexadecimal: " << setw(2) << setfill('0') << getCell(0) << endl;
//    cout << "Screen as Ascii: " << alu.decimalToHex(stoi(getCell(0))) << endl;
    cout << "==================================" << endl << endl;
}

//===========Register==========
// Method to get the value of a specific memory cell
int Register::getcell(int address){
    return reg[address];
}
// Method to set the value of a specific register cell
void Register::setcell(int address,int val){
    reg[address] = val;
}
// Method to display all register cells
void Register::display() {
    ALU alu;
    cout << "Register content:" << endl;
    for(int i = 0 ; i < 16 ; i++){
        if(i <= 9)
            cout << i << ")  " << setw(2) << setfill('0') << alu.decimalToHex(reg[i]) << endl;
        else
            cout << i << ") " << setw(2) << setfill('0') << alu.decimalToHex(reg[i]) << endl;
    }
    cout << endl;
}

//===========CU==========
// Loading a variable to the register
void CU::load(int idxReg, int intMem, Register &reg, Memory &memory) {
    reg.setcell(idxReg,stoi(memory.getCell(intMem)));
}
void CU::load(int idxReg, int val, Register &reg) {
    reg.setcell(idxReg,val);
}
// Storing a variable to the memory
void CU::store(int idxReg, int idxMem, Register &reg, Memory &memory) {
    memory.setCell(idxMem, to_string(reg.getcell(idxReg)));
}
// Moving a variable from a cell to another cell in the register
void CU::move(int idxReg1, int idxReg2, Register &reg) {
    reg.setcell(idxReg2,reg.getcell(idxReg1));
}
// Jumping to another cell
void CU::jump(int idxReg, int idxMem, Register &reg, int &PC) {
    if (reg.getcell(idxReg) == reg.getcell(0)) {
        PC = idxMem;
    }
}
// Halting method
void CU::halt(Memory& memory, Register& reg) {
    if(Type == 1){
        memory.display();
        reg.display();
        memory.Screen();
    }
    cout << "Halting program!" << endl;
    exit(0);
}

//===========CPU==========
CPU::CPU() : programCounter(10) {} // Make the program counter start from 10
// Fetching the instructions from the memory
void CPU::fetch(Memory& memory) {
    instruction = memory.getCell(programCounter) + memory.getCell(programCounter + 1);
    programCounter+=2;
}
// Executing the instructions
void CPU::execute(Register& reg2 , Memory& mem, string& instruction) {
    while(true) {
        char opcode = instruction[0];
        switch (opcode) {
            case '1': { // Loading to register
                int memoryaddress = stoi(alu.hexToDecimal(instruction.substr(2, 2)));
                int registeraddress = stoi(instruction.substr(1, 1));
                cu.load(registeraddress, memoryaddress, reg2, mem);
                break;
            }
            case '2': {
                int memoryaddress = stoi(alu.hexToDecimal(instruction.substr(2, 2)));
                int registeraddress = stoi(instruction.substr(1, 1));
                cu.load(registeraddress, memoryaddress, reg);
                break;
            }
            case '3': { // Storing to the memory
                int memoryaddress = stoi(alu.hexToDecimal(instruction.substr(2, 2)));
                int registeraddress = stoi(instruction.substr(1, 1));
                cu.store(registeraddress, memoryaddress, reg, mem);
                break;
            }
            case '4': { // Moving value from register cell to another
                int registeraddress1 = stoi(instruction.substr(2, 1));
                int registeraddress2 = stoi(instruction.substr(3, 1));
                cu.move(registeraddress1, registeraddress2, reg);
                break;
            }
            case '5': {  // Add Operation
                int registerS = stoi(instruction.substr(2, 1));  // Source register S
                int registerT = stoi(instruction.substr(3, 1));  // Source register T
                int registerR = stoi(instruction.substr(1, 1));  // Destination register R
                alu.addTwoComplement(registerS, registerT, registerR, reg);
                break;
            }
            case '6': { // Floating point
                int registerS = stoi(instruction.substr(2, 1));  // Source register S
                int registerT = stoi(instruction.substr(3, 1));  // Source register T
                int registerR = stoi(instruction.substr(1, 1));  // Destination register R
                // Get the bit patterns from registers S and T
                uint32_t bitPatternS = reg.getcell(registerS);
                uint32_t bitPatternT = reg.getcell(registerT);
                uint32_t resultBitPattern = alu.floatAdd(bitPatternS, bitPatternT);
                // Store the result bit pattern in register R
                reg.setcell(registerR, resultBitPattern);
            }
            case '7': {  // OR Operation
                int registerS = stoi(instruction.substr(2, 1));  // Source register S
                int registerT = stoi(instruction.substr(3, 1));  // Source register T
                int registerR = stoi(instruction.substr(1, 1));  // Destination register R
                // Get the bit patterns from registers S and T
                uint32_t bitPatternS = reg.getcell(registerS);
                uint32_t bitPatternT = reg.getcell(registerT);

                uint32_t resultBitPattern = bitPatternS | bitPatternT;
                // Store the result bit pattern in register R
                reg.setcell(registerR, resultBitPattern);
            }
            case '8':{  // AND Operation
                int registerS = stoi(instruction.substr(2, 1));  // Source register S
                int registerT = stoi(instruction.substr(3, 1));  // Source register T
                int registerR = stoi(instruction.substr(1, 1));  // Destination register R
                // Get the bit patterns from registers S and T
                uint32_t bitPatternS = reg.getcell(registerS);
                uint32_t bitPatternT = reg.getcell(registerT);

                uint32_t resultBitPattern = bitPatternS & bitPatternT;
                // Store the result bit pattern in register R
                reg.setcell(registerR, resultBitPattern);
            }
            case '9': { // XOR operation
                int registerS = stoi(instruction.substr(2, 1));  // Source register S
                int registerT = stoi(instruction.substr(3, 1));  // Source register T
                int registerR = stoi(instruction.substr(1, 1));  // Destination register R
                // Get the bit patterns from registers S and T
                uint32_t bitPatternS = reg.getcell(registerS);
                uint32_t bitPatternT = reg.getcell(registerT);

                uint32_t resultBitPattern = bitPatternS ^ bitPatternT;
                // Store the result bit pattern in register R
                reg.setcell(registerR, resultBitPattern);
            }
            case 'A': { // Rotate
                int cycle = stoi(instruction.substr(3,1));
                string register_as_binary = alu.decimalToBinary(reg.getcell(stoi(instruction.substr(1,1))));
                for(int i = 1 ; i <= cycle ; i++){
                    char right = register_as_binary[register_as_binary.length()-1];
                    for(int k = register_as_binary.length()-1 ; k > 0 ; k--)
                        register_as_binary[k] = register_as_binary[k-1];
                    register_as_binary[0] = right;
                }
                int rotated_register = alu.binaryToDecimal(register_as_binary);
                reg.setcell(stoi(instruction.substr(1,1)),rotated_register);
            }
            case 'B': {  // New Opcode for Conditional Jump
                int registerR = stoi(instruction.substr(1, 1));  // The register to compare
                int memoryAddressXY = stoi(alu.hexToDecimal(instruction.substr(2, 2)));  // Address to jump to if equal
                cu.jump(registerR, memoryAddressXY, reg, programCounter);
                break;
            }
            case 'C': { // Halting
                cu.halt(memory,reg);
                break;
            }
            case 'D': {  // New Opcode for Jump If Greater Than
                int registerR = stoi(instruction.substr(1, 1));          // Register to compare
                int memoryAddressXY = stoi(alu.hexToDecimal(instruction.substr(2, 2)));  // Target address
                // Retrieve values from register R and register 0
                int32_t valueR = static_cast<int32_t>(reg.getcell(registerR));  // Two's complement interpretation
                int32_t value0 = static_cast<int32_t>(reg.getcell(0));          // Two's complement interpretation
                // Check if the value in register R is greater than the value in register 0
                if (valueR > value0) {
                    programCounter = memoryAddressXY;  // Jump to the specified address
                }
            }
        }
        if(Type == 2) {
            memory.display();
            reg.display();
            memory.Screen();
        }
        runNextStep(memory);
    }
}
// Run the program
void CPU::runNextStep(Memory& memory) {
    if(Type == 2)
        cout << "Step #" << counter++ << endl;
    fetch(memory);
    execute(reg, memory, instruction);
}
// loading the input instructions
void CPU::loadProgramFromFile(const string& filename){
    ifstream file(filename);
    if (!file) {
        cout << "Failed to open file: " << filename << endl;
        return;
    }
    string line;
    int address = 9;
    while (getline(file, line)){
        memory.setCell(++address,line.substr(0,2));
        memory.setCell(++address,line.substr(2,2));
    }
//    runNextStep(memory);
    file.close();
}
int main(){
    CPU cpu;
    Memory memory;
    cout << "Ahln ya user ya habibi ^_^" << endl;
    cout << "Choose from this:\n" << "1) Load (input_test.txt) file\n" << "2) Load another file\n" << "3) Exit\n";
    int choice;
    cin >> choice;
    if (choice == 1) {
        cpu.loadProgramFromFile("input_test.txt");
    } else if (choice == 2) {
        cout << "Enter your file name:";
        string filename;
        cin >> filename;
        cpu.loadProgramFromFile(filename);
    } else if (choice == 3) {
        cout << "Thanks for using my program!\n";
        return 0;
    }
    cout << "Choose from this:\n" << "1) Run as a whole\n" << "2) Run step by step\n";
    int choice2;
    cin >> choice2;
    Type = choice2;
    cpu.runNextStep(memory);
}