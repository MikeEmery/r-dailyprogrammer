//
//  main.cpp
//  led-sim
//
//  Created by  Mike Emery on 11/4/16.
//  Copyright © 2016 Mike Emery. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

void printLeds(const uint8_t reg) {
    for(int i = 0; i < 8; ++i) {
        if (0x80 & reg << i) {
            cout << '*';
        }
        else {
            cout << '.';
        }
    }
    cout << endl;
}

class Op {
public:
    Op() {}
    virtual void apply(uint32_t& instruction) {
        ++instruction;
    }
};

shared_ptr<Op> NOOP = make_shared<Op>();

class Load : public Op {
    shared_ptr<uint8_t> reg;
    uint8_t value;
public:
    Load(shared_ptr<uint8_t> reg, uint8_t value): reg(reg), value(value) {}
    void apply(uint32_t& instruction) {
        Op::apply(instruction);
        *reg = value;
    }
};

class Out : public Op {
    shared_ptr<uint8_t> reg;
public:
    Out(shared_ptr<uint8_t> reg): reg(reg) {}
    void apply(uint32_t& instruction) {
        Op::apply(instruction);
        printLeds(*reg);
    }
};

class Rlca : public Op {
    shared_ptr<uint8_t> reg;
public:
    Rlca(shared_ptr<uint8_t> reg): reg(reg) {}
    void apply(uint32_t& instruction) {
        Op::apply(instruction);
        uint8_t msb = 0x80 & *reg;
        *reg = (*reg << 1) & msb;
    }
};

class Rrca : public Op {
    shared_ptr<uint8_t> reg;
public:
    Rrca(shared_ptr<uint8_t> reg): reg(reg) {}
    void apply(uint32_t& instruction) {
        Op::apply(instruction);
        uint8_t lsb = (0x01 & *reg) << 8;
        *reg = (*reg >> 1) & lsb;
    }
};

class Djnz : public Op {
    shared_ptr<uint8_t> reg;
    uint32_t jmpTo;
public:
    Djnz(shared_ptr<uint8_t> reg, const uint32_t jmpTo): reg(reg), jmpTo(jmpTo) {}
    void apply(uint32_t& instruction) {
        Op::apply(instruction);
        *reg -= 1;
        
        if (0 != *reg) {
            instruction = jmpTo;
        }
    }
};

class Label : public Op {
    uint32_t lineNum;
public:
    Label(uint32_t lineNum): lineNum(lineNum) {}
    uint32_t getLineNum() { return lineNum; }
};

shared_ptr<Op> parseLine(const string& line, const uint32_t lineNum, map<string, shared_ptr<uint8_t>> registers) {
    cout << line << endl;
    const char* ccstr = line.c_str();
    char cstr[line.length()];
    
    strcpy(cstr, ccstr);
    
    char* pch = strtok(cstr, " ");
    shared_ptr<Op> op;
    
    if (!pch) {
        op = NOOP;
    }
    else if (0 == strncmp(pch, "ld", 2)) {
        string regKey = strtok(NULL, ",");
        char* rawValue = strtok(NULL, "");
        
        op = make_shared<Load>(registers[regKey], atoi(rawValue));
    }
    else if (0 == strncmp(pch, "out", 3)) {
        strtok(NULL, ",");
        string regKey = strtok(NULL, "");
        
        op = make_shared<Out>(registers[regKey]);
    }
    else if (0 == strncmp(pch, "rlca", 4)) {
        op = make_shared<Rlca>(registers["a"]);
    }
    else if (0 == strncmp(pch, "rrca", 4)) {
        op = make_shared<Rrca>(registers["a"]);
    }
    else if (0 == strncmp(pch, "\n", 1)) {
        op = NOOP;
    }
    else {
        
    }
    
    return op;
}

int main(int argc, const char * argv[]) {
    map<string, shared_ptr<uint8_t>> registers;
    registers["a"] = make_shared<uint8_t>(0);
    registers["b"] = make_shared<uint8_t>(0);
    
    vector<shared_ptr<Op>> instructions;
    
    ifstream inputFile("input.txt");
    string line;
    uint32_t lineNum = 0;
    
    while(getline(inputFile, line)) {
        shared_ptr<Op> inst = parseLine(line, lineNum, registers);
        instructions.push_back(inst);
        lineNum++;
    }
    
    uint32_t i = 0;
    while(i < instructions.size()) {
        shared_ptr<Op> current = instructions[i];
        current->apply(i);
    }
    
    return 0;
}
