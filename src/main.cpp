//
// based on example code Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//
/*
 Project Title: Signs of Surveillance
 Description:
 ©Daniel Buzzo 2019
 dan@buzzo.com
 http://buzzo.com
 https://github.com/danbz
 */


#include "ofAppRunner.h"
#include "ofApp.h"


int main()
{
    ofGLWindowSettings settings;
    settings.setSize(1280, 768);
    settings.setGLVersion(3, 2);
    settings.windowMode = OF_FULLSCREEN;
    auto window = ofCreateWindow(settings);
    auto app = std::make_shared<ofApp>();
    return ofRunApp(app);
}
