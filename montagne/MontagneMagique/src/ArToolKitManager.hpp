//
//  ArToolKitManager.hpp
//  MontagneMagique
//
//  Created by Martial Geoffre-Rouland on 13/12/2018.
//

#ifndef ArToolKitManager_hpp
#define ArToolKitManager_hpp

#include "ofMain.h"
#include "MagiqueMarker.hpp"
class ArToolKitManager {
    
public:
    
    void setup(int width, int height);
    void loadTrackers();
    
    void update(ofBaseHasPixels & input);
    void debugDraw();
    
    void onNewMarker(int & mId);
    void onLostMarker(int & mId);
    
    void clean();
    
    vector<std::shared_ptr<MagiqueMarker>> trackers;
    vector<ofImage> images;

    
private:
    
    int inputWidth, inputHeight;
    
    
    
};

#endif /* ArToolKitManager_hpp */