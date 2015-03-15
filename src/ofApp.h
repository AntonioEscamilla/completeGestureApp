#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxPd.h"
#include "ofxPlaylist.h"
#include "animatedFigure.h"
#include "trackedHand.h"
#include "GestureEvent.h"

using namespace pd;
using namespace Playlist;

#define KINECT_W 640
#define KINECT_H 480

enum State{
    WELCOME, CIRCLE, C_DETECTED, TRIANGLE, T_DETECTED, RECTANGLE, R_DETECTED
};

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
    
        void audioReceived(float * input, int bufferSize, int nChannels);
        void audioRequested(float * output, int bufferSize, int nChannels);
    
        void countTime(int nDelay);
        void countTimeReset();
        void gestureDetected(GestureEvent &e);
        void onKeyframe(ofxPlaylistEventArgs& args);
        void runAnimPlaylist();
    
    private:
        void handEvent(ofxOpenNIHandEvent & event);
    
        ofxOpenNI       openNIDevice;
        ofPoint         mappedHandPos;
        vector<trackedHand*> gesturedHands;
        ofxPd           pd;
        Patch           patch;
        State           estadoActual;
        ofxPlaylist     timeEvents;
        animatedFigure* animatedCircle;
        animatedFigure* animatedTriangle;
        animatedFigure* animatedRect;
};
