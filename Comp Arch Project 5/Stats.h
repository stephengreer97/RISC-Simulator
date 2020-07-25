#ifndef __STATS_H
#define __STATS_H
#include <iostream>

using namespace std;

enum PIPESTAGE { IF1 = 0, IF2 = 1, ID = 2, EXE1 = 3, EXE2 = 4, MEM1 = 5, 
                 MEM2 = 6, WB = 7, PIPESTAGES = 8, NOTHING = -1 };

class Stats {
  private:
    long long cycles;
    int flushes;
    int bubbles;
    int stalls;

    int memops;
    int branches;
    int taken;
    
    int hazards;
    int exe1Count;
    int exe2Count;
    int mem1Count;
    int mem2Count;

    int resultReg[PIPESTAGES];
    PIPESTAGE resultStage[PIPESTAGES];

  public:
    Stats();

    void clock();

    void flush(int count);
    
    void stall(int numStalls);

    void registerSrc(int r, PIPESTAGE stage);
    void registerDest(int r, PIPESTAGE stage);

    void countMemOp() { memops++; }
    void countBranch() { branches++; }
    void countTaken() { taken++; }

    // getters
    long long getCycles() { return cycles; }
    int getFlushes() { return flushes; }
    int getBubbles() { return bubbles; }
    int getMemOps() { return memops; }
    int getBranches() { return branches; }
    int getTaken() { return taken; }
    int getHazards() { return hazards; }
    int getEXE1() { return exe1Count; }
    int getEXE2() { return exe2Count; }
    int getMEM1() { return mem1Count; }
    int getMEM2() { return mem2Count; }
    int getStalls() { return stalls; }

  private:
    void bubble();
};

#endif
