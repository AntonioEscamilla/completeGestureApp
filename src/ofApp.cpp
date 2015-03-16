#include "ofApp.h"
#include <Poco/Path.h>

/*************************************************************************/
void ofApp::setup(){
    
    openNIDevice.setup();
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    // setup the hand generator
    openNIDevice.addHandsGenerator();
    openNIDevice.addHandFocusGesture("Wave");
    openNIDevice.setMaxNumHands(2);
    openNIDevice.start();
    
    // setup OF sound stream & Pd patch
    int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    int numInputs = 1;
    ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    if(!pd.init(2, numInputs, 44100, ticksPerBuffer)) {
        OF_EXIT_APP(1);
    }
    pd.start();
    patch = pd.openPatch("VanillaSamplers/Ejercicio3.pd");
   
    animatedCircle = new animatedFigure(0.5,0.5);
    animatedTriangle = new animatedFigure(0.5,0.1);
    animatedRect = new animatedFigure(0.9,0.1);
    ofxKeyframeAnimRegisterEvents(this);
    
    ofAddListener(openNIDevice.handEvent, this, &ofApp::handEvent);
    ofAddListener(GestureEvent::gestureDetected, this, &ofApp::gestureDetected);
    timeEvents.addKeyFrame(Action::event(this,"GOTO_WELCOME"));
}

/*************************************************************************/
void ofApp::update(){
    openNIDevice.update();
    timeEvents.update();
    animatedCircle->playlist.update();
    animatedTriangle->playlist.update();
    animatedRect->playlist.update();
    
    for (int i = 0; i < openNIDevice.getNumTrackedHands(); i++){
        ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
        ofPoint & handPosition = hand.getPosition();
        mappedHandPos.x = ofMap(ofClamp(handPosition.x,1,KINECT_W),1,KINECT_W,1,ofGetWidth());
        mappedHandPos.y = ofMap(ofClamp(handPosition.y,1,KINECT_H),1,KINECT_H,1,ofGetHeight());
        gesturedHands[i]->addPoint(mappedHandPos.x, mappedHandPos.y);
        gesturedHands[i]->update();
    }
}

/*************************************************************************/
void ofApp::draw(){
    ofSetColor(255,255,255);
    
    openNIDevice.drawDepth(0,0,ofGetWidth(),ofGetHeight());
    for (int i = 0; i < openNIDevice.getNumTrackedHands(); i++){
        gesturedHands[i]->draw();
    }
    
    switch (estadoActual) {
        case CIRCLE:
            animatedCircle->draw();
            break;
            
        case TRIANGLE:
            animatedTriangle->draw();
            break;
            
        case RECTANGLE:
            animatedRect->draw();
            break;
            
        default:
            break;
    }

}

/*************************************************************************/
void ofApp::runAnimPlaylist() {
    switch (estadoActual) {
        case CIRCLE:
            animatedCircle->playlist.addKeyFrame(Action::tween(360,&animatedCircle->angulo,360, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedCircle->playlist.addKeyFrame(Action::tween(1,&animatedCircle->angulo,0, TWEEN_QUAD,TWEEN_EASE_OUT));
            break;
            
        case TRIANGLE:
            animatedTriangle->playlist.addKeyFrame(Action::tween(120,&animatedTriangle->pos.y,0.9, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedTriangle->playlist.addToKeyFrame(Action::tween(120,&animatedTriangle->pos.x,0.1, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedTriangle->playlist.addKeyFrame(Action::tween(120,&animatedTriangle->pos.y,0.9, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedTriangle->playlist.addToKeyFrame(Action::tween(120,&animatedTriangle->pos.x,0.9, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedTriangle->playlist.addKeyFrame(Action::tween(120,&animatedTriangle->pos.y, 0.1, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedTriangle->playlist.addToKeyFrame(Action::tween(120,&animatedTriangle->pos.x,0.5, TWEEN_QUAD,TWEEN_EASE_OUT));
            break;
            
        case RECTANGLE:
            animatedRect->playlist.addKeyFrame(Action::tween(90,&animatedRect->pos.x,0.1, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedRect->playlist.addKeyFrame(Action::tween(90,&animatedRect->pos.y,0.9, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedRect->playlist.addKeyFrame(Action::tween(90,&animatedRect->pos.x, 0.9, TWEEN_QUAD,TWEEN_EASE_OUT));
            animatedRect->playlist.addKeyFrame(Action::tween(90,&animatedRect->pos.y,0.1, TWEEN_QUAD,TWEEN_EASE_OUT));
            break;
            
        default:
            break;
    }
}

/*************************************************************************/
/*                          EVENTOS                                      */
/*************************************************************************/
void ofApp::gestureDetected(GestureEvent &e){
    ofLogNotice() << "Evento de deteccion de un " << e.message << " como gesto";
    if (e.message=="circulo" && estadoActual==CIRCLE){
        estadoActual=C_DETECTED;
        pd.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pd.sendBang(patch.dollarZeroStr()+"-circleDetected");
        timeEvents.addKeyFrame(Action::pause(4*60)).addKeyFrame(Action::event(this,"GOTO_TRIANGLE"));
    }
    if (e.message=="triangulo" && estadoActual==TRIANGLE){
        estadoActual=T_DETECTED;
        pd.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pd.sendBang(patch.dollarZeroStr()+"-triangleDetected");
        timeEvents.addKeyFrame(Action::pause(4*60)).addKeyFrame(Action::event(this,"GOTO_RECTANGLE"));
    }
    if (e.message=="cuadrado" && estadoActual==RECTANGLE){
        estadoActual=R_DETECTED;
        pd.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pd.sendBang(patch.dollarZeroStr()+"-rectangleDetected");
        timeEvents.addKeyFrame(Action::pause(3*60)).addKeyFrame(Action::event(this,"GOTO_WELCOME"));
    }
}

//*************************************************************************/
void ofApp::handEvent(ofxOpenNIHandEvent & event){
    if (event.handStatus!=HAND_TRACKING_UPDATED){
        ofLogNotice() << getHandStatusAsString(event.handStatus) << "for hand" << event.id << "from device" << event.deviceID;
    }
    if(event.handStatus==HAND_TRACKING_STARTED){
        gesturedHands.push_back(new trackedHand(event.id));
    }
    if (event.handStatus==HAND_TRACKING_STOPPED) {
        ofLogNotice() << "antes del HAND_TRACKING_STOPPED: " << "gesturedHands size  = " << gesturedHands.size();
        for (int i = gesturedHands.size()-1;i>=0;i--){
            if (gesturedHands[i]->handId==event.id) {
                gesturedHands.erase(gesturedHands.begin()+i);
            }
        }
        ofLogNotice() << "despues del HAND_TRACKING_STOPPED: " << "gesturedHands size  = " << gesturedHands.size();
    }
}

//*************************************************************************/
void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
    pd.audioIn(input, bufferSize, nChannels);
}

//*************************************************************************/
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
    pd.audioOut(output, bufferSize, nChannels);
}

//*************************************************************************/
void ofApp::onKeyframe(ofxPlaylistEventArgs& args){
    ofLogNotice() << "Keyframe Event received: " << args.message;
    if (args.message=="GOTO_WELCOME"){
        estadoActual=WELCOME;
        pd.sendBang(patch.dollarZeroStr()+"-welcome");
        timeEvents.addKeyFrame(Action::pause(9*58)).addKeyFrame(Action::event(this,"GOTO_CIRCLE"));
    }
    else if(args.message=="GOTO_CIRCLE"){
        estadoActual=CIRCLE;
        pd.sendBang(patch.dollarZeroStr()+"-circle");
        animatedCircle->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_CIRCLE"));
    }
    else if(args.message=="GOTO_TRIANGLE"){
        estadoActual=TRIANGLE;
        pd.sendBang(patch.dollarZeroStr()+"-triangle");
        animatedTriangle->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_TRIANGLE"));
    }
    else if(args.message=="GOTO_RECTANGLE"){
        estadoActual=RECTANGLE;
        pd.sendBang(patch.dollarZeroStr()+"-rectangle");
        animatedRect->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_RECTANGLE"));
    }
    
}
