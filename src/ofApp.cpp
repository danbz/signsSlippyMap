//
// slippy map elements based on example code Copyright (c) 2014 Christopher Baker <https://christopherbaker.net>
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
#include "date.h"
#include <iostream>
#include <string>


using namespace date;
using namespace std::chrono;
using namespace literals;

void ofApp::setup()
{
    ofJson json = ofLoadJson("provider.json");
    tileProvider = std::make_shared<ofxMaps::MapTileProvider>(ofxMaps::MapTileProvider::fromJSON(json));
    Poco::ThreadPool::defaultPool().addCapacity(64);
    bufferCache = std::make_shared<ofxMaps::MBTilesCache>(*tileProvider, "cache/");
    tileSet = std::make_shared<ofxMaps::MapTileSet>(1024, tileProvider, bufferCache);
    tileLayer = std::make_shared<ofxMaps::MapTileLayer>(tileSet, 1 * 1920, 1 * 1080);
    
    ofxGeo::Coordinate chicago(41.8827, -87.6233);
    ofxGeo::Coordinate bethel(45.0579, -93.1605);
    ofxGeo::Coordinate amsterdam(52.370216, 4.895168);
    ofxGeo::Coordinate bristol(51.454514, -2.587910);
    
    tileLayer->setCenter(bristol, 15);
    
    signScale = signDist = 100.0;
    b_showGui = true;
    
    // cam.setGlobalPosition(- ofGetWidth()/2.0, -ofGetHeight()/2.0, 100);
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
    ofEnableDepthTest();
    //cam.begin();
    ofPushMatrix();
    //ofTranslate(-tileLayer->getWidth() / 2, -tileLayer->getHeight() / 2);
    tileLayer->draw(0, 0);
    ofPopMatrix();
    datePath.clear();
    // draw city name in place
    //   // ofxGeo::Coordinate chicago(41.8827, -87.6233);
    //    auto chi = tileLayer->geoToPixels(chicago);
    //    ofDrawBitmapString("chicago", chi.x, chi.y);
    
    // draw signs in place
    for(unsigned int i = 0; i < signsOfSurveillance.size(); i++){
        ofxGeo::Coordinate signGeo(signsOfSurveillance[i].getLat(), signsOfSurveillance[i].getLong());
        auto  signPos = tileLayer->geoToPixels(signGeo);
        signsOfSurveillance[i].draw(signPos.x, signPos.y, 1, signScale);
        ofVec2f newPoint(signPos.x, signPos.y);
        
        datePath.push_back(newPoint); // add a position of a sign into the vector for drawing a path of positions
    }
    
    // cam.end();
    ofDisableDepthTest();
    
    drawTimePath();
    
    if (b_showGui){
        ofDrawBitmapStringHighlight(ofToString(signsOfSurveillance.size() ) + " signs displayed", 14, ofGetHeight() - 48);
        ofDrawBitmapStringHighlight(tileLayer->getCenter().toString(0), 14, ofGetHeight() - 32);
        ofDrawBitmapStringHighlight("Task Queue:" + ofx::TaskQueue::instance().toString(), 14, ofGetHeight() - 16);
        ofDrawBitmapStringHighlight("Connection Pool: " + bufferCache->toString(), 14, ofGetHeight() - 2);
    }
}

//--------------------------------------------------------------


void ofApp::drawTimePath(){
    ofPushStyle();
    ofSetHexColor(0x2bdbe6);
    ofNoFill();
    ofBeginShape(); // draw a bezier path to follow the positons of signs on the map
    
    for (int i = 0; i < datePath.size(); i++){
        
        ofCurveVertex(datePath[i].x, datePath[i].y);
        
        // ofVertex(datePath[i].x, datePath[i].y,1);
    }
    
    ofEndShape();
    ofPopStyle();
}

//--------------------------------------------------------------

void ofApp::keyPressed(int key)
{
    //     if (key == ' ')
    //        {
    //            setsIndex = (setsIndex + 1) % sets.size();
    //            tileLayer->setSetId(sets[setsIndex]);
    //        }
    
    switch (key) {
        case 'g':
            b_showGui = !b_showGui;
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case '-':
            tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(-1));
            break;
            
        case '=':
            tileLayer->setCenter(tileLayer->getCenter().getZoomedBy(1));
            break;
            
        case 'w':
            tileLayer->setCenter(tileLayer->getCenter().getNeighborUp());
            break;
            
        case 'a':
            tileLayer->setCenter(tileLayer->getCenter().getNeighborLeft());
            break;
            
        case 's':
            tileLayer->setCenter(tileLayer->getCenter().getNeighborDown());
            break;
            
        case 'd':
            tileLayer->setCenter(tileLayer->getCenter().getNeighborRight());
            break;
            
        case '1':
            animation -= 0.01;
            break;
            
        case '2':
            animation += 0.01;
            break;
            
        case '3':
            animation = 0;
            break;
            
        case 'p':
            ofSort(signsOfSurveillance, ofApp::sortOnDate);
            break;
            
        case OF_KEY_UP:
            signScale+=5;
            break;
            
        case OF_KEY_DOWN:
            if (signScale>20){
                signScale-=5;
            }
            break;
            
        default:
            break;
    }
    
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
    setup(); // set up MSAInteractiveObject
    mapDotRadius = 10;
    dropShadowOffset = 4;
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
    // image.resize(image.getWidth()/imageScaleFactor, image.getHeight()/imageScaleFactor);
    exifData =  getEXIF(imagePath);     // load XML from jpg
}

//--------------------------------------------------------------
bool sign::isLandscape(){
    // return bool for image orientation
    return false;
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
string  sign::getDate(){
    return  exifData.DateTime;
}

//--------------------------------------------------------------
string sign::getTime(){
    return exifData.DateTimeOriginal;
};

//--------------------------------------------------------------
string sign::getCountry(){
    // calculate country
    return "null-country";
}

//--------------------------------------------------------------
void sign::draw(int x, int y, int z, int scale ){
    int width, height;
    string signLabel;
    
    if (isMouseOver() ) {
        cout << "mouseover" << this << endl;
        scale *=4;
        //  dropShadowOffset = 10;
        z=2;
    }
    
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
    // ofTranslate(-width/2.0,  -height/2.0, 0);
    
    ofPushStyle();
    ofSetColor(SIGN_SHADOW_COLOR, 100);
    ofFill();
    ofDrawRectangle( dropShadowOffset, dropShadowOffset, width, height); // draw offset drop shadow
    
    if (isMouseOver()){ // draw date label if we are rolling over this image
        ofSetColor(SIGN_LABEL_COLOR, 255);
        signLabel = getDate();
        //signLabel = ofToString( exifData.DateTime ) ;
        //signFont.drawString(signLabel, width/2, height + 10);
        ofDrawBitmapString(signLabel, 0, height +  dropShadowOffset *2);
    }
    
    ofPopStyle();
    ofFill();
    ofDrawCircle(0 - mapDotRadius/2.0, 0 - mapDotRadius/2.0, 0, mapDotRadius);
    //  ofDrawLine(0, 0, 0, -width, -height, 1);
    image.draw( 0, 0, 1, width, height) ;
    
    ofPopMatrix();
    
    ofRect(x, y, width, height); // set up MSAInteractiveObject
    set(x, y, width, height);
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
            //enable MSAInteractiveObjects
            for (int i=0; i<signsOfSurveillance.size();i++){
                signsOfSurveillance[i].enableAppEvents();
                signsOfSurveillance[i].enableMouseEvents();
            }
        }
    }
}

//--------------------------------------------------------------

bool ofApp::sortOnDate(const sign &a, const sign &b) {
    //format is   yyyy:mm:dd hh:mm:ss
    int resultCompare;
    
    //return (int)a.exifData.DateTime > (int)b.exifData.DateTime;
    string d1 = a.exifData.DateTime  ;
    string d2 = b.exifData.DateTime  ;
    if (d1!="" and d2 !=""){
        const string search = ":";
        const string replace = "/";
        ofStringReplace(d1, search, replace);
        ofStringReplace(d2, search, replace);
        
        // return true or false
        
        //    //    int date_cmp(const char *d1, const char *d2)  // date string compeare example
        //            // compare years
        //        resultCompare = std::strncmp(d1 , d2 , 4);
        //            if (resultCompare != 0)
        //                return resultCompare;
        //            // compare months
        //        resultCompare = std::strncmp(d1 + 3, d2 + 3, 2);
        //            if (resultCompare != 0)
        //                return resultCompare;
        //
        //            // compare days
        //        return std::strncmp(d1, d2, 2);
        
        //std::chrono::system_clock::time_point
        char dateOnly[20];
        auto date1 = year( d1.copy(dateOnly,0,9) );
        //auto date1 = year(2019)/01/21;
        auto date2 = year( d2.copy(dateOnly,0,9) );
        //auto date2 = year(2019)/01/21;
        if (date2 > date1){
            // std::cout << "date 1 is earlier "  << '\n';
            return true;
        }
        else { // (date2 < date1)
            //std::cout << "date 2 is earlier "  << '\n';
            return false;
            
        }
    }
    return false;
    //    else
    //        cout << " date 1 and date 2 are the same" << endl;
    // compare time of day
    
}

//
//if(key == '3')     {
//    sortTypeInfo = "sorting on date ";
//    ofSort(signsOfSurveillance, ofApp::sortOnDate);
//}
