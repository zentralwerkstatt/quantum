 #include "ofApp.h"

// openFrameworks bug workaround
int firstRun = 0;

// fonts (relative size)
ofTrueTypeFont font3D;
ofTrueTypeFont fontHud;
string fontFile = "courier_new.ttf";
int fontDpi = 72;
const float fontHUDScale = 0.006;
const int font3DSize = 300; // fixed original size to enable hi-res zoom
const float font3DScalar = 0.05;
const float font3DLetterSpacing = 0.8;

// image (relative size)
ofImage image;
bool imageShow = true;
string imageFile = "amplituhedron.png";
int imageLeftMargin = 50;
int imageRightMargin = 50;
int imageTopMargin = 50;
int imageBottomMargin = 50;
float imageOffset = 2.5; // slightly towards the top

// movement
float rotationMin = 0.1;
float rotationMax = 0.5;

// color pallet
ofColor backgroundColor = ofColor(255,255,245,255);
ofColor raysColor = ofColor(0,0,0,255);
ofColor textColor = ofColor(0,0,0,255);
ofColor gridColor = ofColor::fromHsb(0,150,255,255);
ofColor paperColor = ofColor(0,0,0,255);
ofColor hudTextColor1 = ofColor::fromHsb(0,150,255,255);
ofColor hudTextColor2 = ofColor(20,20,20,255);

// data sculpting (relative)
const float gridHeightFactor = 10;
const int gridDimFactor = 20;
const float googleRankFactor = 2;
const float anchorsFactor = 1.5;

// frame layout (relative size)
const float layoutCommonMarginScale = 0.03;
const float layoutImageMarginScale = 0.04;

// text layout (fixed)
const float layoutUpperHudLineSpacing = 1.1;
const int layoutUpperHudLineWidth = 80;
const int layoutLowerHudLineWidth = 60;
const int layoutTitleHudLineWidth = 55;
const int layoutCreditHudLineWidth = 55;

// globals for HUD info
string hudTextUpper1 = "";
string hudTextUpper2 = "";
string hudTextUpper3 = "";
string hudTextLower = "";
// const string hudTextTitle = "QUANTUM AESTHETICS.\n    INTERDISCIPLINARITY IN GOOGLE SCHOLAR DATA.\n This project examines the intersection of quantum physics and aesthetics through the constant evaluation of papers automatically collected from Google Scholar. The most common keywords from the \"quantum\" papers set the terrain on which the \"aesthetics\" papers are mapped as a cloud of amplitudhedrons. The size of each amplitudhedron is correlated to the number of matching keywords on the terrain, while its height is correlated to the corresponding paper's rank in the Google scholar search.";
const string hudTextTitle = "QUANTUM AESTHETICS.\n    INTERDISCIPLINARITY IN GOOGLE SCHOLAR DATA.\n This project examines the intersection of quantum physics and aesthetics through the constant evaluation of papers automatically collected from Google Scholar. The most common keywords from the \"quantum\" papers set the terrain on which the \"aesthetics\" papers are mapped as a cloud of amplitudhedrons. The size of each amplitudhedron is correlated to the number of matching keywords on the terrain, while its height is correlated to the corresponding paper's rank in the Google scholar search. Click the large amplituedron on the left to switch data sources. ";
// const string hudTextCredit = "";
const string hudTextCredit = "A project by Fabian Offert (fabian@mat.ucsb.edu) for MAT259. Amplituhedron sketch from Arkani-Hameda, Nima and Jaroslav Trnka. The Amplituhedron. JHEP Vol. 10 (2014) No. 30. See also: Offert, Fabian. Conceptual Superposition. The Aesthetics of Quantum Simulation. SIGGRAPH Asia 2015 Art Papers.";

// switch for modes
char mode = 'A';

// globals for data
int anchorMeshDimsA;
int anchorMeshDimsB;
int paperVertexNumberA;
int paperVertexNumberB;
int paperIndexCountA = 0;
int paperIndexCountB = 0;
ofVboMesh paperMeshA;
ofVboMesh paperMeshB;
ofVboMesh anchorMeshA;
ofVboMesh anchorMeshB;
vector<vector<string> > tablePapersA;
vector<vector<string> > tableTagsA;
vector<vector<string> > tablePapersB;
vector<vector<string> > tableTagsB;
int rowsTagsA;
int rowsPapersA;
int rowsTagsB;
int rowsPapersB;
vector<Anchor> anchorsA;
vector<Paper> papersA;
vector<Anchor> anchorsB;
vector<Paper> papersB;

// cam
ofEasyCam cam;

// FBO
ofFbo fbo;
bool fboOn = false;


Anchor::Anchor(float pX, float pZ, int pV, string pAnchorTag) {
    
    position = ofVec3f(pX,pV/gridHeightFactor,pZ);
    anchorTag = pAnchorTag;
    v = pV;
    
    active = false;
    
    a = ofRandom(0,360);
    aSpeed = ofRandom(rotationMin,rotationMax);
    
    textWidth = font3D.stringWidth(anchorTag);
    
}


void Anchor::update() {a += aSpeed;}


void Anchor::draw() {
    
    if (active) {
        ofPushMatrix();
        ofSetColor(textColor);
        ofTranslate(position);
        ofRotateY(a);
        ofScale(font3DScalar,font3DScalar,0);
        font3D.drawString(ofToString(anchorTag),-textWidth/2,0);
        ofPopMatrix();
    }
    
}


Paper::Paper(string pPaperTitle, string pPaperAuthorsYearSource, string pPaperAbstract, int pPaperCitations, int pPaperGoogleRank, string pPaperTags, ofMesh &pPaperMesh, int &pIndexCount, int &pVertexNumber, ofColor pPaperColor, vector<Anchor> &pAnchors) {
    
    paperTitle = pPaperTitle;
    paperAuthorsYearSource = pPaperAuthorsYearSource;
    paperAbstract = pPaperAbstract;
    paperCitations = pPaperCitations;
    paperGoogleRank = pPaperGoogleRank;
    paperTags = ofSplitString(pPaperTags,";");
    
    for (string &t : paperTags) {
        for (Anchor &a : pAnchors) {
            if (t == a.anchorTag) {
                paperAnchors.push_back(a);
                position += a.position;
            }
        }
    }
    
    position /= paperAnchors.size();
    
    // size from adaption to context
    planetSize = float(paperAnchors.size())*anchorsFactor;
    
    // y position from google rank
    position.y = (1000-paperGoogleRank)/googleRankFactor;
    
    ofMesh planetMesh;
    
    planetMesh.addVertex(ofVec3f(0,0,0));
    planetMesh.addVertex(ofVec3f(-0.5,1,0));
    planetMesh.addVertex(ofVec3f(-1,0,0));
    planetMesh.addVertex(ofVec3f(-0.5,-1,0.5));
    planetMesh.addVertex(ofVec3f(0.5,-1,0));
    planetMesh.addVertex(ofVec3f(1,0,0));
    planetMesh.addVertex(ofVec3f(-0.5,0,1));
    
    float planetMeshIndices[27] = {
        1,7,3,
        1,2,7,
        1,2,3,
        1,4,3,
        1,5,6,
        1,7,6,
        1,7,5,
        7,4,3,
        7,4,5
    };
    
    for (int i=0; i<27; i++) {planetMesh.addIndex(ofIndexType(planetMeshIndices[i]-1));}
    
    vector<ofVec3f> vertexVec = planetMesh.getVertices();
    vector<ofIndexType> indexVec = planetMesh.getIndices();
    
    pVertexNumber = vertexVec.size();
    
    for (ofVec3f &v : vertexVec) {pPaperMesh.addVertex(position+(v*planetSize));}
    for (ofIndexType &i : indexVec) {pPaperMesh.addIndex(pIndexCount+i);}
    
    pIndexCount += pVertexNumber;
    
    a = ofRandom(0,360);
    aSpeed = ofRandom(rotationMin,rotationMax);
    textWidth = font3D.stringWidth(ofToString(paperCitations));
    
}


void Paper::update() {a += aSpeed;}


void Paper::draw() {
    
    if (active) {
        
        ofPushMatrix();
        ofSetColor(textColor);
        ofTranslate(position);
        ofRotateY(a);
        ofScale(font3DScalar,font3DScalar,0);
        font3D.drawString(ofToString(paperCitations),-textWidth/2,0);
        ofPopMatrix();
        
        for (Anchor &a : paperAnchors) {
            
            ofPushMatrix();
            ofSetColor(raysColor);
            ofDrawLine(position,a.position);
            ofPopMatrix();
            
        }
        
    }
    
}


void ofApp::setup(){
    
    // allocate fbo
    fbo.allocate(4000,3000,GL_RGBA);
    
    // general options
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
    ofSetSmoothLighting(true);
    ofHideCursor();
    
    // prepare the fonts
    ofTrueTypeFont::setGlobalDpi(fontDpi);
    font3D.setLetterSpacing(font3DLetterSpacing);
    font3D.load(fontFile,font3DSize,true,true,true);
    fontHud.load(fontFile,int(ofGetWidth()*fontHUDScale),true,true,true);
    
    // prepare image
    image.load(imageFile);
    
    // prepare the meshes
    paperMeshA.setMode(OF_PRIMITIVE_TRIANGLES);
    paperMeshA.disableColors();
    paperMeshA.enableIndices();
    paperMeshA.enableNormals();
    
    paperMeshB.setMode(OF_PRIMITIVE_TRIANGLES);
    paperMeshB.disableColors();
    paperMeshB.enableIndices();
    paperMeshB.enableNormals();
    
    anchorMeshA.setMode(OF_PRIMITIVE_TRIANGLES);
    anchorMeshA.disableColors();
    anchorMeshA.enableIndices();
    anchorMeshA.enableNormals();
    
    anchorMeshB.setMode(OF_PRIMITIVE_TRIANGLES);
    anchorMeshB.disableColors();
    anchorMeshB.enableIndices();
    anchorMeshB.enableNormals();
    
    // load data
    loadCSVAsStrings(tableTagsA, "aesthetics_common.csv",",","|");
    loadCSVAsStrings(tablePapersA, "quantum_data.csv",",","|");
    loadCSVAsStrings(tableTagsB, "quantum_common.csv",",","|");
    loadCSVAsStrings(tablePapersB, "aesthetics_data.csv",",","|");
    rowsTagsA = tableTagsA.size();
    rowsPapersA = tablePapersA.size();
    rowsTagsB = tableTagsB.size();
    rowsPapersB = tablePapersB.size();
    
    // shuffle terrain
    ofRandomize(tableTagsA);
    ofRandomize(tableTagsB);
    
    // populate anchor grids
    anchorMeshDimsA = ceil(sqrt(rowsTagsA));
    for (int i=0; i < rowsTagsA; i++) {
        float x = (int(i/anchorMeshDimsA))*gridDimFactor;
        float z = (i%anchorMeshDimsA)*gridDimFactor;;
        float v = ofToInt(tableTagsA.at(i).at(1));
        anchorsA.push_back(Anchor(x,z,v,tableTagsA.at(i).at(0)));
        anchorMeshA.addVertex(anchorsA.at(i).position);
    }
    
    anchorMeshDimsB = ceil(sqrt(rowsTagsB));
    for (int i=0; i < rowsTagsB; i++) {
        float x = (int(i/anchorMeshDimsB))*gridDimFactor;
        float z = (i%anchorMeshDimsB)*gridDimFactor;;
        float v = ofToInt(tableTagsB.at(i).at(1));
        anchorsB.push_back(Anchor(x,z,v,tableTagsB.at(i).at(0)));
        anchorMeshB.addVertex(anchorsB.at(i).position);
    }
    
    // add indices
    for (int y=0; y < anchorMeshDimsA; y++) {
        for (int x=0; x < anchorMeshDimsA; x++) {
            // account for arbitrary number of anchors
            if (x+(y+1)*anchorMeshDimsA < anchorsA.size()-1) {
                anchorMeshA.addIndex(x+y*anchorMeshDimsA);
                anchorMeshA.addIndex((x+1)+y*anchorMeshDimsA);
                anchorMeshA.addIndex(x+(y+1)*anchorMeshDimsA);
                anchorMeshA.addIndex((x+1)+y*anchorMeshDimsA);
                anchorMeshA.addIndex((x+1)+(y+1)*anchorMeshDimsA);
                anchorMeshA.addIndex(x+(y+1)*anchorMeshDimsA);
            }
        }
    }
    
    for (int y=0; y < anchorMeshDimsB; y++) {
        for (int x=0; x < anchorMeshDimsB; x++) {
            // account for arbitrary number of anchors
            if (x+(y+1)*anchorMeshDimsB < anchorsB.size()-1) {
                anchorMeshB.addIndex(x+y*anchorMeshDimsB);
                anchorMeshB.addIndex((x+1)+y*anchorMeshDimsB);
                anchorMeshB.addIndex(x+(y+1)*anchorMeshDimsB);
                anchorMeshB.addIndex((x+1)+y*anchorMeshDimsB);
                anchorMeshB.addIndex((x+1)+(y+1)*anchorMeshDimsB);
                anchorMeshB.addIndex(x+(y+1)*anchorMeshDimsB);
            }
        }
    }
    
    calcNormals(anchorMeshA,false);
    calcNormals(anchorMeshB,false);
    
    // populate universes
    for (int i=0; i < rowsPapersA; i++) {
        papersA.push_back(Paper(tablePapersA.at(i).at(1),tablePapersA.at(i).at(2),tablePapersA.at(i).at(3),ofToInt(tablePapersA.at(i).at(4)),ofToInt(tablePapersA.at(i).at(5)),tablePapersA.at(i).at(6),paperMeshA,paperIndexCountA,paperVertexNumberA,paperColor,anchorsA));
    }
    
    for (int i=0; i < rowsPapersB; i++) {
        papersB.push_back(Paper(tablePapersB.at(i).at(1),tablePapersB.at(i).at(2),tablePapersB.at(i).at(3),ofToInt(tablePapersB.at(i).at(4)),ofToInt(tablePapersB.at(i).at(5)),tablePapersB.at(i).at(6),paperMeshB,paperIndexCountB,paperVertexNumberB,paperColor,anchorsB));
    }
    
    calcNormals(paperMeshA,false);
    calcNormals(paperMeshB,false);
    
}


void ofApp::draw(){
    
    int drawHeight,drawWidth,drawFontHUDSize,drawCommonMargin,drawImageMargin;
    float fboScalar = ((fbo.getWidth()/ofGetWidth()) + (fbo.getHeight()/ofGetHeight()))/2;
    
    if (fboOn) {
        fbo.begin();
        
        drawHeight = fbo.getHeight();
        drawWidth = fbo.getWidth();
        
        drawFontHUDSize = int((ofGetWidth()*fontHUDScale) * fboScalar);
        drawCommonMargin = int((ofGetWidth()*layoutCommonMarginScale) * fboScalar);
        drawImageMargin = int((ofGetWidth()*layoutImageMarginScale) * fboScalar);
        fontHud.load(fontFile,drawFontHUDSize,true,true,true);
        font3D.load(fontFile,font3DSize,true,true,true);
        
    }
    
    else {
        drawHeight = ofGetHeight();
        drawWidth = ofGetWidth();
        drawFontHUDSize = int(ofGetWidth()*fontHUDScale);
        drawCommonMargin = int(ofGetWidth()*layoutCommonMarginScale);
        drawImageMargin = int(ofGetWidth()*layoutImageMarginScale);
    }
    
    ofEnableDepthTest();
    ofClear(backgroundColor);
    
    cam.begin();
    
    if (mode == 'A') {
        ofSetColor(gridColor);
        anchorMeshA.drawWireframe();
        ofSetColor(paperColor);
        paperMeshA.drawWireframe();
        for (Paper &p : papersA) {
            p.draw();
            for (Anchor &a : p.paperAnchors) {
                a.draw();
            }
        }
    }
    
    if (mode == 'B') {
        ofSetColor(gridColor);
        anchorMeshB.drawWireframe();
        ofSetColor(paperColor);
        paperMeshB.drawWireframe();
        for (Paper &p : papersB) {
            p.draw();
            for (Anchor &a : p.paperAnchors) {
                a.draw();
            }
        }
    }
    
    cam.end();
    ofDisableDepthTest();
    
    ofSetColor(255,255,255,255);
    
    if (fboOn) {
        float aspectFactor = image.getWidth()/image.getHeight();
        ofImage scaled;
        scaled.clone(image);
        scaled.resize(image.getWidth()*fboScalar,image.getHeight()*fboScalar*aspectFactor);
        if (imageShow) {scaled.draw(drawImageMargin,(drawHeight/imageOffset)-(scaled.getHeight()/2));}
    }
    
    else {if (imageShow) {image.draw(drawImageMargin,(drawHeight/imageOffset)-(image.getHeight()/2));}}
    
    ofSetColor(hudTextColor1);
    fontHud.drawString(layoutString(hudTextUpper1,layoutUpperHudLineWidth),drawCommonMargin,drawCommonMargin);
    ofSetColor(hudTextColor2);
    fontHud.drawString(layoutString(hudTextUpper2,layoutUpperHudLineWidth),drawCommonMargin,drawCommonMargin+1*(drawFontHUDSize*layoutUpperHudLineSpacing));
    ofSetColor(hudTextColor1);
    fontHud.drawString(layoutString(hudTextUpper3,layoutUpperHudLineWidth),drawCommonMargin,drawCommonMargin+2*((drawFontHUDSize*layoutUpperHudLineSpacing)));
    
    ofSetColor(hudTextColor1);
    float lowerHudHeight = fontHud.stringHeight(layoutString(hudTextLower,layoutUpperHudLineWidth));
    float lowerHudX = drawCommonMargin;
    float lowerHudY = drawHeight - drawCommonMargin - lowerHudHeight;
    ofSetColor(hudTextColor2);
    fontHud.drawString(layoutString(hudTextLower,layoutLowerHudLineWidth),lowerHudX,lowerHudY);
    
    ofSetColor(hudTextColor2);
    float titleHudWidth = fontHud.stringWidth(layoutString(hudTextTitle,layoutTitleHudLineWidth));
    float titleHudX = drawWidth - drawCommonMargin - titleHudWidth;
    float titleHudY = drawCommonMargin;
    fontHud.drawString(layoutString(hudTextTitle,layoutTitleHudLineWidth),titleHudX,titleHudY);
    
    ofSetColor(hudTextColor2);
    float creditHudWidth = fontHud.stringWidth(layoutString(hudTextCredit,layoutCreditHudLineWidth));
    float creditHudHeight = fontHud.stringHeight(layoutString(hudTextCredit,layoutCreditHudLineWidth));
    float creditHudX = drawWidth - drawCommonMargin - creditHudWidth;
    float creditHudY = drawHeight - drawCommonMargin - creditHudHeight;
    fontHud.drawString(layoutString(hudTextCredit,layoutCreditHudLineWidth),creditHudX,creditHudY);
    
    // for some reason the cam needs around 10 frames to take these values
    if (firstRun < 10) {
        firstRun++;
        resetCam();
    }
    
    // save a hi-res frame
    if (fboOn) {
        fbo.end();
        fboOn = false;
        
        fontHud.load(fontFile,int(ofGetWidth()*fontHUDScale),true,true,true);
        font3D.load(fontFile,font3DSize,true,true,true);
        
        ofPixels hiResPixels;
        fbo.readToPixels(hiResPixels);
        ofFileDialogResult dialog_result = ofSystemSaveDialog("fbo", "export");
        ofSaveImage(hiResPixels, dialog_result.getPath() + ".tiff");
    }
    
}


void ofApp::resetCam() {
    cam.setPosition(ofVec3f(300,450,1400));
    if (mode == 'A') {cam.setTarget(ofVec3f(anchorMeshDimsA/2+280,100,anchorMeshDimsA/2));}
    if (mode == 'B') {cam.setTarget(ofVec3f(anchorMeshDimsB/2+280,100,anchorMeshDimsB/2));}
}


string ofApp::layoutString(string pS, int pLineLength) {
    
    vector<string> splitS = ofSplitString(pS," ");
    string lineS = "";
    string resultS = "";
    
    for (string &s : splitS) {
        if (lineS.length() < pLineLength) {
            lineS += s + " ";
        }
        else {
            resultS += lineS + s + "\n";
            lineS = "";
        }
    }
    
    resultS += lineS;
    return resultS;
    
}


void ofApp::loadCSVAsStrings(vector<vector<string> > &pData, string pPath, string pSeparator, string pEscaper) {
    
    // open the file
    ifstream fileIn;
    fileIn.open(ofToDataPath(pPath, true).c_str());
    
    if(fileIn.is_open()) {
        int lineCount = 0;
        vector<string> rows;
        
        while(fileIn.good()) {
            string rowAsString;
            getline(fileIn, rowAsString);
            
            // skip empty lines.
            if(rowAsString.length() == 0) {}
            else {
                
                // remove carriage returns and newlines
                ofStringReplace(rowAsString,"\r","");
                ofStringReplace(rowAsString,"\n","");
                
                string escapedSeparator = pEscaper + pSeparator;
                string tempReplacement = randomString(100);
                
                ofStringReplace(rowAsString,escapedSeparator,tempReplacement);
                
                vector<string> currentRow = ofSplitString(rowAsString, pSeparator);
                
                // put escaped separator back
                for (string &s : currentRow) {
                    ofStringReplace(s,tempReplacement,pSeparator);
                }
                
                pData.push_back(currentRow);
                
            }
        }
        
    }
    else {
        cerr << "Error opening " << pPath << ".\n";
    }
    
}


string ofApp::randomString(int pLen) {
    
    static const string alphaNum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string returnString = "";
    for (int i=0; i < pLen; i++) {returnString += alphaNum[int(ofRandom(0,alphaNum.length()-1))];}
    return(returnString);
    
}


void ofApp::calcNormals(ofMesh &pMesh, bool pNormalize){
    
    for (int i=0; i < pMesh.getVertices().size(); i++) {
        pMesh.addNormal(ofPoint(0,0,0));
    }
    
    for (int i=0; i < pMesh.getIndices().size(); i+=3){
        const int ia = pMesh.getIndices()[i];
        const int ib = pMesh.getIndices()[i+1];
        const int ic = pMesh.getIndices()[i+2];
        
        ofVec3f e1 = pMesh.getVertices()[ia] - pMesh.getVertices()[ib];
        ofVec3f e2 = pMesh.getVertices()[ic] - pMesh.getVertices()[ib];
        ofVec3f no = e2.cross( e1 );
        
        pMesh.getNormals()[ia] += no;
        pMesh.getNormals()[ib] += no;
        pMesh.getNormals()[ic] += no;
    }
    
    if (pNormalize)
        for(int i=0; i < pMesh.getNormals().size(); i++ ) {
            pMesh.getNormals()[i].normalize();
        }
}


void ofApp::mouseMoved(int x, int y ){
    // to avoid the cursor being hidden (bug in OSX Yosemite, see http://forum.openframeworks.cc/t/mouse-cursor-invisible-in-osx/17929)
    ofShowCursor();
}


void ofApp::mousePressed(int x, int y, int button){
    
    // check if image clicked (not in FBO mode)
    if (x > (layoutImageMarginScale*ofGetWidth())+imageLeftMargin &&
        x < image.getWidth()+(layoutImageMarginScale*ofGetWidth())-imageRightMargin &&
        y > (ofGetHeight()/imageOffset)-(image.getHeight()/2)+imageTopMargin &&
        y < (ofGetHeight()/imageOffset)+(image.getHeight()/2)-imageBottomMargin) {
        if (mode == 'A') {mode = 'B';}
        else {mode = 'A';}
    }
    
    // 3D picking (not in FBO mode)
    ofVec2f mouse(x,y);
    
    if (mode == 'A') {
        int n = paperMeshA.getNumVertices();
        float nearestDistance = 0;
        ofVec2f nearestVertex;
        int nearestIndex = 0;
        
        for(int i = 0; i < n; i++) {
            ofVec3f cur = cam.worldToScreen(paperMeshA.getVertex(i));
            float distance = cur.distance(mouse);
            if(i == 0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex = cur;
                nearestIndex = i;
            }
        }
        
        for (Paper &p : papersA) {
            p.active = false;
            for (Anchor &a : p.paperAnchors) {
                a.active = false;
            }
        }
        int paperNo = nearestIndex/paperVertexNumberA;
        papersA.at(paperNo).active = true;
        hudTextUpper1 = papersA.at(paperNo).paperAuthorsYearSource;
        hudTextUpper2 = papersA.at(paperNo).paperTitle;
        
        hudTextUpper3 = "";
        for (Anchor &a : papersA.at(paperNo).paperAnchors) {
            hudTextUpper3 += a.anchorTag + ", ";
            a.active = true;
        }
        hudTextUpper3 += "...";
        
        hudTextLower = papersA.at(paperNo).paperAbstract;
        
    }
    
    if (mode == 'B') {
        int n = paperMeshB.getNumVertices();
        float nearestDistance = 0;
        ofVec2f nearestVertex;
        int nearestIndex = 0;
        
        for(int i = 0; i < n; i++) {
            ofVec3f cur = cam.worldToScreen(paperMeshB.getVertex(i));
            float distance = cur.distance(mouse);
            if(i == 0 || distance < nearestDistance) {
                nearestDistance = distance;
                nearestVertex = cur;
                nearestIndex = i;
            }
        }
        
        for (Paper &p : papersB) {
            p.active = false;
            for (Anchor &a : p.paperAnchors) {
                a.active = false;
            }
        }
        int paperNo = nearestIndex/paperVertexNumberB;
        papersB.at(paperNo).active = true;
        hudTextUpper1 = papersB.at(paperNo).paperAuthorsYearSource;
        hudTextUpper2 = papersB.at(paperNo).paperTitle;
        
        hudTextUpper3 = "";
        for (Anchor &a : papersB.at(paperNo).paperAnchors) {
            hudTextUpper3 += a.anchorTag + ", ";
            a.active = true;
        }
        hudTextUpper3 += "...";
        
        hudTextLower = papersB.at(paperNo).paperAbstract;
        
    }
    
}


void ofApp::keyPressed(int key){
    if (key == 'c') {resetCam();}
    if (key == 'f') {fboOn = true;}
    if (key == 's') {if (mode == 'A') {mode = 'B';} else {mode = 'A';}}
}


void ofApp::update(){
    if (mode == 'A') {
        for (Paper &p : papersA) {
            p.update();
            for (Anchor &a : p.paperAnchors) {
                a.update();
            }
        }
        
    }
    if (mode == 'B') {
        for (Paper &p : papersB) {
            p.update();
            for (Anchor &a : p.paperAnchors) {
                a.update();
            }
        }
        
    }
}


// unused openFrameworks template functions
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::keyReleased(int key){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
