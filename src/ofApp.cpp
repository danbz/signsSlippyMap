//
// based on example code Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//
/*
 Project Title: Signs of Surveillance
 Description:
 ©Daniel Buzzo 2019
 dan@buzzo.com
 http://buzzo.com
 https://github.com/danbz
 */

#include "ofApp.h"


void ofApp::setup()
{
    coordinates =
    {
        { 42.2610492, -91.4501953 },
        { 43.0046471, -90.4833984 },
        { 43.0367759, -89.3847656 },
        { 41.9676592, -88.4619141 },
        { 41.2117215, -89.0332031 },
        { 40.5805847, -90.1318359 },
        { 40.6806380, -91.1865234 },
        { 41.1124688, -92.4169922 },
        { 42.1959688, -93.2958984 },
        { 43.2932003, -92.1972656 },
        { 44.0560117, -90.7470703 }
    };
    
    ofJson json = ofLoadJson("provider.json");
    
    tileProvider = std::make_shared<ofxMaps::MapTileProvider>(ofxMaps::MapTileProvider::fromJSON(json));
    Poco::ThreadPool::defaultPool().addCapacity(64);
    bufferCache = std::make_shared<ofxMaps::MBTilesCache>(*tileProvider, "cache/");
    tileSet = std::make_shared<ofxMaps::MapTileSet>(1024,
                                                    tileProvider,
                                                    bufferCache);
    
    tileLayer = std::make_shared<ofxMaps::MapTileLayer>(tileSet, 1 * 1920, 1 * 1080);
    
    ofxGeo::Coordinate chicago(41.8827, -87.6233);
    ofxGeo::Coordinate bethel(45.0579, -93.1605);
    
    tileLayer->setCenter(coordinates[3], 21);
    
    signScale = signDist = 60.0;
}

//--------------------------------------------------------------

void ofApp::update()
{
    tileLayer->update();
    
    if (!ofIsFloatEqual(animation, 0.f))
        tileLayer->setCenter(tileLayer->getCenter().getNeighbor(animation, 0));
}

//--------------------------------------------------------------

void ofApp::draw()
{
    //    ofScale(0.25, 0.25, 1);
    //    ofBackgroundGradient(ofColor(255), ofColor(0));
    //    ofFill();
    //    ofSetColor(255);
    
    //    cam.begin();
    ofPushMatrix();
    //ofTranslate(-tileLayer->getWidth() / 2, -tileLayer->getHeight() / 2);
    tileLayer->draw(0, 0);
    ofPopMatrix();
    
    ofPushStyle();
    ofNoFill();
    ofSetColor(0, 255, 0);
    
    for (auto coordinate: coordinates)
    {
        auto tc = tileLayer->geoToPixels(coordinate);
        ofDrawCircle(tc.x, tc.y, 20);
    }
    ofPopStyle();
    
    // draw city name in place
    ofxGeo::Coordinate chicago(41.8827, -87.6233);
    auto chi = tileLayer->geoToPixels(chicago);
    ofDrawBitmapString("chicago", chi.x, chi.y);
    
    // draw signs in place
    for(unsigned int i = 0; i < signsOfSurveillance.size(); i++){
        ofxGeo::Coordinate signGeo(signsOfSurveillance[i].getLat(), signsOfSurveillance[i].getLong());
        auto  signPos = tileLayer->geoToPixels(signGeo);
        signsOfSurveillance[i].draw(signPos.x, signPos.y, 0, signScale);
        
    }
    // end draw city name
    
    //    cam.end();
    
    ofDrawBitmapStringHighlight(tileLayer->getCenter().toString(0), 14, ofGetHeight() - 32);
    ofDrawBitmapStringHighlight("Task Queue:" + ofx::TaskQueue::instance().toString(), 14, ofGetHeight() - 16);
    ofDrawBitmapStringHighlight("Connection Pool: " + bufferCache->toString(), 14, ofGetHeight() - 2);
    
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key)
{
    if (key == 'f' || key == 'F')
    {
        ofToggleFullscreen();
    }
    else if (key == '-')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(-1));
    }
    else if (key == '=')
    {
        tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(1));
    }
    else if (key == 'w')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborUp());
    }
    else if (key == 'a')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborLeft());
    }
    else if (key == 's')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborDown());
    }
    else if (key == 'd')
    {
        tileLayer->setCenter(tileLayer->getCenter().getNeighborRight());
    }
    else if (key == '1')
    {
        animation -= 0.01;;
    }
    else if (key == '2')
    {
        animation += 0.01;;
    }
    else if (key == '3')
    {
        animation = 0;
    }
    //    else if (key == ' ')
    //    {
    //        setsIndex = (setsIndex + 1) % sets.size();
    //        tileLayer->setSetId(sets[setsIndex]);
    //    }
    
    if (key == 'l'){
        ofFileDialogResult openFileResult= ofSystemLoadDialog("Select a folder with jpg image files");
        
        if (openFileResult.bSuccess){
            ofLogVerbose("User selected a folder");
            processOpenFileSelection(openFileResult);
            
        }else {
            ofLogVerbose("User hit cancel");
        }
    }
    
}


//--------------------------------------------------------------
sign::sign(){  // sign constructor
    
}

//--------------------------------------------------------------
sign::~sign(){    // sign destuctor
    
}

//--------------------------------------------------------------
void sign::load(string imagePath){
    // int imageScaleFactor = 10;
    cout << "loading: " << ofToString(imagePath) << endl;
    ofImageLoadSettings myImageLoadSettings;
    myImageLoadSettings.exifRotate = true;
    image.load(imagePath, myImageLoadSettings );
    // input routine here to resize images to target size if not already resized...
    //  image.resize(image.getWidth()/imageScaleFactor, image.getHeight()/imageScaleFactor);
    exifData =  getEXIF(imagePath);     // load XML from jpg
}

//--------------------------------------------------------------
bool sign::isLandscape(){
    // return bool for image orientation
}

//--------------------------------------------------------------
float sign::getLat(){
    return exifData.GeoLocation.Latitude;
}

//--------------------------------------------------------------
float sign::getLong(){
    return  exifData.GeoLocation.Longitude;
}

//--------------------------------------------------------------
float sign::getDate(){
    
}

//--------------------------------------------------------------
string sign::getTime(){
    return exifData.DateTimeOriginal;
};

//--------------------------------------------------------------
string sign::getCountry(){
    // calculate country
}

//--------------------------------------------------------------
void sign::draw(int x, int y, int z, int scale ){
    int width, height;
    string signLabel;
    if (image.getWidth() > image.getHeight() ){ // determine portrait or landscape orientation
        width =  scale;
        height =  scale * 0.75;
    } else  {
        width =  scale * 0.75;
        height =  scale;
    }
    
    // signLabel = ofToString(exifData.GeoLocation.Latitude) + ", " + ofToString(exifData.GeoLocation.Longitude);
    // signLabel += " orientation: " + ofToString(exifData.Orientation);
    //  image.rotate90(1);
    // rotate to orientate as tangent to line to globe centre
    // rotate image perpendicluar to line to centre of globe
    ofVec2f v1(0, -1); // constant for heading angle and vector calculations
    ofVec2f XY(x,z);
    ofPushMatrix();
    
    float mapRotationXY =  v1.angle(XY);
    
    // ofTranslate(x-width/2.0,  y-height/2.0, z);
    ofTranslate(x, y, z);
    // ofRotateYDeg(-mapRotationXY + 180);
    ofTranslate(-width/2.0,  -height/2.0, 0);
    
    image.draw( 0, 0, width, height) ;
    
    // signLabel = ofToString( exifData.DateTime ) ;
    // signFont.drawString(signLabel, width/2, height + 10);
    ofPopMatrix();
}

//--------------------------------------------------------------

void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult){
    
    ofLogVerbose("getName(): "  + openFileResult.getName());
    ofLogVerbose("getPath(): "  + openFileResult.getPath());
    
    ofFile file (openFileResult.getPath());
    ofFilePath dirPath;
    if (file.exists()){
        ofLogVerbose("The file exists - now checking the type via file extension");
        string fileExtension = ofToUpper(file.getExtension());
        if (fileExtension == "JPG") {         // We only want jpg images
            ofDirectory imageDirectory( dirPath.getEnclosingDirectory(file) );
            imageDirectory.listDir();
            imageDirectory.allowExt("jpg");
            imageDirectory.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order
            for (int i=0; i<imageDirectory.size(); i++){
                sign newSign;
                newSign.load(imageDirectory.getPath(i)) ;
                signsOfSurveillance.push_back(newSign);
            }
        }
    }
}

//--------------------------------------------------------------
