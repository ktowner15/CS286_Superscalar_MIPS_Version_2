#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <bitset>
#include <fstream>
#include <string>
#include <list>
using namespace std;



void printBuffers();
string printCache(cacheSet cache[], int numberOfSets);
string determineRegister(bitset<5> regAddress);
int determineRegisterInt(bitset<5> regAddress);
int convertImmToInt(string imm);

struct cacheSet{        //Cache will be an array of these structs
        bitset<1> LRU = {0};
        bitset<1> valid = {0};
        bitset<1> dirty = {0};
        bitset<32> tag = {00000000000000000000000000000000};                 //FIXME: how many bits long should the tag be?  If it's an int, should it be 32, or am I missing something?
        bitset<32> block[1] = {00000000000000000000000000000000};

        
};

struct bitGroup_32{
        bitset<32> commandLine;
        bitset<1> validBit;
        bitset<5> opcodeBits;
        bitset<5> rsBits;
        bitset<5> rtBits;
        bitset<5> rdBits;
        bitset<5> shamtBits;
        bitset<6> functBits;
        bitset<16> immediateBits;
        bitset<26> addressBits;

        

};

const int MAX_INSTRUCTION_COUNT = 500;        //Maximum number of storable instructions per program
int beginningAddress = 96;
int PC = beginningAddress;
int readCounter = 0;

int instructionAddresses[MAX_INSTRUCTION_COUNT];        /* This tracks the address (starting at the PC and incrementing by 4 for each successive instruction) line-by=line,
                                                         * and should serve as a record of each instruction's address (will be used during jump instructions)*/

bitset<32> command;
bitset<1> validflag;
bitset<5> opcode;
bitset<5> rs;
bitset<5> rt;
bitset<5> rd;
bitset<5> shamt;
bitset<6> funct;
bitset<16> immediate;
bitset<26> address;

bitset<32> commands[MAX_INSTRUCTION_COUNT];
bitset<1> validflags[MAX_INSTRUCTION_COUNT];
bitset<5> opcodes[MAX_INSTRUCTION_COUNT];
bitset<5> rss[MAX_INSTRUCTION_COUNT];
bitset<5> rts[MAX_INSTRUCTION_COUNT];
bitset<5> rds[MAX_INSTRUCTION_COUNT];
bitset<5> shamts[MAX_INSTRUCTION_COUNT];
bitset<6> functs[MAX_INSTRUCTION_COUNT];
bitset<16> immediates[MAX_INSTRUCTION_COUNT];
bitset<26> addresses[MAX_INSTRUCTION_COUNT];
string instructions[MAX_INSTRUCTION_COUNT];

int dataValues[MAX_INSTRUCTION_COUNT];
int dataStartingAddress = 0;
int numberOfDataEntries = 0;

cacheSet cache[8];
bitset<32> preIssueBuffer[4];
bitset<32> preALUQueue[2];
bitset<32> postALUQueue[1];
list<bitset<32>> preMEMQueue[2];
list<bitset<32>> postMEMQueue[1];


int main(int argc, char* argv[])
{       
        /*BEGINNING OF DISASSEMBLER*/
        string filenameOut;
        string filenameSim;

        stringstream ss1;
        stringstream ss2;

        ss1 << argv[4] << "_dis.txt";     //Sets disassembly output filename
        filenameOut = ss1.str();
        

        ss2 << argv[4] << "_pipeline.txt";    //Sets simulation output filename
        filenameSim = ss2.str();
        





        ofstream ofs;
        ofs.open(filenameOut);
        int breakflag = 0;

        
        if(!ofs.is_open()){
                cerr << "Failed to open output file, exiting & returning 1..." << endl;
                exit(1);
        }
        
        char buffer[4];
        int i;
        char * iPtr;
        iPtr = (char*)(void*) &i;

        

        int FD = open(argv[2], O_RDONLY);       //argv[2] is the filename
        int programCounter = 96;
        int programCounterTracker = 96;
        int dataValueIndex = 0;

        int amt = 4;
        while( amt != 0 )
        {       //Reads each binary line as a command, then stores that command in subsequent arrays, in order
                amt = read(FD, buffer, 4);      //DEBUG: This line caused amt to decrement to 3, then decrement to 0.  Unknown why.
                if( amt == 4)                           //This just reassigns all bits to correct location 
                                                        //(since MIPS is little-endian and x86 is big-endian)
                {
                        iPtr[0] = buffer[3];
                        iPtr[1] = buffer[2];
                        iPtr[2] = buffer[1];
                        iPtr[3] = buffer[0];
                        // cout << endl << "i = " <<hex<< i << endl;
                        // cout << "op code: " << dec << (((unsigned int)i)>>26) << endl;
                        // cout << "rs bits: " << ((((unsigned int)i)<<6)>>27) << endl;

                        command = i;
                        validflag = (i>>31);
                        opcode = (i>>26);
                        rs = (i>>21);
                        rt = (i>>16);
                        rd = (i>>11);
                        shamt = (i>>6);
                        //what differentiates the funct, immediate, and address
                        //      is the bitset length that they're declared as 
                        //      (see lines 9-18)
                        funct = (i>>0);
                        immediate = (i>>0);
                        address = (i>>0);

                        
                        
                        /* This is test code that prints out:
                         * - The full 32bit value,
                         * - The first bit of the opcode (indicates
                         *      instruction validity)
                         * - The full opcode
                         * - The full rs bitset
                         * - The full rt bitset
                         * - The full rd bitset
                         * - The full shamt bitset
                         * - The full funct bitset
                         * - The full immediate bitset
                         * - The full address bitset */
                        /*cout << "Results for line" << readCounter << ": " << endl 
                             << "---------------------------------" << endl
                             << "command: " << command << endl
                             << "validflag: " << validflag << endl
                             << "opcode: " << opcode << endl
                             << "rs: " << rs << endl
                             << "rt: " << rt << endl
                             << "rd: " << rd << endl
                             << "shamt: " << shamt << endl
                             << "funct: " << funct << endl
                             << "immediate: " << immediate << endl
                             << "address: " << address << endl
                             << "---------------------------------" << endl << endl;
                        */

                        commands[readCounter] = command;
                        validflags[readCounter] = validflag;
                        opcodes[readCounter] = opcode;
                        rss[readCounter] = rs;
                        rts[readCounter] = rt;
                        rds[readCounter] = rd;
                        shamts[readCounter] = shamt;
                        functs[readCounter] = funct;
                        immediates[readCounter] = immediate;
                        addresses[readCounter] = address;
                        instructionAddresses[readCounter] = programCounterTracker;
                        
                        
                        

                        if(breakflag == 0){

                                ofs << setw(2) << left << validflags[readCounter]
                                    << setw(6) << left << opcodes[readCounter]
                                    << setw(6) << left << rss[readCounter]
                                    << setw(6) << left << rts[readCounter]
                                    << setw(6) << left << rds[readCounter]
                                    << setw(6) << left << shamts[readCounter]
                                    << setw(8) << left << functs[readCounter]
                                    << setw(4) << left << instructionAddresses[readCounter];

                                if (validflags[readCounter].to_ulong() == 0){//else if invalid instruction, then:
                                        ofs << "Invalid Instruction" << endl;
                                }
                                if(validflags[readCounter].to_ulong() == 1){    //If valid instruction, then translate.
                                        int numInstructions = 0;
                                        /*Add more if needed*/

                                        switch (opcodes[readCounter].to_ulong()) {      /*Prints out operation, determined by opcode & funct*/
                                                case 0://R-TYPE
                                                        //Now we must decide between all R instr.s using funct
                                                        switch(functs[readCounter].to_ulong()){         //All of the R-instr.s are complete
                                                                case 0://SLL or NOP (checked)
                                                                        
                                                                        //if (the whole instruction is 0s): it's a NOP (checked)
                                                                        if( (rss[readCounter] == 0)&&(rts[readCounter] == 0)&&(rds[readCounter] == 0)&&(shamts[readCounter] == 0) ){   
                                                                                ofs << setw(6) <<"NOP ";
                                                                                //No operation
                                                                        }
                                                                        //else: it's an SLL(checked + tested)
                                                                        else{   
                                                                                ofs << setw(6) << "SLL ";
                                                                                //Format: rd, rt, sa
                                                                                ofs << determineRegister(rds[readCounter]) << ", "      /*Do not use PCcounter, since it starts at 96*/
                                                                                    << determineRegister(rts[readCounter]) << ", "
                                                                                    << "#" << shamts[readCounter].to_ulong();

                                                                        }
                                                                break;
                                                                case 2://SRL (checked + tested)
                                                                        ofs << setw(6) << "SRL ";
                                                                        //Format: rd, rt, sa
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]) << ", "
                                                                            << "#" << shamts[readCounter].to_ulong();

                                                                break;
                                                                case 8://JR (checked + tested)
                                                                        ofs << setw(6) << "JR ";
                                                                        //Format: rs
                                                                        ofs << determineRegister(rss[readCounter]);
                                                                break;
                                                                case 10://MOVZ (checked + tested)
                                                                        ofs << setw(6) << "MOVZ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                                break;
                                                                case 13://BREAK (checked + tested)
                                                                        ofs << setw(6) << "BREAK ";
                                                                        numInstructions = 
                                                                        breakflag = readCounter;                    //Trips breakflag, signals that it's time to start reading raw data as ints.
                                                                        dataStartingAddress = instructionAddresses[readCounter] + 4;    //Stores address where data memory starts
                                                                break;
                                                                case 32://ADD (checked + tested)
                                                                        ofs << setw(6) << "ADD ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                                break;
                                                                case 34://SUB (checked + tested)
                                                                        ofs << setw(6) << "SUB ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                                break;
                                                                case 36://AND (not present)
                                                                        ofs << setw(6) << "AND ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                                break;
                                                                case 37://OR (not present)
                                                                        ofs << setw(6) << "OR ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                                break;

                                                                /*End of R-switches determining operation
                                                                  Now it's time to see what the rs, rt, and rd bits are.
                                                                  */

                                                        }
                                                break;
                                                case 1://BLTZ (checked + tested) 
                                                        ofs << setw(6) << "BLTZ ";
                                                        //Format: rs, offset
                                                        ofs << determineRegister(rss[readCounter]) << ", "
                                                            << "#" << immediates[readCounter].to_ulong() * 4;   // immediate is multiplied by 4 to give correct offset amount
                                                break;
                                                case 2://J (checked + tested)
                                                        ofs << setw(6) << "J ";
                                                        //Format: address
                                                        ofs << "#"
                                                            << addresses[readCounter].to_ulong() * 4;
                                                break;
                                                case 3://LW (checked + tested)
                                                        ofs << setw(6) << "LW ";
                                                        //Format: rt, offset(base)
                                                        ofs << determineRegister(rts[readCounter]) << ", "
                                                            << immediates[readCounter].to_ulong()
                                                            << "(" << determineRegister(rss[readCounter]) << ")";
                                                break;
                                                case 4://BEQ (not present)
                                                        ofs << setw(6) << "BEQ ";
                                                break;
                                                case 8://ADDI (checked + tested)
                                                        ofs << setw(6) << "ADDI ";
                                                        //Format: rt, rs, immediate

                                                        ofs << determineRegister(rts[readCounter]) << ", "
                                                            << determineRegister(rss[readCounter]) << ", "
                                                            << "#"
                                                            << convertImmToInt(immediates[readCounter].to_string()); //Converts 16-bit binary immediate to signed int (I think it's two's comp?)
                                                            //<< immediates[readCounter].to_string();
                                                break;
                                                case 11://SW (checked + tested)
                                                        ofs << setw(6) << "SW ";
                                                        //Format: rt, offset(base)
                                                        ofs << determineRegister(rts[readCounter]) << ", "
                                                            << convertImmToInt(immediates[readCounter].to_string())
                                                            << "(" << determineRegister(rss[readCounter]) << ")";
                                                break;
                                                case 28://MUL (checked + tested)
                                                                        ofs << setw(6) << "MUL ";
                                                                        //Format: rd, rs, rt
                                                                        ofs << determineRegister(rds[readCounter]) << ", "
                                                                            << determineRegister(rss[readCounter]) << ", "
                                                                            << determineRegister(rts[readCounter]);
                                                
                                        }
                                        ofs << endl;


                                }
                        }
                        else if(breakflag != 0){
                                //int temp = commands[readCounter].to_ulong();
                                dataValues[dataValueIndex] = commands[readCounter].to_ulong();  //stores int value of data bitstring in array for printing
                                
                                dataValueIndex++;
                                numberOfDataEntries++;

                                ofs << (commands[readCounter]) 
                                    << "\t" 
                                    <<  instructionAddresses[readCounter] << " "
                                    << left << (int)commands[readCounter].to_ulong() 
                                    << endl;
                        }   
                        /*QUESTION: Will using streams to output to file produce problems later? 
                                         Unsure about using strings due to earlier warnings, 
                                         but it seems like a good way to precisely write the results to a file.
                                         
                                                ANSWER: Don't use strings. Use the already-stored bitset arrays.
                                                That way, you can re-use your if/else blocks from the translation stage
                                                in order to determine what to do with a given instruction.
                                        */
                        else{
                                cout << "Error: program reached end of switches but not end of instruction array..." << endl;
                        }
                        
                        

                        
                        readCounter++;
                        programCounterTracker += 4;
                }
        
        }
        ofs.close();
        dataValueIndex = 0;
        /*This code tests if arrays store the 32bit bitsets correctly*/
        /*for(int a = 0; a < readCounter; a++){
                cout << endl
                     << "--------------------------------------------------" << endl
                     << "Command " << a << ": " << commands[a] << endl
                     << "Validflag " << a << ": " << validflags[a] << endl
                     << "Opcode " << a << ": " << opcodes[a] << endl
                     << "RS " << a << ": " << rss[a] << endl
                     << "RT " << a << ": " << rts[a] << endl
                     << "RD " << a << ": " << rds[a] << endl
                     << "ShamT " << a << ": " << shamts[a] << endl
                     << "Funct " << a << ": " << functs[a] << endl
                     << "Immediate " << a << ": " << immediates[a] << endl
                     << "Address " << a << ": " << addresses[a] << endl
                     << "--------------------------------------------------" << endl;

        }*/
        /*END OF DISASSEMBLER*/

        /*START OF PIPELINE*/
        ofs.open(filenameSim);
        breakflag = 0;
        int cycleCounter = 0;   //cycleCounter tracks the cycles, loopCounter tracks the array index of the instr.

        int registers[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                    0, 0};
        int registerIndex = 0;
        int registerIndex2 = 0;
        int offsetRegIndex = 0;
        int destinationRegIndex = 0;
        int shiftAmtRegIndex = 0;
        int shiftAmt = 0;
        int constant = 0;

        int temp1 = 0;          //Temporary ints used as placeholders to make code more readable.
        int temp2 = 0;
        int temp3 = 0;
        int temp4 = 0;
        int temp5 = 0;
        string temp1String;
        string temp2String;
        string temp3String;
        string temp4String;
        string temp5String;
        bitset<32> temp1Word;
        bitset<32> temp2Word;
        bitset<32> temp3Word;
        bitset<32> temp4Word;
        bitset<32> temp5Word;

        cacheSet cache[4];
        

        for(int loopCounter = 0; loopCounter < readCounter &&  cycleCounter <= 50; loopCounter++){
                if(!ofs.is_open()){
                        cerr << "Failed to open simulation file, exiting & returning 1..." << endl;
                        exit(1);
                }
                if(breakflag == 0){

                }
        }
        /*END OF PIPELINE*/
        
}

/*DISASSEMBLER NEEDS THIS FUNCTION TO WORK*/

string determineRegister( bitset<5> regAddress ){
        //takes 5-bit bitstring, then sends back the register name as a char string.
        string result;
        switch(regAddress.to_ulong()){
                case 0:
                        result = "R0";
                break;
                case 1:
                        result = "R1";
                break;
                case 2:
                        result = "R2";
                break;
                case 3:
                        result = "R3";
                break;
                case 4:
                        result = "R4";
                break;
                case 5:
                        result = "R5";
                break;
                case 6:
                        result = "R6";
                break;
                case 7:
                        result = "R7";
                break;
                case 8:
                        result = "R8";
                break;
                case 9:
                        result = "R9";
                break;
                case 10:
                        result = "R10";
                break;

        }
        return result;
}

int determineRegisterInt( bitset<5> regAddress ){
        //takes 5-bit bitstring, then sends back the register name as a char string.
        int result;
        switch(regAddress.to_ulong()){
                case 0:
                        result = 0;
                break;
                case 1:
                        result = 1;
                break;
                case 2:
                        result = 2;
                break;
                case 3:
                        result = 3;
                break;
                case 4:
                        result = 4;
                break;
                case 5:
                        result = 5;
                break;
                case 6:
                        result = 6;
                break;
                case 7:
                        result = 7;
                break;
                case 8:
                        result = 8;
                break;
                case 9:
                        result = 9;
                break;
                case 10:
                        result = 10;
                break;

        }
        return result;
}

int convertImmToInt(string imm){        //Converts two's complement 16-bit string to signed int
        int tempResult;
        int power = 0;

        if(imm[0] == '0'){
                //Indicates positive number COMPLETED
                tempResult = 0; 
                for(int i = 16; i >= 1; i--){
                        if(imm[i] == '1'){
                                // cout << "Adding power " << power << ": " << (1<<power) << endl
                                //      << "tempResult: " << tempResult + (1<<power) << endl;
                                tempResult = tempResult + (1<<power);
                                power++;
                        }
                        else if(imm[i] == '0'){
                                power++;
                        }
                //---------------------------------------------
                }
                
                

        }
        if(imm[0] == '1'){
                //indicates negative number
                tempResult = -1;
                for(int i = 16; i >= 1; i--){
                        if(imm[i] == '0'){
                                // cout << "Subtracting power " << power << ": " << (1<<power) << endl
                                //      << "tempResult: " << tempResult - (1<<power) << endl;
                                tempResult = tempResult - (1<<power);
                                power++;
                        }
                        else if(imm[i] == '1'){
                                power++;
                        }
                //---------------------------------------------        
                }
        }
        // cout << "Final tempResult returned: " << tempResult << endl;
        return tempResult;

}

string printCache(cacheSet cache[], int numberOfSets){    //Here, numberOfSets would be 4, since there are cache entries 0-3
        stringstream ss;
        for(int i = 0; i < numberOfSets; i++){
                ss << "Set " << i << ": LRU=" << cache[i].LRU << endl
                     << "\t" << "Entry"
                     << "0:[(" << cache[i].LRU << "," << cache[i].valid << "," << cache[i].dirty << ")" 
                     << "<" << cache[i].block[0] << "," << cache[i].block[1] << ">]" << endl;
                ss << "\t" << "Entry"
                     << "1:[(" << cache[i + 1].LRU << "," << cache[i + 1].valid << "," << cache[i + 1].dirty << ")" 
                     << "<" << cache[i + 1].block[0] << "," << cache[i + 1].block[1] << ">]" << endl;
        }
        return ss.str();
}

void printBuffers(){

}

