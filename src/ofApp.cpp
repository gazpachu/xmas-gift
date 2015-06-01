#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	ofBackground(0);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);

	if( ! bg.loadImage("bg.jpg")) printf("Bg loading failed");
	if( ! gift1.loadImage("gift1.png")) printf("Gift1 loading failed");
	if( ! gift2.loadImage("gift2.png")) printf("Gift2 loading failed");

	scoreFont.loadFont("bebasneue-webfont.ttf", 25);

	blur.setup(ofGetWidth(),ofGetHeight());

	// billboard particles
	for (int i=0; i<NUM_BILLBOARDS; i++) {
		pos[i].x = ofRandomWidth();
		pos[i].y = ofRandomHeight();
		vel[i].x = ofRandomf();
		vel[i].y = ofRandomf();
		home[i] = pos[i];
		pointSizes[i] = ofNextPow2(ofRandom(2, 40));
		rotations[i] = ofRandom(0, 360);
	}

	// set the vertex data
	vbo.setVertexData(pos, NUM_BILLBOARDS, GL_DYNAMIC_DRAW);
	shader.load("Billboard");

	ofDisableArbTex();
	texture.loadImage("snow.png");

	nearThreshold = 400;
    farThreshold = 700;
    filterFactor = 0.1f;
    oldY = 0;
    oldX = 0;
    frame = 0;
    started = false;

	niContext.setup();
	//niContext.toggleRegisterViewport();
	niContext.toggleMirror();

    niDepthGenerator.setup( &niContext );
    niImageGenerator.setup( &niContext );

    niUserGenerator.setup( &niContext );
    niUserGenerator.setSmoothing(filterFactor);				// built in openni skeleton smoothing...
	niUserGenerator.setMaxNumberOfUsers(1);

    niHandGenerator = new ofxHandGenerator();
	niHandGenerator->setup(&niContext,1);
	niHandGenerator->setMaxNumHands(1);
	//niHandGenerator.setSmoothing(filterFactor);		// built in openni hand track smoothing...
	//niHandGenerator.setFilterFactors(filterFactor);	// custom smoothing/filtering (can also set per hand with setFilterFactor)...set them all to 0.1f to begin with
    //niHandGenerator.startTrackHands();
}

//--------------------------------------------------------------
void ofApp::update()
{
    niDepthGenerator.setDepthThreshold(0, nearThreshold, farThreshold);

    niContext.update();
    niDepthGenerator.update();
    niImageGenerator.update();
    niUserGenerator.update();

    tracked = niHandGenerator->getHand(0);

    if( tracked->projectPos.y )
        started = true;

    ofVec2f mouse(tracked->projectPos.x, tracked->projectPos.y);
    ofVec2f mouseVec(oldX - tracked->projectPos.x, oldY - tracked->projectPos.y);

    if( oldY > tracked->projectPos.y + 15 && oldX > tracked->projectPos.x + 15 )
        score += 1;


    oldY = tracked->projectPos.y;
    oldX = tracked->projectPos.x;

    mouseVec.limit(10.0);

    for (int i=0; i<NUM_BILLBOARDS; i++) {
        ofSeedRandom(i);
        if(mouse.distance(pos[i]) < ofRandom(100, 200)) {
            vel[i] -= mouseVec;
        }

        pos[i] += vel[i];
        vel[i] *= 0.84f;

        if(pos[i].x < 0) pos[i].x = ofGetWidth();
        if(pos[i].x > ofGetWidth()) pos[i].x = 0;
        if(pos[i].y < 0) pos[i].y = ofGetHeight();
        if(pos[i].y > ofGetHeight()) pos[i].y = 0;

        ofVec2f center(ofGetWidth()/2, ofGetHeight()/2);
        ofVec2f frc = home[i] - pos[i];
        if(frc.length() > 20.0) {
            frc.normalize();
            frc *= 0.84;
            vel[i] += frc;
        }

        // get the 2d heading
        float angle = (float)atan2(-vel[i].y, vel[i].x) + PI;
        rotations[i] = (angle * -1.0);
    }

    if( frame < 60 )
        frame++;
    else
        frame = 0;
}

//--------------------------------------------------------------
void ofApp::draw() {

    //bg.resize(ofGetWidth(), ofGetHeight());
    ofSetColor(255);

    blur.begin();
    blur.amount = ofMap(10 - (score/3),0,10,0,10,true);
    blur.iterations = ofMap(10 - (score/3),0,10,1,10,true);
    bg.draw(0,0);

    if( frame < 30 )
        gift1.draw(ofGetWidth()/2 - 125, ofGetHeight()/2 + 76);
    else
        gift2.draw(ofGetWidth()/2 - 125, ofGetHeight()/2 + 76);

    blur.end();

    //ofDrawBitmapString("amount: " + ofToString(blur.amount), 100,150);
    //ofDrawBitmapString("iterations: " + ofToString(blur.iterations), 100,250);

    //niDepthGenerator.draw(0,0, ofGetWidth(), ofGetHeight());
    //niImageGenerator.draw(0, 0, 640, 480);

    glPushMatrix();

    ofScale( 1.6f, 1.6f, 1.6f );

	ofEnableAlphaBlending();
	ofSetColor(255);

	ofEnablePointSprites();
	shader.begin();

	// we are getting the location of the point size attribute
	// we then set the pointSizes to the vertex attritbute
	// we then bind and then enable
	int pointAttLoc = shader.getAttributeLocation("pointSize");
	glVertexAttribPointer(pointAttLoc, 1, GL_FLOAT, false, 0, pointSizes);
	glBindAttribLocation(shader.getProgram(), pointAttLoc, "pointSize");
	glEnableVertexAttribArray(pointAttLoc);

	// rotate the snow based on the velocity
	int angleLoc = shader.getAttributeLocation("angle");
	glVertexAttribPointer(angleLoc, 1, GL_FLOAT, false, 0, rotations);
	glBindAttribLocation(shader.getProgram(), angleLoc, "angle");
	glEnableVertexAttribArray(angleLoc);

	texture.getTextureReference().bind();
	vbo.updateVertexData(pos, NUM_BILLBOARDS);
	vbo.draw(GL_POINTS, 0, NUM_BILLBOARDS);
	texture.getTextureReference().unbind();

	shader.end();
	ofDisablePointSprites();

	// disable vertex attributes
	glDisableVertexAttribArray(pointAttLoc);
	glDisableVertexAttribArray(angleLoc);

	glPopMatrix();

    if( !started )
    {
        ofSetColor(255, 255, 255);
        scoreFont.drawString("PLEASE, CLEAR OUR WINDOW IN CIRCLES TO GET YOUR XMAS GIFT", ofGetWidth()/2 - 350, ofGetHeight()/2);
    }
    else
    {
        glPushMatrix();
        ofScale( 1.6f, 1.6f, 1.6f );
        niHandGenerator->drawHands();
        glPopMatrix();

        ofSetColor(110, 255, 0);
        scoreFont.drawString("SCORE " + ofToString(score), 10, 30);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'f') {
		ofToggleFullscreen();
	}

	if(key == 32) { //spacebar
        score = 0;
        started = false;
        niHandGenerator->dropHands();

        delete tracked;
        delete niHandGenerator;
        niHandGenerator = new ofxHandGenerator();
        niHandGenerator->setup(&niContext,1);
        niHandGenerator->setMaxNumHands(1);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	for (int i=0; i<NUM_BILLBOARDS; i++) {
		home[i].x = ofRandomWidth();
		home[i].y = ofRandomHeight();
	}
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
