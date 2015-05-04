#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxPd.h"
#include "ofxPlaylist.h"
#include "animatedFigure.h"
#include "trackedHand.h"
#include "GestureEvent.h"
#include  "ofxUI.h"

using namespace pd;
using namespace Playlist;

#define KINECT_W 640
#define KINECT_H 480

enum State{
    IDLE, WELCOME, CIRCLE, C_DETECTED, TRIANGLE, T_DETECTED, RECTANGLE, R_DETECTED, EXC1_1, EXC1_2
};

enum ExerciseType{
    EXC_NONE, EXC_ONE, EXC_TWO, EXC_THREE
};

class ofApp : public ofBaseApp, public PdReceiver{

    public:
        void setup();
        void update();
        void draw();
        void startOpenNI(int numHands);
    
        void audioReceived(float * input, int bufferSize, int nChannels);
        void audioRequested(float * output, int bufferSize, int nChannels);
    
        void countTime(int nDelay);
        void countTimeReset();
        void gestureDetected(GestureEvent &e);
        void onKeyframe(ofxPlaylistEventArgs& args);
        void runAnimPlaylist();
    
        ofxUISuperCanvas *gui;
        ofxUISuperCanvas *gui1;
        void exit();
        void guiEvent(ofxUIEventArgs &e);
        void setGUI();
        void setGUI1();
        int EXC1_LEVEL_OP;
        int EXC1_VEL_OP;
    
        void print(const std::string& message);
        void receiveFloat(const std::string& dest, float value);
        void receiveBang(const std::string& dest);

    
    private:
        void handEvent(ofxOpenNIHandEvent & event);
        ofxOpenNI       openNIDevice;
        ofPoint         mappedHandPos;
        vector<trackedHand*> gesturedHands;
        ofxPd           pdObject;
        Patch           patch;
        State           estadoActual;
        ExerciseType    ejercicioActual;
        ofxPlaylist     timeEvents;
        animatedFigure* animatedCircle;
        animatedFigure* animatedTriangle;
        animatedFigure* animatedRect;
};
