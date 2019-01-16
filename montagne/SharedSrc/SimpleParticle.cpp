//
//  SimpleParticle.cpp
//  MontagneMagique
//
//

#include "SimpleParticle.hpp"
#include "ofMain.h"

//------------------------------------------------------------
SimpleParticle::SimpleParticle(){
    setInitialCondition(0,0,0,0);
    damping             = 0.01f;
    scale               = 1.0f;
    rotation            = 0.0f;
    smoothedRotation    = 0.0f;
}

//------------------------------------------------------------
void SimpleParticle::resetForce(){
    // we reset the forces every frame
    frc.set(0,0);
}

ofVec2f SimpleParticle::getRandomForceLessThan(float val, float mult) {
    
    float x = ofRandom(-1, 1) * mult;
    float y = ofRandom(-1, 1) * mult;
    
    while( abs(x-frc.x) > val )
        x = ofRandom(-1, 1) * mult;
    
    while( abs(y-frc.y) > val )
        y = ofRandom(-1, 1) * mult;
    
    return ofVec2f(x,y);
    
}


//------------------------------------------------------------
void SimpleParticle::addForce(float x, float y){
    // add in a force in X and Y for this frame.
    frc.x = frc.x + x;
    frc.y = frc.y + y;
}

void SimpleParticle::addNonBackingForce(float mult) {
    
    // adjust just one
    if( (frc.x > 0 && frc.y > 0) || (frc.x < 0 && frc.y < 0) ) {
        
        return;
        
    } else {
        float rx = ofRandom(-1, 1) * mult;
        float ry = ofRandom(-1, 1) * mult;

        addForce(rx, ry);
    }
    
}


//------------------------------------------------------------
void SimpleParticle::addDampingForce(){
    
    // the usual way to write this is  vel *= 0.99
    // basically, subtract some part of the velocity
    // damping is a force operating in the oposite direction of the
    // velocity vector
    
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
}

//------------------------------------------------------------
void SimpleParticle::setInitialCondition(float px, float py, float vx, float vy){
    pos.set(px,py);
    vel.set(vx,vy);
}

//------------------------------------------------------------
void SimpleParticle::update(){
    
    vel = vel + frc;
    
    ofVec2f newPos      = pos + vel;
    ofVec2f diffPos     = newPos - pos;
    rotation            = ofRadToDeg(atan2(diffPos.y, diffPos.x));
    
    float blurRate      = 0.999;
    smoothedRotation    = blurRate * smoothedRotation   +  (1.0f - blurRate) * rotation;
    pos                 = newPos;
    
}

//------------------------------------------------------------
void SimpleParticle::bounceOffWalls(){
    
    // sometimes it makes sense to damped, when we hit
    bool bDampedOnCollision = true;
    bool bDidICollide = false;
    
    // what are the walls
    float minx = 0;
    float miny = 0;
    float maxx = ofGetWidth();
    float maxy = ofGetHeight();
    
    if (pos.x > maxx){
        pos.x = maxx; // move to the edge, (important!)
        vel.x *= -1;
        bDidICollide = true;
    } else if (pos.x < minx){
        pos.x = minx; // move to the edge, (important!)
        vel.x *= -1;
        bDidICollide = true;
    }
    
    if (pos.y > maxy){
        pos.y = maxy; // move to the edge, (important!)
        vel.y *= -1;
        bDidICollide = true;
    } else if (pos.y < miny){
        pos.y = miny; // move to the edge, (important!)
        vel.y *= -1;
        bDidICollide = true;
    }
    
    if (bDidICollide == true && bDampedOnCollision == true){
        vel *= 0.3;
    }
    
}


void SimpleParticle::begin(float xOffSet, float yOffSet) {

    ofPushMatrix();
    ofTranslate(pos.x + xOffSet, pos.y + yOffSet);
    ofScale(scale,scale);
    ofRotateDeg(90 + smoothedRotation);
    ofTranslate(-xOffSet, -yOffSet);

}

void SimpleParticle::end() {
    ofPopMatrix();
}

//------------------------------------------------------------
void SimpleParticle::draw(){
        
    ofSetRectMode(OF_RECTMODE_CENTER);
    begin();
    img->draw(0,0);
    end();
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    
}