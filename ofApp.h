#pragma once
#include "ofMain.h"
#include <vector>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <json/json.h>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

struct SatelliteData {
    std::string name;
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
    void exit();

    void run();  // Function to handle socket communication
    void cleanupWinsock();
    void initializeWinsock();
    std::string receiveData(SOCKET ConnectSocket);

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
    ofImage backgroundImage; // Background image

private:
    std::thread socketThread;  // Thread for handling socket communication
    std::mutex satelliteMutex;
};
