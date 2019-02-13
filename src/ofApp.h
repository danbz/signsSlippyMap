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

#pragma once


#include "ofMain.h"
#include "ofxMaps.h"

#include "ofxExif.h"
#include "ofxGui.h"
#include "ofxThreadedImageLoader.h"

class sign {
    public :
    
    ofImage image;
    easyexif::EXIFInfo exifData;
    
    sign(); // sign constructor
    
    ~sign(); //sign destructor
    
    bool isLandscape(); // getter and setter functions
    float getLat();
    float getLong();
    float getDate();
    string getTime();
    void draw(int x, int y, int z, int scale);
    void load(string filePath);
    string getCountry();
    
    private :
    
};

class ofApp: public ofBaseApp
{
public:
    void setup() override;
    void update() override;
    void draw() override;

    void keyPressed(int key) override;

    std::shared_ptr<ofxMaps::MBTilesCache> bufferCache;
    std::shared_ptr<ofxMaps::MapTileLayer> tileLayer;
    std::shared_ptr<ofxMaps::MapTileSet> tileSet;
    std::shared_ptr<ofxMaps::MapTileProvider> tileProvider;

    std::vector<ofxGeo::Coordinate> coordinates;

    std::vector<std::string> sets;
    int setsIndex = 0;

    float animation = 0;

    ofEasyCam cam;
    
    void processOpenFileSelection(ofFileDialogResult openFileResult);
    void sortSigns();
    static  bool sortDescending(const sign &i, const sign &j);
    
    vector<sign> signsOfSurveillance;
    float signScale, signDist;
    
};
