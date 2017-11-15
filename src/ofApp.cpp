// This is a test line
#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    // Set background.
    ofBackground(ofColor::black);
    
    // Setup the video player.
    grabber.setup(600,440);

    boundingSize = {2000, 500, 1000};

    decayRate = 0.5;
    volume = 0;
    frequency = 0;
    frequencyTarget = frequency;
    
    minimumThreshold = 0.1;
    threshold1 = minimumThreshold;
    
    phase = 0;
    updateWaveform(32);
    
    soundStream.printDeviceList();
    
    int bufferSize = 256;
    
    left.assign(bufferSize, 0.0);
    right.assign(bufferSize, 0.0);
    volHistory.assign(400, 0.0);
    

    bufferCounter	= 0;
    drawCounter		= 0;
    smoothedVol     = 0.0;
    scaledVol		= 0.0;

    
    ofSoundStreamSetup(1, 0); // mono output=1; stereo=2
    
    ofSoundStreamSettings settings;

    auto devices = soundStream.getMatchingDevices("default");
    if(!devices.empty()){
        settings.setInDevice(devices[0]);
    }
    
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    ofSetCircleResolution(50);
    
    image.load("bingo.jpg");
    
 

    
//    frabk.load("frabk.tff", 14, true, true);
//    frabk.setLineHeight(8);

}

//-------------------------------------------------------------
void ofApp::update(){
    
    // Update the player.
    grabber.update();
    scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    volHistory.push_back( scaledVol );
    if( volHistory.size() >= 400 ){
        volHistory.erase(volHistory.begin(), volHistory.begin()+1);
    }
   
    //i dont get how to get the screen save to reload
    if(ofGetSeconds()>20)
    {   image.update();
        image.getPixels();
    }
    
    
//    ofPixels newPixels;
//    int skipPixel = 1;
//    if (grabber.isFrameNew())
//    {
//        // Do some work.
//        grabberPixels = grabber.getPixels();
//        for (int x = 0; x < grabberPixels.getWidth(); x = x + skipPixel) {
//            for (int y = 0; y < grabberPixels.getHeight(); y = y + skipPixel) {
//                // Modify each pixel at [x, y] based on waveform values.
//                 grabberPixels.setColor(x, y, grabberPixels.getColor(x,y).getBrightness());
//                }
//
//                // Update newPixels
//            
//        }
//        

//    grabberTexture.loadData(grabberPixels);
//    newTexture.loadData(grabberPixels);
//    }

    frequency = ofLerp(frequency, frequencyTarget, 0.4);
    
 
    if(ofGetKeyPressed()) {
        volume= ofLerp(volume, 1, 0.8); // jump quickly to 1
    } else {
        volume= ofLerp(volume, 0, 0.1); // fade slowly to 0
    }
    
    ofScopedLock waveformLock(waveformMutex);
    updateWaveform(ofMap(ofGetMouseX(), 0, ofGetWidth(), 3, 150, true));
}

//    
//    
//    if(ofGetKeyPressed('l'))
//    {
//    

    //for envelope generator; key is pressed affecting attack/decay
    //ofLerp= float start, float stop, and float amount;
   


//--------------------------------------------------------------
void ofApp::draw(){
    // Reset the color of grabber texture to original color.
    ofSetColor(ofColor::white);
 
    
    for (auto position: outLine)
    {
        if (position.x < grabber.getWidth())
        {     }
//
            ofFill();

                ofDrawCircle(20,20, scaledVol * 190.0f);
        for (int i=0; i<ofGetWidth(); i++)
        {
//            ofSetColor(grabber.getPixels().getColor(100));
            ofSetColor(ofColor::white);
            ofDrawLine(0+i,ofGetHeight(),0,ofGetHeight()-(4*(scaledVol*190.0f)));
        }
    
            ofNoFill();
        
            
        
    
    
    //for variable frame color dependent on frequency
    float lineVariable= ofMap(frequencyTarget,0,3000,5,50);
    float colorVariable= ofRandom(40,ofMap(frequencyTarget,40,3000,30,255));
    float hueVariable= ofMap(frequencyTarget,40,3000,0,255);
    ofSetLineWidth(lineVariable);
    ofSetColor(ofColor::fromHsb(hueVariable, colorVariable, 255));

//    ofSetColor(ofColor::white);
    float value = 0.2;
    
    for (auto point: outLine)
    {
        ofDrawCircle(point, 3);
        
    }
    
    
    if ('f')
        {
            image.draw(400, 460, 300, 200);
    
        }
    

    
    //INPUT AUDIO CIRCLE
    
    //lets draw the volume history as a graph
//    ofBeginShape();
//    for (unsigned int i = 0; i < volHistory.size(); i++){
//        if( i == 0 ) ofVertex(i, 400);
//        
//        ofVertex(i, 400 - volHistory[i] * 70);
//        
//        if( i == volHistory.size() -1 ) ofVertex(i, 400);
//    }
//    ofEndShape(false);
    


//
    

    
    
    
}

//--------------------------------------------------------------
void ofApp::updateWaveform(int waveformResolution) {
    waveform.resize(waveformResolution);
    //waveLine.clear();
    
    // "waveformStep" maps a full oscillation of sin() to the size
    // of the waveform lookup table
    float waveformStep = (M_PI * 2.) / (float) waveform.size();
    
    for(int i = 0; i < waveform.size(); i++) {
        waveform[i] = sin(i * waveformStep);
    }
}
//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    
    float curVol = 0.0;
    
    // samples are "interleaved"
    int numCounted = 0;
    
    //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
    for (int i = 0; i < input.getNumFrames(); i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    //this is how we get the mean of rms :)
    curVol /= (float)numCounted;
    
    // this is how we get the root of rms :)
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}

//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
        ofScopedLock waveformLock(waveformMutex);
        
        float sampleRate = 44100;
    //prev^ =44100
        float phaseStep = frequency / sampleRate;
        
        outLine.clear();
    
        float alpha = 0.999;
        float ySmooth = ofMap(0, -1, 1, 0, ofGetHeight());
    
        for(int i = 0; i < bufferSize * nChannels; i += nChannels)
        {
            phase += phaseStep;
            int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
            //This below line is specifically for envelope generator
            
            output[i] = waveform[waveformIndex] * volume;
            
            ySmooth = (1 - alpha) * ofMap(output[i], -1, 1, 0, ofGetHeight()) + (alpha) * ySmooth;
            
            outLine.addVertex(ofMap(i, 0, bufferSize - 1, 0, ofGetWidth()),
                              ySmooth);
            
        }
    
    
}
//___
    
void ofApp::audioIn(float * input, int bufferSize, int nChannels) {
//////         ... the contents of audioIn() from audioInputExample ...
    float rms = 0.0;
    int numCounted = 0;

    for (int i = 0; i < bufferSize; i++){
        float leftSample = input[i * 2] * 0.5;
        float rightSample = input[i * 2 + 1] * 0.5;

        rms += leftSample * leftSample;
        rms += rightSample * rightSample;
        numCounted += 2;
    }
    rms /= (float)numCounted;
    rms = sqrt(rms);

        threshold1= ofLerp(threshold1, minimumThreshold, decayRate);

        if(rms > threshold1) {
            // onset detected!
            threshold1 = rms;
    }
}
//_________
void ofApp::keyPressed(int key) {
    
    if(key=='f'){
        ofSaveScreen("bingo.jpg");
    
    }
    if(key == 'z') {
        frequencyTarget = 261.63; // C
    } else if(key == 'x') {
        frequencyTarget = 293.67; // D
    } else if(key == 'c') {
        frequencyTarget = 329.63; // E
    } else if(key == 'v') {
        frequencyTarget = 349.23; // F
    } else if(key == 'b') {
        frequencyTarget = 392.00; // G
    } else if(key == 'n') {
        frequencyTarget = 440.00; // A
    } else if(key == 'm') {
        frequencyTarget = 493.88; // B
    } else if(key == 'q') {
        frequencyTarget = 30;
    } else if(key == 'w') {
        frequencyTarget = ofRandom(0,100);
    } else if(key == 'e') {
        frequencyTarget = ofRandom(200,2000);
    } else if(key == 'r') {
        frequencyTarget = ofRandom(2001,2400);
    } else if(key == 't') {
        frequencyTarget = ofRandom(100,200);
    } else if (key=='o'){
        for(int o=0;o<ofMap(ofGetHeight(), 0, 1024,200,6000);o++)
        {   frequencyTarget=ofGetMouseX()*o;
        }
    }
    else if (key=='p'){
        for(int p=0;p<10000;p++)
        {
            int targetP= ofMap(ofGetWidth(), 0, 1024, 0, 10000);
            frequencyTarget=ofRandom(targetP, targetP*p);
        }
    }
    else if (key=='a'){
        for(int a=0; a<40; a++)
        {   frequencyTarget= sin(phase)*a;
        }
            
    
    
 

        //SUCCESSFUL PITCH MAP is A ^
    }
    

}
