#ifndef __circularBuffer__
#define __circularBuffer__

#include "ofMain.h"

class circularBuffer{
public:
    circularBuffer(int size);
    void write(float value);
    void writeLerped(float value);
    float sumDiff();
    void randomReset();
    void clear();
    void print();
    float mean();
    float lerped();
    void resetAtPoint(int point);   //modificacion para uso en gesture tracking-detection
    void resetAtLastPoint();        //modificacion para uso en gesture tracking-detection
    
    int size;
    float* buffer;
    int pW;

};

#endif
