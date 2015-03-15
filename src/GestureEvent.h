#ifndef _GestureEvent_h
#define _GestureEvent_h

class GestureEvent : public ofEventArgs {
    
public:
    string   message;
    
    GestureEvent() {
    }
    
    static ofEvent <GestureEvent> gestureDetected;
};

#endif
