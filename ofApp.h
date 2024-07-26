#pragma once
#include "ofMain.h"
#include <vector>

struct SatelliteData {
    double latitude;
    double longitude;
    double altitude;
    ofVec3f position;
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    ofSpherePrimitive sphere;
    ofLight light;
    ofTexture texture;
    ofEasyCam cam;
    int sphereResolution;

    std::vector<SatelliteData> satellites; // Vector of satellites
    std::vector<float> satelliteAngles; // Angles for satellite movement

    int updateBatchSize; // Number of satellites to update per frame
    int currentBatchIndex; // Index to track the current batch
    float lastUpdateTime; // Time of the last update
    float updateInterval; // Interval between updates

    
};
