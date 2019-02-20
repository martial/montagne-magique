//
//  BigBang.cpp
//  MontagneMagique
//
//  Created by Martial Geoffre-Rouland on 18/02/2019.
//

#include "BigBang.hpp"
#include "ofApp.h"

void BigBang::setup(string dataPath) {
    
    radius          = 10.0;
    threshold       = 80;
    repulsionScale  = 0.0;
    maxParticleLife = 900;
    
    mode            = 2;
    
    lineDistance    = 200;
    blobDistance    = 350;
    
    forceRandomNessScale = 1.0;
    
}


void BigBang::update() {
    
    ofApp * app = (ofApp*) ofGetAppPtr();

    // we allocate openCV images if needed
    if(!colorImg.bAllocated) {
    
        colorImg.allocate(app->videoInputWidth,app->videoInputHeight);
        grayImage.allocate(app->videoInputWidth,app->videoInputHeight);
        grayBg.allocate(app->videoInputWidth,app->videoInputHeight);
        grayDiff.allocate(app->videoInputWidth,app->videoInputHeight);
        
    }
    
    // we do the countour finding
    colorImg.setFromPixels(app->trackedVideoInput->getPixels());
    grayImage = colorImg;
    grayImage.threshold(100);
    contourFinder.findContours(grayImage, 20, (app->videoInputWidth*app->videoInputHeight), 10, false);    // find holes
    
   
    // delete old particles
    for(int i = particles.size() - 1; i >= 0; i --) {
        
        particles[i]->life += 1;
        if(particles[i]->life > maxParticleLife) {
            particles[i] = NULL;
            particles.erase(particles.begin() + i);
        }
        
    }
    
    // we set behaviour for different modes
    
    if(mode == 0) {
        
        maxParticleLife = 900;
        forceRandomNessScale = 1.0;
        repulsionScale -= .1;
        repulsionScale = ofClamp(repulsionScale, 0.0, 10.0);
        
    }
    
    if(mode == 1) {
        
        maxParticleLife = 900;
        forceRandomNessScale = 1.0;
        
        repulsionScale -= .025;
        repulsionScale = ofClamp(repulsionScale, 0.0, 10.0);
        
        if(ofGetFrameNum() % 8 == 0)
            addParticles(2);
        
    }
    
    if(mode == 2) {
        
        maxParticleLife = 5000;
        forceRandomNessScale = 0.01;
        repulsionScale = 0.0;
        if(ofGetFrameNum() % 24 == 0 && particles.size() < 30)
            addParticles(2);
        
    }
    
}

void BigBang::draw() {
    
     ofApp * app = (ofApp*) ofGetAppPtr();
    
    
    
    for(int i = 0; i < particles.size(); i ++) {
        
        // reset forces
        particles[i]->resetForce();
        
        // add some randomness force
        float xFrc = ofRandom(-1.5 * forceRandomNessScale, 1.5 * forceRandomNessScale);
        float yFrc = ofRandom(-1.5 * forceRandomNessScale, 1.5 * forceRandomNessScale);
        particles[i]->addForce(xFrc, yFrc);
        
        // if we are in mode 0 and 1, add attraction and repulsion forces to the blobs
        if( mode != 2) {
            
            for (int j = 0; j < contourFinder.nBlobs; j++){
                
                if(repulsionScale > 0.0) {
                    particles[i]->addRepulsionForce(contourFinder.blobs[j].boundingRect.getCenter(), ofGetWidth() * 5, repulsionScale);
                }
                particles[i]->addAttractionForce(contourFinder.blobs[j].boundingRect.getCenter(),ofGetWidth() * 5, 0.8);
               
            }
        }
        
        // add damping
        particles[i]->addDampingForce(app->bigBangScaleDampingScale);
        particles[i]->update();
        
        // add ease in / out transition
        float alpha = 1.0;
        if(particles[i]->life < 100) {
            alpha = ofMap(particles[i]->life, 0, 100, 0.0, 1.0);
        }
        
        if(particles[i]->life > maxParticleLife - 100) {
            alpha = ofMap(particles[i]->life, maxParticleLife - 100, maxParticleLife, 1.0, 0.0);
        }
        
        
        // draw
        ofSetColor(255, 255 * alpha);
        particles[i]->begin();
        ofDrawEllipse(0.0, 0.0, particles[i]->scale, particles[i]->scale);
        particles[i]->end();
        
        // we draw lines in mode 1
        
        if( mode == 1 ) {
            
            // if there is no blob, we don't have any reason to draw lines
            if(contourFinder.nBlobs > 0 ) {
                
                float minDistFromBlobs = particles[i]->pos.distance(contourFinder.blobs[0].boundingRect.getCenter());
                for (int j = 1; j < contourFinder.nBlobs; j++){
                    minDistFromBlobs = MIN( minDistFromBlobs,particles[i]->pos.distance(contourFinder.blobs[j].boundingRect.getCenter()) );
                    
                }
            
                // we draw lines only if we have a minimum distance away from a blob
                if( minDistFromBlobs > blobDistance) {
                
                     for(int j = 0; j < particles.size(); j ++) {
                         
                         if ( i != j  ) {
                           
                             float dist = particles[i]->pos.distance(particles[j]->pos);
                             if ( dist < lineDistance) {
                                 
                                 alpha = ofMap(dist, 0, lineDistance, 255, 0);
                                 ofSetColor(255,  alpha);
                                 ofDrawLine(particles[i]->pos, particles[j]->pos);
                             }
                             
                         }
                         
                     }
                    
                }
                
            }
            
        }
      
    }

}

void BigBang::addParticles(int nParticles) {
    
    ofApp * app = (ofApp*) ofGetAppPtr();
    
    vector<ofVec2f> positions;
    
    // we get positions from the borders
    if(mode == 0 ) {
        
            for(int i=0; i<ofGetWidth(); i++) {
            
            ofVec2f pos(i, 0.0);
            positions.push_back(pos);
            
            ofVec2f posy(i, ofGetHeight());
            positions.push_back(posy);
            
        }
        
        for(int i=0; i<ofGetHeight(); i++) {
            
            ofVec2f pos(0.0, i);
            positions.push_back(pos);
            
            ofVec2f posy(ofGetWidth(), i);
            positions.push_back(posy);
            
        }
        
    }
    // we get positions from the blobs
    else if ( mode == 1 ) {
        
        
         for (int j = 0; j < contourFinder.nBlobs; j++){
             positions.push_back(contourFinder.blobs[j].boundingRect.getCenter());
         }
        
        
    }
     // we get positions randomly
    else if ( mode == 2 ) {
        
        for (int j = 0; j < 100; j++){
            positions.push_back(ofVec2f(ofRandom(ofGetWidth()),ofRandom(ofGetHeight())));
        }
        
    }
    
    // if there is no position available, skip !
    if(positions.size() > 0 ) {

        for(int i=0; i<nParticles; i++) {
            
            SimpleParticle  * p = new SimpleParticle();
            int rdm = floor(ofRandom(positions.size()));
            ofVec2f pos = positions[rdm];
            p->setInitialCondition(pos.x,pos.y,0.0,0.0);
            p->scale        = ofRandom(app->bigBangScaleMin, app->bigBangScaleMax);
            p->damping      = ofRandom(app->bigBangDampingMin, app->bigBangDampingMax);
            p->life         = 0.0;
            particles.push_back(p);
                
            
            
        }
        
    }
    
    positions.clear();
    
}



void BigBang::startRepulsion(float scale) {
    ofApp * app = (ofApp*) ofGetAppPtr();
    app->bigBangRepulsionFactor = scale;
    repulsionScale = app->bigBangRepulsionFactor;
    
}
