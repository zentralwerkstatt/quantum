#pragma once

#include "ofMain.h"


class Anchor {
    
public:
    ofVec3f position;
    string anchorTag;
    int v;
    float a;
    float aSpeed;
    float textWidth;
    bool active;
    
    Anchor(float pX, float pZ, int pV, string pAnchorTag);
    void draw();
    void update();
    
};


class Paper {
    
public:
    string paperTitle;
    string paperAuthorsYearSource;
    string paperAbstract;
    int paperCitations;
    int paperGoogleRank;
    vector<string> paperTags;
    vector<Anchor> paperAnchors;
    ofVec3f position;
    bool active = false;
    float planetSize;
    float a;
    float aSpeed;
    float textWidth;
    
    Paper(string pPaperTitle, string pPaperAuthorsYearSource, string pPaperAbstract, int pPaperCitations, int pPaperGoogleRank, string pPaperTags, ofMesh &pPaperMesh, int &pIndexCount, int &pVertexNumber, ofColor pPaperColor, vector<Anchor> &pAnchors);
    void draw();
    void update();
    
};



class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void mousePressed(int x, int y, int button);
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void loadCSVAsStrings(vector<vector<string> > &pData, string pPath, string pSeparator, string pEscaper);
    string randomString(int pLen);
    string layoutString(string pS, int pLineLength);
    void calcNormals(ofMesh &pMesh, bool pNormalize);
    void resetCam();
    
};
