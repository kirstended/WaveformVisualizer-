#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    // Set background.
    ofBackground(ofColor::black);
    
    // Setup the video player.
    grabber.setup(ofGetWidth()/2,ofGetHeight()/2);

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
    soundPlayer.setMultiPlay(true);
    
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
    
    //This is for the text
    text="";
    position=0;
    cursorx=0;
    cursory=0;

 


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

    
    frequency = ofLerp(frequency, frequencyTarget, 0.4);
    
 
    if(ofGetKeyPressed()) {
        volume= ofLerp(volume, 1, 0.3); // jump quickly to 1
    } else {
        volume= ofLerp(volume, 0, 0.1); // fade slowly to 0
    }
    
    ofScopedLock waveformLock(waveformMutex);
    updateWaveform(ofMap(ofGetMouseX(), 0, ofGetWidth(), 3, 150, true));
}

    //for envelope generator; key is pressed affecting attack/decay
    //ofLerp= float start, float stop, and float amount;
   


//--------------------------------------------------------------
void ofApp::draw(){
    //for bitmaptext
    ofPushMatrix();
    drawText();
    ofPopMatrix();
    
        // Reset the color of grabber texture to original color.
    ofSetColor(ofColor::white);
    
    ofDrawBitmapStringHighlight("Mouse Click= Initialize Sound", grabber.getWidth()+10,10);
    ofDrawBitmapStringHighlight("Top row of keyboard letters= Play Sound Samples", grabber.getWidth()+10,30);
    ofDrawBitmapStringHighlight("Spacebar=Stop Sample", grabber.getWidth()+10,50);
    ofDrawBitmapStringHighlight("Bottom 2 rows of keyboard letters =MIDI NOTES", grabber.getWidth()+10,70);
    ofDrawBitmapStringHighlight("Shift+Key= Sustain", grabber.getWidth()+10,90);
    ofDrawBitmapStringHighlight("MouseMoved= Changes Frequency Bands", grabber.getWidth()+10,110);

    
//    QWERTYUIOP Keys= Play Sound Samples; Spacebar=Stop Sound Samples; ASDFGHJKLZXCVBNM=MIDI NOTES; Shift+KeyPress= Hold Note; MouseMoved=Changes Frequency Bands", grabber.getWidth()+10,10);
    
    for (auto position: outLine)
    {
        if (position.x < grabber.getWidth())
        {
            float subsectionWidth = 4;
            
            grabber.getTexture().drawSubsection((int)position.x,
                                                position.y - ofGetHeight() / 2,
                                                subsectionWidth,
                                                grabber.getTexture().getHeight(),
                                                (int)position.x,
                                                0,
                                                subsectionWidth,
                                                grabber
                                                .getTexture().getHeight()
                                                );
        }
// Translating the waveform line
        float lineVariable= ofMap(frequencyTarget,0,3000,5,50);
        float colorVariable= ofRandom(0,ofMap(frequencyTarget,0,3000,0,255));
        float hueVariable= ofMap(frequencyTarget,40,3000,0,255);
        ofSetLineWidth(1);
        ofSetColor(ofColor::fromHsb(hueVariable, colorVariable, 245));
        
    if (ofGetKeyPressed())
    {
        for (int a=0; a<ofGetWidth(); a++)
        {
            ofDrawLine(1+(a*2), grabber.getHeight(), 1+ (a*2) , grabber.getHeight()-scaledVol * 500.0f);
    }
        
    
    float value = 0.2;
    
    for (auto point: outLine)
    {
        ofDrawCircle(point, 6);
        
    }

    
    }
}
//VOID FOR BITMAP___________________________________________________________
}
void ofApp::drawText() {
    
    
    
    float charWidth = 8.5;
    float desiredLineWidth = ofGetWidth();
    int numCharsPerLine = desiredLineWidth / charWidth;
    float charHeight=10;
    float desiredLineHeight= ofGetHeight()/2;
    int numCharsLength= desiredLineHeight/charHeight;
    std::string brokenText = "";
    
    
    int numLines = 0;
    
    for (std::size_t i = 0; i < text.size(); i++)
    {
        if ((i % numCharsPerLine) == 0)
        {
            brokenText += "\n";
            numLines++;
        }
        
        if (text[i] == '\n')
        {
            numLines++;
        }
        
        brokenText += text[i];
        
        if (numLines > 5)
        {
            text = "";
            position = 0;
        }
    }
    
  

    float colorVariable= ofRandom(0,ofMap(frequencyTarget,0,3000,0,255));
    ofSetColor(ofColor::fromHsb(colorVariable, 255, 245));
    ofDrawBitmapString(brokenText, 10,grabber.getHeight()+20);



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
    
    for (int i = 0; i < input.getNumFrames(); i++){
        left[i]		= input[i*2]*0.5;
        right[i]	= input[i*2+1]*0.5;
        
        curVol += left[i] * left[i];
        curVol += right[i] * right[i];
        numCounted+=2;
    }
    
    curVol /= (float)numCounted;
    
    curVol = sqrt( curVol );
    
    smoothedVol *= 0.93;
    smoothedVol += 0.07 * curVol;
    
    bufferCounter++;
    
}

//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    
        ofScopedLock waveformLock(waveformMutex);
        
        float sampleRate = 44100;
        float phaseStep = frequency / sampleRate;
        
        outLine.clear();
    
        float alpha = 0.999;
        float ySmooth = ofMap(0, -1.5, 2, 0, ofGetHeight());
    
        for(int i = 0; i < bufferSize * nChannels; i += nChannels)
        {
            phase += phaseStep;
            int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
            //This below line is specifically for envelope generator
            
            output[i] = waveform[waveformIndex] * volume;
            
            ySmooth = (1 - alpha) * ofMap(output[i], -1, 1, 0, ofGetHeight()) + (alpha) * ySmooth;
            
            outLine.addVertex(ofMap(i, 0, bufferSize-1 , 0, ofGetWidth()),
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
    typeKey(key);
    
    ofSetColor(ofColor::white);
    
//Top Row
    if(key == 'q') {
    frequencyTarget = 55; //A1
    } else if(key == 'w') { 
    frequencyTarget = ofRandom(150);
    
    } else if(key == 'e') {
        frequencyTarget = 65.406; //C2
    } else if(key == 'r') {
        frequencyTarget = 73.416; //D2
    } else if(key == 't') {
        frequencyTarget = 82.407; //E2
    } else if(key == 'y') {
        frequencyTarget = 87.307; //F2
    } else if(key== 'u') {
        frequencyTarget= 97.999; //G2
    } else if( key== 'i') {
        frequencyTarget= 110.000; //A2
    } else if (key=='o'){
            for(int o=0;o<ofMap(ofGetHeight(), 0, 1024,200,6000);o++)
            {   frequencyTarget=ofGetMouseX()*o;}
    } else if (key=='p'){
            for(int p=0;p<6000;p++)
            { int targetP= ofMap(ofGetWidth(), 0, ofGetWidth(), 0, 6000);
                frequencyTarget=ofRandom(300, targetP);
            }
//middle row
        
    } else if (key=='a'){
        frequencyTarget = 246.94; //B3
        }
     else if(key== 's') {
        frequencyTarget= 261.63; //C4
    
    } else if(key== 'd') {
        frequencyTarget= 293.67; //D4
    }else if(key=='f'){
        ofSaveScreen("bingo.jpg");
        frequencyTarget=329.63;   //E4
    } else if(key== 'g') {
        frequencyTarget= 349.23; // F4
    } else if(key== 'h') {
        frequencyTarget= 392; //G4
    } else if(key== 'j') {
        frequencyTarget= 440; //A4
    } else if(key== 'k') {
        frequencyTarget= 493; //B4
    } else if(key== 'l') {
        frequencyTarget= 523.25; //C5
    }
    else if (key ==' ' and soundPlayer.isPlaying())
    {
        soundPlayer.stop();
    }
        
    
//Bottom Row
    else if(key == 'z') {
        soundPlayer.loadSound("Ripping the knife apart.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
        // C
    } else if(key == 'x') {
        soundPlayer.loadSound("kidpixinstaller.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
    } else if(key == 'c') {
        soundPlayer.loadSound("mix1.2.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
    } else if(key == 'v') {
        soundPlayer.loadSound("oopsie.beat1.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
    } else if(key == 'b') {
        soundPlayer.loadSound("patticake.1.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
    } else if(key == 'n') {
        soundPlayer.loadSound("chirping.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();
    } else if(key == 'm') {
        soundPlayer.loadSound("musicbox.mp3");
        soundPlayer.setVolume(frequencyTarget);
        soundPlayer.play();

    }
    
 
    }
//_________BITMAP KEYS____________________

void ofApp::typeKey(int key) {
    //add charachter
    
    
    if (key >=32 && key <=126) {
        text.insert(text.begin()+position, key);
        position++;
    }
    
    
    if (key==OF_KEY_RETURN) {
        text.insert(text.begin()+position, '\n');
        position++;
    }
    
    if (key==OF_KEY_BACKSPACE) {
        if (position>0) {
            text.erase(text.begin()+position-1);
            --position;
        }
    }
    
    if (key==OF_KEY_DEL) {
        if (text.size() > position) {
            text.erase(text.begin()+position);
        }
    }
    
    if (key==OF_KEY_LEFT)
        if (position>0)
            --position;
    
    if (key==OF_KEY_RIGHT)
        if (position<text.size()+1)
            ++position;



    
    //for multiline:
    cursorx = cursory = 0;
    for (int i=0; i<position; ++i) {
        if (*(text.begin()+i) == '\n') {
            ++cursory;
            cursorx = 0;
        } else {
            cursorx++;
        }
    }
    
}

    


