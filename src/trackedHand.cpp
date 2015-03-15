#include "trackedHand.h"

ofEvent <GestureEvent> GestureEvent::gestureDetected;

/*************************************************************************/
trackedHand::trackedHand(){
    dollar.load(ofToDataPath("gestures.txt",true));
    //showMessage("Loaded! Note that saved gestures are rotated to the optimal position!",3000);
    gestureLineX = new circularBuffer(GESTURE_SIZE);
    gestureLineY = new circularBuffer(GESTURE_SIZE);
}

/*************************************************************************/
void trackedHand::addPoint(float x, float y){
    gestureLineX->write(x);
    gestureLineY->write(y);
}

/*************************************************************************/
void trackedHand::update(){
    if (showText){
        if(ofGetElapsedTimeMillis() >= hide_message_on){
            showText = false;
            found_gesture.clear();
        }
    }
    else{
        findGesture();
    }
}

/*************************************************************************/
void trackedHand::draw(){
    if(showText){
        ofSetColor(255, 0, 140);
        ofDrawBitmapString(message, 10, 30);
    }else{
        ofSetColor(255, 255, 0);
        line.draw();
    }
    
    ofSetColor(0, 255, 140);
    found_gesture.draw();
}

/*************************************************************************/
void trackedHand::findGesture(){
    ofxGesture* tmp = new ofxGesture();
    line.clear();
    for(int i=gestureLineX->pW;i>gestureLineX->pW-GESTURE_SIZE;i--){
        tmp->addPoint (gestureLineX->buffer[i%GESTURE_SIZE],gestureLineY->buffer[i%GESTURE_SIZE]);
        line.addVertex(ofVec2f(gestureLineX->buffer[i%GESTURE_SIZE],gestureLineY->buffer[i%GESTURE_SIZE]));
    }
    
    double score = 0.0;
    ofxGesture* match = dollar.match(tmp, &score);
    if(match != NULL && score>0.85) {
        string result = "Matching score: " +ofToString(score);
        result +=", which matches with gesture: " +match->name;
        found_gesture.clear();
        float dx = ofGetWidth()/2;
        float dy = ofGetHeight()/2;
        for(int i = 0; i < match->resampled_points.size(); ++i) {
            found_gesture.addVertex(ofVec2f(dx+match->resampled_points[i].x, dy+match->resampled_points[i].y));
        }
        gestureLineX->resetAtLastPoint();
        gestureLineY->resetAtLastPoint();
        line.clear();
        static GestureEvent detected;
        detected.message = match->name;
        ofNotifyEvent(GestureEvent::gestureDetected, detected);
        showMessage(result, 500);
    }
    delete tmp;
}

/*************************************************************************/
void trackedHand::showMessage(string sMessage) {
    message = sMessage;
    showText = true ;
    hide_message_on = ofGetElapsedTimeMillis()+1000;
}

/*************************************************************************/
void trackedHand::showMessage(string sMessage, int nDelay) {
    message = sMessage;
    showText = true ;
    hide_message_on = ofGetElapsedTimeMillis() + nDelay;
}
