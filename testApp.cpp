#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	blobDetectorWidth = 640;
	blobDetectorHeight = 480;
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(blobDetectorWidth, blobDetectorHeight);		
    colorImg.allocate(blobDetectorWidth, blobDetectorHeight);
	grayImage.allocate(blobDetectorWidth, blobDetectorHeight);
	grayBg.allocate(blobDetectorWidth, blobDetectorHeight);
	grayDiff.allocate(blobDetectorWidth, blobDetectorHeight);

	bLearnBakground = true;
	threshold = 120;

	for (int i=0; i < 4; i++)
		isQuadrantOn[i] = false;

	quadrantWidth = 50;
	quadrantHeight = 50;
	minBlobArea = 200;
	windowWidth = 10;
	windowHeight = 10;

	//audio setup
	int bufferSize		= 512;
	sampleRate 			= 44100;
	//phase, phase2, phase3 = 0.0f;
	phase = 0.0f; phase2 = 0.0f; phase3 = 0.0f; phase4 = 0.0f;
	phaseAdder = 0.0f; phaseAdder2 = 0.0f; phaseAdder3 = 0.0f; phaseAdder4 = 0.0f;
	phaseAdderTarget = 0.0f; phaseAdderTarget2 = 0.0f; phaseAdderTarget3 = 0.0f; phaseAdderTarget4 = 0.0f;
	volume				= 0.1f;
	bNoise 				= false;
	lAudio.assign(bufferSize, 0.0);
	rAudio.assign(bufferSize, 0.0);
	lAudio2.assign(bufferSize, 0.0);
	rAudio2.assign(bufferSize, 0.0);
	soundStream.setup(this, 2, 0, sampleRate, bufferSize, 4);

	targetFrequency = 440.0f;
	phaseAdderTarget = (targetFrequency / (float) sampleRate) * TWO_PI;
	//pythagorean 5th
	phaseAdderTarget2 = ((targetFrequency * 1.5f) / (float) sampleRate) * TWO_PI;
	//pythagorean major third (plus an octave)
	phaseAdderTarget3 = (((targetFrequency * 2.0f) * (81.0f / 64.0f)) / (float) sampleRate) * TWO_PI;
	//pythagorean major seventh (plus an octave)
	phaseAdderTarget4 = (((targetFrequency * 2.0f) * (243.0f / 128.0f)) / (float) sampleRate) * TWO_PI;
	pan = 0.5f;
	
	ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

        colorImg.setFromPixels(vidGrabber.getPixels(), blobDetectorWidth, blobDetectorHeight);

        grayImage = colorImg;
		if (bLearnBakground == true) {
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of minBlobArea pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, minBlobArea, 
			(blobDetectorWidth * blobDetectorHeight) / 3, 10, false);	// don't find holes
	}

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	//ofSetHexColor(0xffffff);
	//colorImg.draw(20,20);
	//grayImage.draw(360,20);
	//grayBg.draw(20,280);
	//grayDiff.draw(360,280);

	for (int i=0; i < 4; i++)
		if (isQuadrantOn[i]) fillInQuadrant(i);

	// draw grey box behind the contours:	
	int contourBoxX = 160;
	int contourBoxY = 340;
	//ofFill();
	//ofSetHexColor(0x333333);
	//ofRect(contourBoxX, contourBoxY, 320, 240);
	//ofSetHexColor(0xffffff);

	//see which quadrants are on
	for (int i = 0; i < 4; i++)
		setQuadrantState(i, false);
    for (int i = 0; i < contourFinder.nBlobs; i++) {
		int quadNum = getQuadrant(contourFinder.blobs[i].centroid);
		//isQuadrantOn[getQuadrant(contourFinder.blobs[i].centroid)] = true;
		setQuadrantState(getQuadrant(contourFinder.blobs[i].centroid), true);
    }
	for (int i = 0; i < 4; i++) {
		bool thing = isQuadrantOn[i]; //DEBUG
		if (isQuadrantOn[i]) fillInQuadrant(i);
	}

	//draw each blob individually
    for (int i = 0; i < contourFinder.nBlobs; i++) {
        contourFinder.blobs[i].draw(0, 0);
    }

	// finally, a report:
	ofSetHexColor(0xffffff);
	char reportStr[1024];
	sprintf(reportStr, "bg subtraction and blob detection\npress ' ' to capture bg\nthreshold %i (press: +/-)\nnum blobs found %i, fps: %f", threshold, contourFinder.nBlobs, ofGetFrameRate());
	ofDrawBitmapString(reportStr, 20, 600);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
		case '0':
			isQuadrantOn[0] = !isQuadrantOn[0];
			break;
		case '1':
			isQuadrantOn[1] = !isQuadrantOn[1];
			break;
		case '2':
			isQuadrantOn[2] = !isQuadrantOn[2];
			break;
		case '3':
			isQuadrantOn[3] = !isQuadrantOn[3];
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	quadrantWidth = w / 2;
	quadrantHeight = h / 2;
	windowWidth = w;
	windowHeight = h;
	//blobDetectorWidth = w;
	//blobDetectorHeight = h;
	//grayDiff.resize(blobDetectorWidth, blobDetectorHeight);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::fillInQuadrant(int quadrantNum) {
	int rowNum = quadrantNum / 2;
	int colNum = quadrantNum % 2;

	ofFill();
	ofSetHexColor(0x999922);
	ofRect(colNum * quadrantWidth, 
		   rowNum * quadrantHeight, 
		   quadrantWidth, 
		   quadrantHeight);
}

int testApp::getQuadrant(ofPoint p) {
	int rowNum = p.y / quadrantHeight;
	int colNum = p.x / quadrantWidth;

	return rowNum * 2 + colNum;
}

void testApp::audioOut(float * output, int bufferSize, int nChannels){
	float leftScale = 1 - pan;
	float rightScale = pan;
	volume = 0.05f;

	// sin (n) seems to have trouble when n is very large, so we
	// keep phase in the range of 0-TWO_PI like this:
	while (phase > TWO_PI){
		phase -= TWO_PI;
	}

	phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
	phaseAdder2 = 0.95f * phaseAdder2 + 0.05f * phaseAdderTarget2;
	phaseAdder3 = 0.95f * phaseAdder3 + 0.05f * phaseAdderTarget3;
	phaseAdder4 = 0.95f * phaseAdder4 + 0.05f * phaseAdderTarget4;
	for (int i = 0; i < bufferSize; i++){
		phase += phaseAdder; phase2 += phaseAdder2; phase3 += phaseAdder3; phase4 += phaseAdder4;
		float sample = sin(phase);
		float sample2 = sin(phase2);
		float sample3 = sin(phase3);
		float sample4 = sin(phase4);

		float sample_left = 0.0f;
		float sample_right = 0.0f;
		if (isQuadrantOn[0]) sample_left += sample;
		if (isQuadrantOn[1]) sample_left += sample2;
		if (isQuadrantOn[2]) sample_right += sample3;
		if (isQuadrantOn[3]) sample_right += sample4;

		lAudio[i] = output[i*nChannels    ] = sample_left * volume * leftScale;
		rAudio[i] = output[i*nChannels + 1] = sample_right * volume * rightScale;
	}
}

void testApp::setQuadrantState(int quadNum, bool isOn) {
	isQuadrantOn[quadNum] = isOn;
}