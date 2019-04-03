/*BEGINNING OF INTERPRETER*/
        /*At this point, all of the instructions are stored correctly in the arrays (Whether they are valid or not.)
          Now it is time to interpret the parsed instructions using the SAME SWITCH STATEMENT LOGIC AS THE DISASSEMBLER.*/

        /*Must support the following commands:
         * J
         * JR
         * BEQ
         * BLTZ
         * ADD
         * ADDI
         * SUB
         * SW
         * LW
         * SLL
         * SRL
         * MUL
         * AND
         * OR
         * MOVZ
         * NOP
         * */
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
        

        for(int loopCounter = 0; loopCounter < readCounter &&  cycleCounter <= 50; loopCounter++){        //FIXME: remove the cycleCounter limit after branch dependencies are fixed
                if(!ofs.is_open()){
                cerr << "Failed to open simulation file, exiting & returning 1..." << endl;
                exit(1);
                }
                

                if(breakflag == 0){


                        if (validflags[loopCounter].to_ulong() == 0){//else if invalid instruction, then:
                                //Invalid instruction; don't do anything except increment cycleCounter
                                
                        }
                        if(validflags[loopCounter].to_ulong() == 1){    //If valid instruction, then translate.

                                ofs << "--------------------" << endl
                                    << "Cycle:" << cycleCounter + 1 << " "
                                    << setw(4) << instructionAddresses[loopCounter];
                                

                                switch (opcodes[loopCounter].to_ulong()) {      /*Prints out operation, determined by opcode & funct*/
                                        case 0://R-TYPE
                                                //Now we must decide between all R instr.s using funct
                                                switch(functs[loopCounter].to_ulong()){         //All of the R-instr.s are complete
                                                        case 0://SLL or NOP (checked + tested) & DONE
                                                                
                                                                //if (the whole instruction is 0s): it's a NOP (checked + tested) & DONE
                                                                if( (rss[loopCounter] == 0)&&(rts[loopCounter] == 0)&&(rds[loopCounter] == 0)&&(shamts[loopCounter] == 0) ){   
                                                                        ofs << setw(6) <<"NOP ";
                                                                        //No operation
                                                                }
                                                                //else: it's an SLL(checked + tested) & DONE
                                                                else{   
                                                                        ofs << setw(6) << "SLL ";
                                                                        //Format: rd, rt, sa
                                                                        ofs << determineRegister(rds[loopCounter]) << ", "      /*Do not use PCcounter, since it starts at 96*/
                                                                                << determineRegister(rts[loopCounter]) << ", "
                                                                                << "#" << shamts[loopCounter].to_ulong();
                                                                        //Operation:
                                                                        offsetRegIndex = determineRegisterInt(rts[loopCounter]);
                                                                        destinationRegIndex = determineRegisterInt(rds[loopCounter]);
                                                                        shiftAmtRegIndex = determineRegisterInt(shamts[loopCounter]);
                                                                        shiftAmt = registers[shiftAmtRegIndex];
                                                                         
                                                                        registers[destinationRegIndex] = registers[offsetRegIndex]<<shiftAmt;
                                                                        

                                                                }
                                                        break;
                                                        case 2://SRL (checked + tested) & DONE
                                                                ofs << setw(6) << "SRL ";
                                                                //Format: rd, rt, sa
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]) << ", "
                                                                        << "#" << shamts[loopCounter].to_ulong();

                                                                //Operation:
                                                                offsetRegIndex = determineRegisterInt(rts[loopCounter]);
                                                                destinationRegIndex = determineRegisterInt(rds[loopCounter]);
                                                                shiftAmtRegIndex = determineRegisterInt(shamts[loopCounter]);
                                                                shiftAmt = registers[shiftAmtRegIndex];

                                                                registers[destinationRegIndex] = registers[offsetRegIndex]>>shiftAmt;



                                                        break;
                                                        case 8://JR (checked + tested)
                                                                ofs << setw(6) << "JR ";
                                                                //Format: rs
                                                                ofs << determineRegister(rss[loopCounter]);

                                                                //Operation:    PC = rs

                                                        break;
                                                        case 10://MOVZ (checked + tested)
                                                                ofs << setw(6) << "MOVZ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                                        break;
                                                        case 13://BREAK (checked + tested)
                                                                ofs << setw(6) << "BREAK ";
                                                                breakflag = readCounter;                    //Trips breakflag, signals that it's time to start reading raw data as ints.
                                                        break;
                                                        case 32://ADD (checked + tested)
                                                                ofs << setw(6) << "ADD ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                                        break;
                                                        case 34://SUB (checked + tested)
                                                                ofs << setw(6) << "SUB ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                                        break;
                                                        case 36://AND (not present)
                                                                ofs << setw(6) << "AND ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                                        break;
                                                        case 37://OR (not present)
                                                                ofs << setw(6) << "OR ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                                        break;

                                                        /*End of R-switches determining operation
                                                                Now it's time to see what the rs, rt, and rd bits are.
                                                                */

                                                }
                                        break;
                                        case 1://BLTZ (checked + tested) 
                                                ofs << setw(6) << "BLTZ ";
                                                //Format: rs, offset
                                                ofs << determineRegister(rss[loopCounter]) << ", "
                                                        << "#" << immediates[loopCounter].to_ulong() * 4;   // immediate is multiplied by 4 to give correct offset amount

                                                //Operation: if rs < 0, then branch to instruction @ offset.
                                                temp1 = registers[determineRegisterInt(rss[loopCounter]) < 0];  //temp 1 = value inside register rt
                                                temp2 = immediates[loopCounter].to_ulong() * 4;                  //temp2 = integer value of offset (address to jump to)
                                                //index * 4 = instr. address, so instr. address / 4 = index to jump to
                                                temp3 = temp2 / 4;       //temp3 = index of command to jump to if condition is met
                                                if(registers[determineRegisterInt(rss[loopCounter])] < 0){
                                                        loopCounter = temp3;
                                                }
                                        break;
                                        case 2://J (checked + tested)
                                                ofs << setw(6) << "J ";
                                                //Format: address
                                                ofs << "#"
                                                        << addresses[loopCounter].to_ulong() * 4;
                                        break;
                                        case 3://LW (checked + tested) & DONE
                                                ofs << setw(6) << "LW ";
                                                //Format: rt, offset(base)
                                                ofs << determineRegister(rts[loopCounter]) << ", "
                                                        << immediates[loopCounter].to_ulong()
                                                        << "(" << determineRegister(rss[loopCounter]) << ")";
                                                
                                                //Operation:    rt = memory[base + offset]
                                                temp1 = registers[determineRegisterInt(rss[cycleCounter])];//base
                                                temp2 = (convertImmToInt(immediates[cycleCounter].to_string()) - dataStartingAddress) / 4;//offset
                                                //temp2 = (convertImmToInt(immediates[cycleCounter].to_string())) / 4;
                                                temp3 = registers[determineRegisterInt(rts[cycleCounter])];//rt register index
                                                temp4 = determineRegisterInt(rts[cycleCounter]);
                                                temp3 = dataValues[temp1 + temp2];
                                                registers[temp4] = temp3;
                                                
                                                //CONTINUE: Determine if this is working properly on cycle 3
                                        break;
                                        case 4://BEQ (not present)
                                                ofs << setw(6) << "BEQ ";
                                        break;
                                        case 8://ADDI (checked + tested) & DONE
                                                ofs << setw(6) << "ADDI ";
                                                //Format: rt, rs, immediate

                                                ofs << determineRegister(rts[loopCounter]) << ", "
                                                        << determineRegister(rss[loopCounter]) << ", "
                                                        << "#"
                                                        << convertImmToInt(immediates[loopCounter].to_string()); //Converts 16-bit binary immediate to signed int (two's complement)
                                                        //<< immediates[loopCounter].to_string();
                                                //Operation:
                                                registerIndex = determineRegisterInt(rts[loopCounter]);
                                                registerIndex2 = determineRegisterInt(rss[loopCounter]);
                                                constant = convertImmToInt(immediates[loopCounter].to_string());

                                                registers[registerIndex] = registers[registerIndex2] + constant;
                                                



                                        break;
                                        case 11://SW (checked + tested) & DONE
                                                ofs << setw(6) << "SW ";
                                                //Format: rt, offset(base)
                                                ofs << determineRegister(rts[loopCounter]) << ", "
                                                        << convertImmToInt(immediates[loopCounter].to_string())
                                                        << "(" << determineRegister(rss[loopCounter]) << ")";

                                                //Operation:    memory[base + offset] = rt 
                                                temp1 = registers[determineRegisterInt(rss[cycleCounter])];//base
                                                temp2 = (convertImmToInt(immediates[cycleCounter + 1].to_string()) - dataStartingAddress) / 4;//offset
                                                temp3 = registers[determineRegisterInt(rts[cycleCounter])];//rt register index

                                                dataValues[temp1 + temp2] = temp3;

                                        break;
                                        case 28://MUL (checked + tested)
                                                                ofs << setw(6) << "MUL ";
                                                                //Format: rd, rs, rt
                                                                ofs << determineRegister(rds[loopCounter]) << ", "
                                                                        << determineRegister(rss[loopCounter]) << ", "
                                                                        << determineRegister(rts[loopCounter]);
                                        break;
                                        if(breakflag != 0){
                                                //do nothing
                                        }
                                        
                                }
                                ofs << endl << endl << "Pre-Issue Buffer: " << endl
                                            << "\tEntry 0: " << endl
                                            << "\tEntry 1: " << endl
                                            << "\tEntry 2: " << endl
                                            << "\tEntry 3: " << endl
                                            << "Pre_ALU Queue: " << endl
                                            << "\tEntry 0: " << endl
                                            << "\tEntry 1: " << endl
                                            << "Post_ALU Queue: " << endl
                                            << "\tEntry 0: " << endl
                                            << "Pre_MEM Queue: " << endl
                                            << "\tEntry 0: " << endl
                                            << "\tEntry 1: " << endl
                                            << "Post_MEM Queue: " << endl
                                            << "\tEntry 0: " << endl << endl;

                                ofs << "Registers: " << endl
                                    << "R00:\t" 
                                    << registers[0] << "\t" 
                                    << registers[1] << "\t"
                                    << registers[2] << "\t"
                                    << registers[3] << "\t"
                                    << registers[4] << "\t"
                                    << registers[5] << "\t"
                                    << registers[6] << "\t"
                                    << registers[7]
                                    << endl
                                    << "R08:\t"
                                    << registers[8]<< "\t" 
                                    << registers[9]<< "\t"
                                    << registers[10] << "\t"
                                    << registers[11] << "\t"
                                    << registers[12] << "\t"
                                    << registers[13] << "\t"
                                    << registers[14] << "\t"
                                    << registers[15]
                                    << endl
                                    << "R16:\t"
                                    << registers[16] << "\t" 
                                    << registers[17] << "\t"
                                    << registers[18] << "\t"
                                    << registers[19] << "\t"
                                    << registers[20] << "\t"
                                    << registers[21] << "\t"
                                    << registers[22] << "\t"
                                    << registers[23]
                                    << endl
                                    << "R24:\t"
                                    << registers[24] << "\t" 
                                    << registers[25] << "\t"
                                    << registers[26] << "\t"
                                    << registers[27] << "\t"
                                    << registers[28] << "\t"
                                    << registers[29] << "\t"
                                    << registers[30] << "\t"
                                    << registers[31]
                                    << endl
                                    << endl;
                                
                                ofs << "Cache" << endl;


                                ofs << printCache(cache, 4) << endl;

                                ofs << "Data: " << endl;                         //Data value outputs

                                //FIXED: reverse the output
                                int dataAddressIter = dataStartingAddress;      //DataAddressIter just tracks where every 8th dataValue address will fall, for printing purposes
                                int dataIndexIter = dataValueIndex;             //DataIndexIter tracks which array entry will print.
                                
                                // for(int x = 0; x < numberOfDataEntries; x += 8){     //FIXED: the datavals are STORED in dataValues[] IN ORDER, but are PRINTED in REVERSE ORDER.
                                //         ofs << dataAddressIter << ": \t"
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t"  
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t"
                                //             << dataValues[dataIndexIter++] << "\t" << endl;
                                //             dataAddressIter += 32;
                                // }

                                for(int x = 0; x < numberOfDataEntries; x += 8){     //FIXED: the datavals are STORED in dataValues[] IN ORDER, but are PRINTED in REVERSE ORDER.
                                        ofs << dataAddressIter << ": \t";

                                        dataIndexIter += 7;

                                        ofs << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t"  
                                            << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t"
                                            << dataValues[dataIndexIter--] << "\t" << endl;
                                            dataAddressIter += 32;
                                        dataIndexIter += 9;
                                }
                                ofs << endl;
                                cycleCounter++;
                        }
                }
                else if(breakflag != 0){
                        //ofs << (commands[loopCounter]) //FIXED: Take this section out afterwards; it only appends the data section to end of simulation file
                                //<< "\t" 
                                //<<  instructionAddresses[loopCounter] << " "
                                //<< left << (int)commands[loopCounter].to_ulong() 
                                //<< endl;
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
        }
        /*END OF INTERPRETER*/