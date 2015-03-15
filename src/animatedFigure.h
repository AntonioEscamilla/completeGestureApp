#ifndef __animatedFigure__
#define __animatedFigure__

#include "ofxPlaylist.h"

class animatedFigure{
public:
    animatedFigure(float x,float y);
    ~animatedFigure();
    void draw();
    void clearTrail();
    void onKeyframe(ofxPlaylistEventArgs& args);
    
    float           angulo;
    ofFloatColor    color;
    ofVec3f         pos;
    ofxPlaylist     playlist;
    ofFbo           fboFigureDraw;
    bool            isCircle;
};

#endif
