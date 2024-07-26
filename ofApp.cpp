#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableAlphaBlending();
    ofEnableDepthTest();
    ofDisableArbTex();
   

    light.enable();
    light.setPosition(100, 100, 200);
    light.lookAt(ofVec3f(0, 0, 0));

    cam.setDistance(5000);

    ofLoadImage(texture, "8k_earth_daymap.jpg");
    sphereResolution = sphere.getResolution();
    sphere.setRadius(637.0); // Set the radius of the Earth



    // Initialize satellites data
    int numSatellites = 100; // Number of satellites
    for (int i = 0; i < numSatellites; ++i) {
        SatelliteData satellite;
        satellite.latitude = ofRandom(-90, 90); // Random latitude
        satellite.longitude = ofRandom(0, 360); // Initial longitude
        satellite.altitude = 100 + ofRandom(-50, 50); // Random altitude around 300 km

        satellites.push_back(satellite);
        satelliteAngles.push_back(ofRandom(0, 360)); // Initial angle for movement
    }

    updateBatchSize = 100; // Update 100 satellites per frame
    currentBatchIndex = 0; // Start with the first batch
    lastUpdateTime = ofGetElapsedTimef(); // Get the current time
    updateInterval = 0.1f; // Update every 0.1 seconds

  
}

//--------------------------------------------------------------
void ofApp::update() {
    ofVec3f cameraPos = cam.getPosition();
    ofVec3f lightPos = light.getPosition();
    ofVec3f delta = cameraPos - lightPos;
    if (delta.length() > 5) {
        light.setPosition(lightPos + delta / 50);
    }

 
    // Check if it's time to update
    float currentTime = ofGetElapsedTimef();
    if (currentTime - lastUpdateTime >= updateInterval) {
        double earthRadius = 637.0; // Approx radius of Earth in km

        // Update a batch of satellites
        for (int i = 0; i < updateBatchSize; ++i) {
            int index = (currentBatchIndex + i) % satellites.size();
            satelliteAngles[index] += 0.1; // Change this value to speed up or slow down the movement
            satellites[index].longitude = satelliteAngles[index];
            double latRad = ofDegToRad(satellites[index].latitude);
            double lonRad = ofDegToRad(satellites[index].longitude);
            double altitude = satellites[index].altitude; // Already in km

            satellites[index].position.x = (earthRadius + altitude) * cos(latRad) * cos(lonRad);
            satellites[index].position.y = (earthRadius + altitude) * cos(latRad) * sin(lonRad);
            satellites[index].position.z = (earthRadius + altitude) * sin(latRad);

            // Print the satellite's position for debugging
            //std::cout << "Satellite " << index + 1 << " position: " << satellites[index].position << std::endl;
        }

        // Update the index for the next batch
        currentBatchIndex = (currentBatchIndex + updateBatchSize) % satellites.size();
        lastUpdateTime = currentTime; // Update the last update time
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    cam.begin();

    // Draw Earth
    ofRotateYDeg(ofGetFrameNum() / 9.0);
    texture.bind();
    sphere.draw();
    texture.unbind();

    // Draw Satellites
    ofSetColor(255, 165, 0); // Orange color for the satellites
    for (const auto& satellite : satellites) {
        ofDrawSphere(satellite.position, 2);  // Draw satellite as a small sphere
    }

    ofSetColor(255); // Reset the color to white

    cam.end();
}

