#include "ArSceneManager.hpp"
#include "VideoScene.hpp"
#include "BigBangScene.hpp"
#include "BirdsScene.hpp"
#include "GenericScene.hpp"
#include "HoleScene.hpp"
#include "DrawScene.hpp"
#include "ofApp.h"

void ArSceneManager::setup(vector<std::shared_ptr<MagiqueMarker>> & trackers) {
    
    bDebugMode = false;

    BigBangScene * bigBangScene     = new BigBangScene();
    bigBangScene->setup("bigbang");
    
    DrawScene * drawScene           = new DrawScene();
    drawScene->setup("draw");
    
    BirdsScene * birdsScene         = new BirdsScene();
    birdsScene->setup("OISEAU_BULLES");
    
    BirdsScene * otherBirdScene     = new BirdsScene();
    otherBirdScene->setup("OISEAU_BULLES2");
    
    BirdsScene * staticBirdScene     = new BirdsScene();
    staticBirdScene->setup("OISEAU_BULLES3");
    
    scenes.push_back(bigBangScene);
    scenes.push_back(drawScene);
    scenes.push_back(birdsScene);
    scenes.push_back(otherBirdScene);
    scenes.push_back(staticBirdScene);

    loadAutoScenes();
    
    for(int i=0; i<trackers.size(); i++) {
        
        int sceneIndex = getSceneIndexForMarkerID(trackers[i]->markerid);
        if(sceneIndex >= 0)
            scenes[sceneIndex]->setMarker(trackers[i]);
        
    }

}

void ArSceneManager::loadAutoScenes() {
    
    string path = "autoscenes";
    ofDirectory dir(path);
    dir.listDir();
    
    //go through and print out all the paths
    for(int i = 0; i < dir.size(); i++){
        
        if( dir.getFile(i).isDirectory()) {
            
            // load json and check class
            string configUrl = dir.getPath(i) + "/config.json";
            ofJson config = ofLoadJson(configUrl);
            
            if(config["class"] == "GenericScene") {
                
                GenericScene * scene = new GenericScene();
                scene->assetsDir = path + "/";
                scene->setup(dir.getName(i));
                scenes.push_back(scene);
                                                
            }
            
        }
        
    }
    
}

void ArSceneManager::updateCamera(int markerIndex, MagiqueMarker & marker,  ofBaseHasPixels & input) {
    
    int sceneIndex = getSceneIndexForMarkerID(marker.markerid);
    
    if(sceneIndex >= 0) {
         scenes[sceneIndex]->updateCamera(input);
    }
    
}

void ArSceneManager::drawScene(int sceneIndex) {
    
    scenes[sceneIndex]->update();
    scenes[sceneIndex]->draw();
    
}

void ArSceneManager::drawOffScreen(int sceneIndex) {
    
    scenes[sceneIndex]->drawOffScreen();

}

void ArSceneManager::draw(int markerIndex, MagiqueMarker & marker) {
    
    int sceneIndex = getSceneIndexForMarkerID(marker.markerid);
    
    if(sceneIndex >= 0) {
        
        scenes[sceneIndex]->update();
        scenes[sceneIndex]->draw();

    }
    
}

void ArSceneManager::activateAllMarkers() {}

void ArSceneManager::activateMarkersFromScene() {
    
    bool unlockAll = true;
    
    ofApp * app = (ofApp*) ofGetAppPtr();
    string currentScene = app->app.currentSceneName + "/" + app->app.currentSubSceneName;

    vector<string> results;
    int nMarkersActivated = 0;
    for(int i=0; i<scenes.size(); i++) {
        
        // if no marker skip obviously
        if(!scenes[i]->marker)
            continue;
        
        if(unlockAll) {
            scenes[i]->marker->bIsActive = true;
            nMarkersActivated++;
            continue;
        }
        
        // bIsAlwaysActive means that It will always be detected
        if(scenes[i]->marker->bIsAlwaysActive) {
            scenes[i]->marker->bIsActive = true;
            results.push_back(scenes[i]->name);
            continue;
        }
        
        // if not, we check if lockedscene is ok with current
        if(scenes[i]->lockedScene == currentScene) {
            scenes[i]->marker->bIsActive = true;
            results.push_back(scenes[i]->name);
            nMarkersActivated++;

        } else {
            scenes[i]->marker->bIsActive = false;
            scenes[i]->marker->updateBlank();

        }
        
    }
    
    app->addMessage("Setting scene : " + currentScene + " with " + ofToString(nMarkersActivated) + " markers activated");
    for(int i=0; i<results.size(); i++) {
        app->addMessage(results[i] + ofToString(" markers activated"));
    }
    
}

AbstractARScene * ArSceneManager::getSceneIndexForIndex(int index, MagiqueMarker & marker) {
    
    int sceneIndex = getSceneIndexForMarkerID(marker.markerid);
    
    if(sceneIndex >= 0) {
        
        return scenes[sceneIndex];
        
    }
    return NULL;
}

int ArSceneManager::getSceneIndexForMarkerID(string markerID) {
    
    for(int i=0; i<scenes.size(); i++) {
        
        if(scenes[i]->markerID == markerID)
            return i;
        
    }
    
    return -1;
    
}

AbstractARScene * ArSceneManager::getSceneIndexForPath(string path) {
    
    for(int i=0; i<scenes.size(); i++) {
        
        if(scenes[i]->name == path)
            return scenes[i];
        
    }
    
    return NULL;
    
}

void ArSceneManager::setGenericSceneLoopMode(ofLoopType loopState) {
    
    for(int i=0; i<scenes.size(); i++) {

        GenericScene * genericScene = dynamic_cast<GenericScene*>(scenes[i]);
        if (genericScene) {
            genericScene->setLoopMode(loopState);
        }
    }
    
}

void ArSceneManager::setDebugMode(bool debugMode) {
    
    for(int i=0; i<scenes.size(); i++) {
        
        scenes[i]->bDebugMarker = debugMode;
          
    }
    this->bDebugMode = debugMode;
}
