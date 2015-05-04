#ifndef ____trackedHand__
#define ____trackedHand__

#include "ofMain.h"
#include "ofxOneDollar.h"
#include "circularBuffer.h"
#include "GestureEvent.h"
#include "analizador.h"

#define GESTURE_SIZE 100

enum HandType{
    L_PERCUSSION, R_PERCUSSION, GEO_FIGURE
};

class trackedHand{
public:
    trackedHand(int _handId, int _handType);
    ~trackedHand();
    void findOneDollarGesture();
    void findPercusiveGesture();
    void addPoint(float x, float y);
    void update();
    void draw();
    void showMessage(string sMessage);
    void showMessage(string sMessage, int nDelay);
    
    circularBuffer* smoothPosX;
    circularBuffer* smoothPosY;
    circularBuffer* gestureLineX;
    circularBuffer* gestureLineY;
    ofxOneDollar    dollar;
    ofxGesture*     gesture;
    Analizador*     percGestAnalizer;
    ofPolyline      line,found_gesture;
    double          score=0.0;
    
    bool            showText=false;
    string          message;
    int             hide_message_on;
    int             handId;
    
    HandType        handType;
};


#endif
