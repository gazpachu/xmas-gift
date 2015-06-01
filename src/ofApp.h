#pragma once
#include "ofMain.h"
#include "ofVbo.h"
#include "ofxOpenNI.h"
#include "ofTrueTypeFont.h"
#include "ofxBlurShader.h"

#define NUM_BILLBOARDS 8000

class ofApp : public ofBaseApp {
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofVbo vbo;
		ofShader shader;
		ofImage texture;
		ofVec2f pos[NUM_BILLBOARDS];
		ofVec2f vel[NUM_BILLBOARDS];
		ofVec2f home[NUM_BILLBOARDS];
		float pointSizes[NUM_BILLBOARDS];
		float rotations[NUM_BILLBOARDS];

		ofxOpenNIContext niContext;
        ofxDepthGenerator niDepthGenerator;
        ofxImageGenerator niImageGenerator;
        ofxUserGenerator niUserGenerator;
        ofxHandGenerator* niHandGenerator;
        ofxTrackedHand* tracked;

        int nearThreshold;
        int farThreshold;
        int score, oldY, oldX, frame;
        float filterFactor;
        bool started;

        ofImage bg, gift1, gift2;
        ofTrueTypeFont scoreFont;

        ofxBlurShader blur;
};









