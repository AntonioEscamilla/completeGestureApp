#include "circularBuffer.h"

circularBuffer::circularBuffer(int _size){
    size=_size;
    buffer = new float[size];
    memset (buffer, 0.f, size * sizeof (float));
    pW=size-1;
}
    
void circularBuffer::write(float value){
    pW++;
    int wIndex=pW%size;
    buffer[wIndex]=value;
}

float circularBuffer::sumDiff(){
    float sumDiff=0;
    for(int i=pW;i>pW-size+1;i--){
        sumDiff += abs(buffer[i%size]-buffer[(i-1)%size]);
    }
    return sumDiff/(size-1);
}

void circularBuffer::randomReset(){
    for(int i=0;i<size;i++){
        buffer[i]=ofRandom(1,5000);
    }
    pW=size-1;
}

void circularBuffer::resetAtPoint(int point){
    for(int i=0;i<size;i++){
        buffer[i]=point;
    }
    pW=size-1;
}

void circularBuffer::resetAtLastPoint(){
    int point = buffer[pW%size];
    for(int i=0;i<size;i++){
        buffer[i]=point;
    }
    pW=size-1;
}

void circularBuffer::clear(){
    memset (buffer, 0.f, size * sizeof (float));
    pW=size-1;
}

void circularBuffer::print(){
    for(int i=0;i<size;i++){
        ofLog() << "posicion"<< i << " = " << buffer[i];
    }
    ofLog() << "PROMEDIO =" << this->sumDiff();
}

float circularBuffer::mean(){  //promedio de todos los valores en el buffer
    float sum=0;
    for(int i=pW;i>pW-size;i--){
        sum += buffer[i%size];
    }
    return sum/size;
}

float circularBuffer::lerped(){  //interpola la nueva posicion con la anterior (tendiendo a la anterior posicion)
    return ofLerp(buffer[pW%size],buffer[(pW-1)%size],0.9f);
}

