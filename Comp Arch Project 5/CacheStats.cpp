/******************************
 * Submitted by: Stephen Greer s_g435
 * CS 3339 - Fall 2019, Texas State University
 * Project 5 Data Cache
 * Copyright 2019, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "CacheStats.h"
#include "Stats.h"
using namespace std;

CacheStats::CacheStats() {
  cout << "Cache Config: ";
  if(!CACHE_EN) {
    cout << "cache disabled" << endl;
  } else {
    cout << (SETS * WAYS * BLOCKSIZE) << " B (";
    cout << BLOCKSIZE << " bytes/block, " << SETS << " sets, " << WAYS << " ways)" << endl;
    cout << "  Latencies: Lookup = " << LOOKUP_LATENCY << " cycles, ";
    cout << "Read = " << READ_LATENCY << " cycles, ";
    cout << "Write = " << WRITE_LATENCY << " cycles" << endl;
  }
    
  loads = 0;
  stores = 0;
  load_misses = 0;
  store_misses = 0;
  writebacks = 0;

    for (int i = 0; i < WAYS; i++){
        for (int j = 0; j < SETS; j++){
            tagArray[i][j] = 0;
            dirty[i][j] = 0;
            valid[i][j] = false;
        }
    }
    for (int i = 0; i < SETS; i++)
        roundRobin[i] = 0;
}

int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {
    if(!CACHE_EN) { // cache disabled
        return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
    }
    if(type == LOAD)
        loads++;
    else
        stores++;
    
    int hitWay;
    bool hit = false;
    index = (addr >> 5) & 0x07;
    tag = (addr >> 8);
    
    for(int wayIncrement = 0; wayIncrement < WAYS; wayIncrement++){
        if(tagArray[index][wayIncrement] == tag){
            if(valid[index][wayIncrement] == true){
                hitWay = wayIncrement;
                hit = true;
                break;
            }
            break;
        }
    }
    //if its a hit
    if(hit == true){
        //if its write, set dirty, return
        if(type == STORE){
            dirty[index][hitWay] = true;
            stats2.stall(LOOKUP_LATENCY);
            return 0;
        }
        //if its read, return
        else{
            stats2.stall(LOOKUP_LATENCY);
            return 0;
        }
    }
    //if its a miss
    else {
        //if dirty
        if(dirty[index][roundRobin[index]] == true){
            writebacks++;
            stats2.stall(WRITE_LATENCY);
        }
    
        stats2.stall(READ_LATENCY);
        //if load, set dirty to false
        if(type == LOAD)
            dirty[index][roundRobin[index]] = false;
        //if store, set dirty to true
        else
            dirty[index][roundRobin[index]] = true;
        valid[index][roundRobin[index]] = true;
        tagArray[index][roundRobin[index]] = tag;
        roundRobin[index] = (roundRobin[index] + 1) % 4;
        
        if(type == LOAD){
            load_misses++;
        }
        else
            store_misses++;
        
        return 0;
    }
}
void CacheStats::printFinalStats() {
    
    for(int i = 0; i < SETS; i++){
        for(int j = 0; j < WAYS; j++)
        {
            if(dirty[i][j]==1){
                writebacks++;
            }
        }
    }
    
  int accesses = loads + stores;
  int misses = load_misses + store_misses;
  cout << "Accesses: " << accesses << endl;
  cout << "  Loads: " << loads << endl;
  cout << "  Stores: " << stores << endl;
  cout << "Misses: " << misses << endl;
  cout << "  Load misses: " << load_misses << endl;
  cout << "  Store misses: " << store_misses << endl;
  cout << "Writebacks: " << writebacks << endl;
  cout << "Hit Ratio: " << fixed << setprecision(1) << 100.0 * (accesses - misses) / accesses;
  cout << "%" << endl;
}
