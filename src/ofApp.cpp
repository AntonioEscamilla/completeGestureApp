#include "ofApp.h"
#include <Poco/Path.h>

/*************************************************************************/
void ofApp::setup(){
    // setup OF sound stream & Pd patch
    int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    int numInputs = 1;
    ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    if(!pdObject.init(2, numInputs, 44100, ticksPerBuffer)) {
        OF_EXIT_APP(1);
    }
//    pdObject.addToSearchPath("../../../../rjdj-rjlib/");
//    pdObject.addToSearchPath("../../../../rjdj-rjlib/pd");
//    pdObject.addToSearchPath("../../../../rjdj-rjlib/rj");
    pdObject.subscribe("metroTempo");
    pdObject.addReceiver(*this);   // automatically receives from all subscribed sources
    pdObject.start();
   
    animatedCircle = new animatedFigure(0.5,0.5);
    animatedTriangle = new animatedFigure(0.5,0.1);
    animatedRect = new animatedFigure(0.9,0.1);
    ofxKeyframeAnimRegisterEvents(this);
    
    ofAddListener(openNIDevice.handEvent, this, &ofApp::handEvent);
    ofAddListener(GestureEvent::gestureDetected, this, &ofApp::gestureDetected);

    ejercicioActual = EXC_NONE;
    estadoActual = IDLE;
    
    setGUI();
    setGUI1();
}

/*************************************************************************/
void ofApp::update(){
    openNIDevice.update();
    timeEvents.update();
    animatedCircle->playlist.update();
    animatedTriangle->playlist.update();
    animatedRect->playlist.update();
    
    for (int i = 0; i < gesturedHands.size(); i++){
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
    for (int i = 0; i < gesturedHands.size(); i++){
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
    ofDrawBitmapStringHighlight( " FPS: " + ofToString(ofGetFrameRate()),0,40);
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
void ofApp::startOpenNI(int numHands){
    openNIDevice.setup(false);
    openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    // setup the hand generator
    openNIDevice.addHandsGenerator();
    openNIDevice.addHandFocusGesture("Wave");
    openNIDevice.setMaxNumHands(numHands);
    openNIDevice.start();
}

/*************************************************************************/
/*                          EVENTOS                                      */
/*************************************************************************/
void ofApp::gestureDetected(GestureEvent &e){
    ofLogNotice() << "Evento de deteccion de un " << e.message << " como gesto";
    if (e.message=="circulo" && estadoActual==CIRCLE){
        estadoActual=C_DETECTED;
        pdObject.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pdObject.sendBang(patch.dollarZeroStr()+"-circleDetected");
        timeEvents.addKeyFrame(Action::pause(4*60)).addKeyFrame(Action::event(this,"GOTO_TRIANGLE"));
    }
    if (e.message=="triangulo" && estadoActual==TRIANGLE){
        estadoActual=T_DETECTED;
        pdObject.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pdObject.sendBang(patch.dollarZeroStr()+"-triangleDetected");
        timeEvents.addKeyFrame(Action::pause(4*60)).addKeyFrame(Action::event(this,"GOTO_RECTANGLE"));
    }
    if (e.message=="cuadrado" && estadoActual==RECTANGLE){
        estadoActual=R_DETECTED;
        pdObject.sendBang(patch.dollarZeroStr()+"-stopAll");
        timeEvents.clear();
        pdObject.sendBang(patch.dollarZeroStr()+"-rectangleDetected");
        timeEvents.addKeyFrame(Action::pause(3*60)).addKeyFrame(Action::event(this,"GOTO_WELCOME_3"));
    }
    if (e.message=="PercHitL" && estadoActual==IDLE){
        pdObject.sendBang(patch.dollarZeroStr()+"-hit");
    }
    if (e.message=="PercHitL" && (estadoActual==EXC1_1 || estadoActual==EXC1_2)){
        pdObject.sendSymbol(patch.dollarZeroStr()+"-perc", "kick");
    }
    if (e.message=="PercHitR" && (estadoActual==EXC1_1 || estadoActual==EXC1_2)){
        pdObject.sendSymbol(patch.dollarZeroStr()+"-perc", "snare");
    }
}

//*************************************************************************/
void ofApp::handEvent(ofxOpenNIHandEvent & event){
    if (event.handStatus!=HAND_TRACKING_UPDATED){
        ofLogNotice() << getHandStatusAsString(event.handStatus) << "for hand" << event.id << "from device" << event.deviceID;
    }
    if(event.handStatus==HAND_TRACKING_STARTED){
        switch (ejercicioActual) {
            case EXC_ONE:
                if (gesturedHands.size()==0) gesturedHands.push_back(new trackedHand(event.id,1));
                else{
                    trackedHand* trackedhand = gesturedHands[0];
                    if(trackedhand->handType == R_PERCUSSION){
                        gesturedHands.push_back(new trackedHand(event.id,2));
                    }else{
                        gesturedHands.push_back(new trackedHand(event.id,1));
                    }
                }
                break;
                
            case EXC_TWO:
                gesturedHands.push_back(new trackedHand(event.id,ejercicioActual));
                timeEvents.addKeyFrame(Action::event(this,"GOTO_WELCOME_2"));
                break;
                
            case EXC_THREE:
                gesturedHands.push_back(new trackedHand(event.id,ejercicioActual));
                timeEvents.addKeyFrame(Action::event(this,"GOTO_WELCOME_3"));
                break;
                
            default:
                break;
        }

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
    pdObject.audioIn(input, bufferSize, nChannels);
}

//*************************************************************************/
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
    pdObject.audioOut(output, bufferSize, nChannels);
}

//*************************************************************************/
void ofApp::onKeyframe(ofxPlaylistEventArgs& args){
    ofLogNotice() << "Keyframe Event received: " << args.message;
    if (args.message=="GOTO_WELCOME_3"){
        estadoActual=WELCOME;
        pdObject.sendBang(patch.dollarZeroStr()+"-welcome");
        timeEvents.addKeyFrame(Action::pause(9*58)).addKeyFrame(Action::event(this,"GOTO_CIRCLE"));
    }
    
    else if (args.message=="GOTO_WELCOME_2"){
        estadoActual=IDLE;
        pdObject.sendBang(patch.dollarZeroStr()+"-restart");
        pdObject.sendBang(patch.dollarZeroStr()+"-runVel1");
        timeEvents.addKeyFrame(Action::pause(51*60)).addKeyFrame(Action::event(this,"GOTO_WELCOME_2"));
    }
    
    else if(args.message=="GOTO_CIRCLE"){
        estadoActual=CIRCLE;
        pdObject.sendBang(patch.dollarZeroStr()+"-circle");
        animatedCircle->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_CIRCLE"));
    }
    else if(args.message=="GOTO_TRIANGLE"){
        estadoActual=TRIANGLE;
        pdObject.sendBang(patch.dollarZeroStr()+"-triangle");
        animatedTriangle->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_TRIANGLE"));
    }
    else if(args.message=="GOTO_RECTANGLE"){
        estadoActual=RECTANGLE;
        pdObject.sendBang(patch.dollarZeroStr()+"-rectangle");
        animatedRect->clearTrail();
        runAnimPlaylist();
        timeEvents.addKeyFrame(Action::pause(8*60)).addKeyFrame(Action::event(this,"GOTO_RECTANGLE"));
    }
    
}

//*************************************************************************/
void ofApp::exit(){
    gui->saveSettings("settings.xml");
    delete gui;
}

//*************************************************************************/
void ofApp::guiEvent(ofxUIEventArgs &e){
    
    if(e.widget->getKind() == OFX_UI_WIDGET_IMAGEBUTTON){
        ofxUIImageButton *button = (ofxUIImageButton *) e.widget;
        
        if (e.getName()=="BOTON_EXC1" && button->getValue()==1){
            startOpenNI(2);
            gui1->enable();
            patch = pdObject.openPatch("VanillaSamplers/Exc1/Ejercicio1.pd");
            pdObject.sendFloat(patch.dollarZeroStr()+"-metroTempo", 60);
            ejercicioActual = EXC_ONE;
        }
        else if (e.getName()=="BOTON_EXC2" && button->getValue()==1){
            startOpenNI(1);
            patch = pdObject.openPatch("VanillaSamplers/Exc2/Ejercicio2.pd");
            ejercicioActual = EXC_TWO;
        }
        else if (e.getName()=="BOTON_EXC3" && button->getValue()==1){
            startOpenNI(1);
            patch = pdObject.openPatch("VanillaSamplers/Exc3/Ejercicio3.pd");
            ejercicioActual = EXC_THREE;
        }
        else if (e.getName()=="BOTON_Q" && button->getValue()==1){
            openNIDevice.stop();
            pdObject.closePatch(patch);
            timeEvents.clear();
            gui1->disable();
            estadoActual = IDLE;
            ejercicioActual = EXC_NONE;
        }
    }
    if (e.getName()=="EXC1_LEVEL_OP") {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        if (EXC1_LEVEL_OP==1 && (radio->getValue()+1) !=1 ) {
            pdObject.sendBang(patch.dollarZeroStr()+"-mOn");
            ofxUIToggle *toggle = (ofxUIToggle *) gui1->getWidget("ON/OFF METRO");
            toggle->setValue(0);
        }else if(EXC1_LEVEL_OP==2 && (radio->getValue()+1) !=2 ){
            pdObject.sendBang(patch.dollarZeroStr()+"-stopAll");
        }
        EXC1_LEVEL_OP = radio->getValue()+ 1;
        if (EXC1_LEVEL_OP==1) {
            estadoActual = EXC1_1;
        }else if (EXC1_LEVEL_OP==2) {
            estadoActual = EXC1_2;
        }
    }
    
    if(e.getName()=="ON/OFF METRO"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if (EXC1_LEVEL_OP==1) {
            pdObject.sendBang(patch.dollarZeroStr()+"-mOn");
        }
    }
    if(e.getName() == "BPM"){
        if (EXC1_LEVEL_OP==1) {
            ofxUISlider *slider = (ofxUISlider *) e.widget;
            pdObject.sendFloat(patch.dollarZeroStr()+"-metroTempo", slider->getScaledValue());
        }
    }

    if(e.getName()=="STOP SONG"){
        ofxUIButton *boton = (ofxUIButton *) e.widget;
        if (EXC1_LEVEL_OP==2 && boton->getValue()==1) {
            pdObject.sendBang(patch.dollarZeroStr()+"-stopAll");
        }
    }
    if (e.getName()=="EXC1_VEL_OP") {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        string st = "PlayVel" + ofToString(radio->getValue()+ 1);
        pdObject.sendSymbol(patch.dollarZeroStr()+"-playSong", st);
    }
}

//*************************************************************************/
void ofApp::setGUI(){
    gui = new ofxUISuperCanvas("CHOOSE AN APP");

    gui->addSpacer();
    gui->setGlobalButtonDimension(175);
    gui->addImageButton("BOTON_EXC1", "GUI/images/App.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addImageButton("BOTON_EXC2", "GUI/images/App.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addImageButton("BOTON_EXC3", "GUI/images/App.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addImageButton("BOTON_Q", "GUI/images/Preview.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->setGlobalButtonDimension(8);
    gui->setPosition(0, 0);
    gui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
}

//*************************************************************************/
void ofApp::setGUI1(){
    gui1 = new ofxUISuperCanvas("EJERCICIO 1");
    gui1->disable();
    
    gui1->addSpacer();
    gui1->addLabel("NIVEL DIFICULTAD ");
    vector<string> hnames; hnames.push_back("LEVEL1"); hnames.push_back("LEVEL2"); hnames.push_back("LEVEL3");
    gui1->addRadio("EXC1_LEVEL_OP", hnames, OFX_UI_ORIENTATION_HORIZONTAL);
    
    gui1->addSpacer();
    gui1->addLabel("NIVEL DIFICULTAD 1");
    gui1->addToggle("ON/OFF METRO", false);
    gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui1->addSlider("BPM", 50, 120, 60);
    
    gui1->addSpacer();
    gui1->addLabel("NIVEL DIFICULTAD 2");
    gui1->addButton("STOP SONG", false);
    gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    vector<string> velnames; velnames.push_back("PLAY VEL 1"); velnames.push_back("PLAY VEL 2");
    velnames.push_back("PLAY VEL 3");velnames.push_back("PLAY VEL 4"); velnames.push_back("PLAY VEL 5");
    gui1->addRadio("EXC1_VEL_OP", velnames, OFX_UI_ORIENTATION_VERTICAL);
    
    gui1->setGlobalButtonDimension(8);
    gui1->setPosition(212, 0);
    gui1->autoSizeToFitWidgets();
    
    ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
}

//*************************************************************************/
void ofApp::receiveFloat(const std::string& dest, float value) {
    ofLogNotice() << "OF: float " << dest << ": " << value;
}

//*************************************************************************/
void ofApp::receiveBang(const std::string& dest) {
    ofLogNotice() << "OF: bang " << dest;
}

//*************************************************************************/
void ofApp::print(const std::string& message) {
    cout << message << endl;
}


