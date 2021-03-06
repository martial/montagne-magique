#ifndef MagiqueMarker_hpp
#define MagiqueMarker_hpp

#include "ofMain.h"
#include "ofxARTNftTracker.h"
#include "SimpleKalmanFilter.hpp"

using namespace ofxArtool5;

class MagiqueMarker  : public NftTracker {
    
public:
    
    MagiqueMarker();
    
    void updateTimes();
    
    void beginAR();
    void beginHardAR();
    void endAR();
    
    void updateBlank(); // this used for shutting marker detection
    
    bool hasBeenTracked();
    string markerid;
    
    void setImage(ofImage * image);
    ofImage * image;
    float width, height;
    int videoOutputWidth, videoOutputHeight;
    
    bool getIsSolidFound();
    int timeFoundElapsed, timeLostElapsed;
    int timeSolidFoundElapsed, timeSolidLostElapsed;

    bool bIsFound, bIsSolidFound;
    
    ofEvent<string> solidFoundEvent;
    ofEvent<string> solidLostEvent;
    
    bool bIsActive, bIsAlwaysActive;

    int timeFoundDelay, timeLostDelay;

private:
    
    ARdouble currentPose[16] ;
    bool bTracked;

    int timeFound, timeLost;
    int timeSolidFound, timeSolidLost;
    
    vector<SimpleKalmanFilter> filters;

};


#endif /* MagiqueMarker_hpp */
