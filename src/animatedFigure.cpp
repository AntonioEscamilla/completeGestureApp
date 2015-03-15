#include "animatedFigure.h"

/*************************************************************************/
animatedFigure::animatedFigure(float x, float y){
    fboFigureDraw.allocate(400, 400, GL_RGBA32F_ARB);
    fboFigureDraw.begin();
    ofClear(255,255,255, 0);
    fboFigureDraw.end();
    pos=ofVec3f(x ,y ,0);
    color=ofColor(255,255,255);
    angulo = 0.0f;
    if (x==0.5f && y==0.5f) isCircle=true;
    else isCircle=false;
}

/*************************************************************************/
animatedFigure::~animatedFigure(){
    playlist.clear();
};

/*************************************************************************/
void animatedFigure::draw(){
    fboFigureDraw.begin();
    ofFill();  //rectangulo del tamaño de la fbo con grado de transparencia (trail drawing)
    ofSetColor(255,255,255, 1);
    ofRect(0,0,400,400);
    
    ofFill();
    ofSetColor(color);
    
    ofPushMatrix();
    ofTranslate(pos.x*fboFigureDraw.getWidth(),pos.y*fboFigureDraw.getHeight());
    ofRotate(angulo);
    if(isCircle) ofRectRounded(0.35*fboFigureDraw.getWidth(), 0, 0.15*fboFigureDraw.getWidth(), 15, 10);
    else ofCircle(0, 0, 20);
    ofPopMatrix();
    fboFigureDraw.end();
    
    ofSetColor(255, 255, 255);
    fboFigureDraw.draw(pos.x,pos.y,200,200);
}

/*************************************************************************/
void animatedFigure::clearTrail(){
    fboFigureDraw.begin();
    ofClear(255,255,255, 0);
    fboFigureDraw.end();
}

/*************************************************************************/
void animatedFigure::onKeyframe(ofxPlaylistEventArgs& args){
    // this check is only necessary if you want to be absolutely sure that
    // the onKeyFrame Event was sent by the same object as the receiver.
    if (args.pSender != static_cast<void*>(this)) return;
    
    ofLog(OF_LOG_VERBOSE) << "Animated Rectangle: Keyframe Event received for (" << args.pSender << "): " << args.message << ": " << ofGetFrameNum();
}