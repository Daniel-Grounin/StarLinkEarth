#include "ofApp.h"
#define EARTH_RADIUS 6371.0
//--------------------------------------------------------------
void ofApp::setup() {
    ofDisableAlphaBlending();
    ofEnableDepthTest();
    ofDisableArbTex();

    light.enable();
    light.setPosition(100, 100, 200);
    light.lookAt(ofVec3f(0, 0, 0));

    ofLoadImage(texture, "8k_earth_daymap.jpg");
    backgroundImage.load("space.jpg");


    sphereResolution = sphere.getResolution();
    sphere.setRadius(EARTH_RADIUS); // Set the radius of the Earth

    cam.setTarget(sphere);

    cam.setNearClip(0.0);
    cam.setFarClip(100000);
    cam.setDistance(EARTH_RADIUS * 2);


    // Initialize satellites data as empty
    satellites.clear();
    satelliteAngles.clear();

    // Set up update parameters
    updateBatchSize = 100; // Update 100 satellites per frame
    currentBatchIndex = 0; // Start with the first batch
    lastUpdateTime = ofGetElapsedTimef(); // Get the current time
    updateInterval = 1.0f; // Update every 1 seconds

    // Start the socket communication in a new thread
    socketThread = std::thread(&ofApp::run, this);
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
        {
            std::lock_guard<std::mutex> guard(satelliteMutex);  // Lock the mutex
            int batchSize = 100; // Update 100 satellites per frame
            for (int i = 0; i < batchSize && currentBatchIndex < satellites.size(); ++i, ++currentBatchIndex) {
                auto& satellite = satellites[currentBatchIndex];
                // Use already received Cartesian coordinates
                cout << "from update: " << satellite.name << " " << satellite.position.x << " " << satellite.position.y << " " << satellite.position.z << endl;
            }
            if (currentBatchIndex >= satellites.size()) {
                currentBatchIndex = 0; // Reset batch index
            }
        }
        lastUpdateTime = currentTime; // Update the last update time
    }
}




//--------------------------------------------------------------
void ofApp::draw() {
    ofDisableDepthTest();
    backgroundImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    ofEnableDepthTest();

    cam.begin();

    // Draw Earth
    ofRotateYDeg(ofGetFrameNum() / 9.0);
    texture.bind();
    sphere.draw();
    texture.unbind();

    // Draw Satellites
    std::lock_guard<std::mutex> guard(satelliteMutex);  // Lock the mutex
    ofSetColor(255, 165, 0); // Orange color for the satellites
    for (const auto& satellite : satellites) {
        ofDrawSphere(satellite.position, 50);  // Draw satellite as a small sphere
    }

    ofSetColor(255); // Reset the color to white

    cam.end();
}




void ofApp::run() {
    initializeWinsock();

    std::string server_ip = "127.0.0.1";
    int port = 65432;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string port_str = std::to_string(port);
    int result_int = getaddrinfo(server_ip.c_str(), port_str.c_str(), &hints, &result);
    if (result_int != 0) {
        std::cerr << "getaddrinfo failed: " << result_int << std::endl;
        WSACleanup();
        return;
    }

    SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    result_int = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (result_int == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return;
    }

    std::cout << "Connected to the server at " << server_ip << ":" << port << std::endl;

    while (true) {
        std::string response = receiveData(ConnectSocket);

        if (!response.empty()) {
            // Debugging output
            std::cout << "Received JSON: " << response << std::endl;

            Json::CharReaderBuilder readerBuilder;
            Json::Value root;
            std::string errs;
            std::istringstream s(response);
            bool parsingSuccessful = Json::parseFromStream(readerBuilder, s, &root, &errs);
            if (!parsingSuccessful) {
                std::cerr << "Failed to parse the response: " << errs << std::endl;
            }
            else {
                try {
                    if (root.isArray()) {
                        std::lock_guard<std::mutex> guard(satelliteMutex);  // Lock the mutex
                        for (const auto& sat : root) {
                            if (sat.isMember("name") && sat.isMember("x") && sat.isMember("y") && sat.isMember("z")) {
                                SatelliteData satellite;
                                satellite.name = sat["name"].asString();  // Set the name
                                satellite.position.x = sat["x"].asDouble();
                                satellite.position.y = sat["y"].asDouble();
                                satellite.position.z = sat["z"].asDouble();
                                satellites.push_back(satellite);
                            }
                        }
                    }
                    else {
                        std::cerr << "Expected an array of satellites" << std::endl;
                    }
                }
                catch (const std::exception& ex) {
                    std::cerr << "Exception occurred: " << ex.what() << std::endl;
                }
            }
        }

        Sleep(1000); // Adjust sleep time as needed
    }

    closesocket(ConnectSocket);
    cleanupWinsock();
}




void ofApp::cleanupWinsock()
{
    WSACleanup();
}

void ofApp::initializeWinsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        exit();
    }
}

std::string ofApp::receiveData(SOCKET ConnectSocket) {
    char recvbuf[512];
    std::string data;
    int recvbuflen = 512;
    int recvResult;

    do {
        recvResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (recvResult > 0) {
            data.append(recvbuf, recvResult);
        }
        else if (recvResult == 0) {
            std::cout << "Connection closed" << std::endl;
        }
        else {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
            return "";
        }
    } while (recvResult == recvbuflen);  // Continue reading until the buffer is not full

    return data;
}

//--------------------------------------------------------------
void ofApp::exit() {
    if (socketThread.joinable()) {
        socketThread.join();
    }
    cleanupWinsock();
}
