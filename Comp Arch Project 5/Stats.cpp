/******************************
 * Submitted by: Stephen Greer s_g435
 * CS 3339 - Fall 2019, Texas State University
 * Project 3 Pipelining
 * Copyright 2019, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "Stats.h"

Stats::Stats() {
  cycles = PIPESTAGES - 1; // pipeline startup cost
  flushes = 0;
  bubbles = 0;
  stalls = 0;

  memops = 0;
  branches = 0;
  taken = 0;
    
  hazards = 0;
  exe1Count = 0;
  exe2Count = 0;
  mem1Count = 0;
  mem2Count = 0;

  for(int i = IF1; i < PIPESTAGES; i++) {
    resultReg[i] = -1;
  }
    
  for(int i = IF1; i < PIPESTAGES; i++) {
    resultStage[i] = ID;
  }
}

void Stats::clock() {
  cycles++;

  // advance all pipeline flip-flops
  for(int i = WB; i > IF1; i--) {
    resultReg[i] = resultReg[i-1];
  }
  resultReg[IF1] = -1;
    
  for(int i = WB; i > IF1; i--) {
      resultStage[i] = resultStage[i-1];
  }
  resultStage[IF1] = ID;
}

void Stats::registerSrc(int r, PIPESTAGE stage) {
    //checking for hazards and calling bubble if there are any
    int tempBubbles = 0;
    bool hazard = false;
    
    for(int i = 3; i < WB; i++){
        if(r == resultReg[i] && r != 0){
            if(i == EXE1){
                exe1Count++;
            }
            else if(i == MEM1 && hazard == false){
                mem1Count++;
            }
            else if(i == EXE2 && hazard == false){
                exe2Count++;
            }
            else if(i == MEM2 && hazard == false){
                mem2Count++;
            }
            int valid = resultStage[i] - i;
            int needed = stage - ID;
            tempBubbles = valid - needed;
            if(hazard== false)
                hazards++;
            hazard = true;
            for (int j = 0; j < tempBubbles; j++){
                bubble();
            }
            if(tempBubbles > 0){
                i += tempBubbles;
            }
        }
    }
}

void Stats::registerDest(int r, PIPESTAGE stage) {
    //stores the register being used in resultReg
    resultReg[ID] = r;
    resultStage[ID] = stage;
}

void Stats::flush(int count) { // count == how many ops to flush
    
    flushes += count;
    clock();
    clock();
}

void Stats::bubble() {
    cycles++;
    bubbles++;
    //shifting over everything past ID
    for(int i = WB; i > ID; i--) {
        resultReg[i] = resultReg[i-1];
    }
    
    resultReg[EXE1] = -1;
    
    for(int i = WB; i > ID; i--) {
        resultStage[i] = resultStage[i-1];
    }
    
    resultStage[EXE1] = ID;
}

void Stats::stall(int numStalls) {
    stalls += numStalls;
    cycles += numStalls;
}

