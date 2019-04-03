//Fetch 2 instr.s at a time.  Problem is that when there is a cache miss for the first instr, you're not supposed to request the second
//  until the first is handled (because the results of handling the first instruction could cause the query for the second instr. to change).

//Pseudocode for the structure of fetching instructions from the cache:
for(i=0; i < 2, i++){
    result = cache(96 + 4)  //cache(<instr. addr>) should return either a hit or a miss.
    if(result != hit){
        break;
    }
    else if(result == branch || result == jump /*Use opcodes to determine this*/){    
        
        /*Easier to (stop, then handle, then continue) rather than to 
        fetch two instr.s at once, then 'undo' a cache miss,
        so we will do the following:*/

        //1) Stop doing what you're doing
        //2) Handle the conditional using the opcodes:

            //if(opcode == branch){ handle branch }
            //else if (opcode == jump) { handle jump }

        //Don't attempt to query the cache for the second address until the branch/jump is handled.

    }
    else {  //It's a miss

        /*  1) Get the (instruction to be retrieved) from the array of instructions, 
            2) write it to the cache, 
            3) loop back up while decrementing PC by 4 so the program 
                tries again (and hopefully gets a hit this time.)*/
    }
}