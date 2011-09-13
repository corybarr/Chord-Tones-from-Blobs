#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"

#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file

class testApp : public ofBaseApp{

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

		ofVideoGrabber 		vidGrabber;

        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;

		//audio stuff
		ofSoundStream soundStream;
		float 	pan;
		int		sampleRate;
		bool 	bNoise;
		float 	volume;
		float 	targetFrequency;
		float 	phase, phase2, phase3, phase4;
		float 	phaseAdder, phaseAdder2, phaseAdder3, phaseAdder4;
		float 	phaseAdderTarget, phaseAdderTarget2, phaseAdderTarget3, phaseAdderTarget4;
		vector <float> lAudio, rAudio, lAudio2, rAudio2;
		void audioOut(float * input, int bufferSize, int nChannels);

	private:
		void fillInQuadrant(int quadrantNum);
		bool isQuadrantOn[4];
		int quadrantHeight, quadrantWidth;
		int minBlobArea;
		int getQuadrant(ofPoint p);
		int windowWidth, windowHeight;
		int blobDetectorWidth, blobDetectorHeight;
		void setQuadrantState(int quadNum, bool isOn);
};

