#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
        void update();
		void draw();
        void updateWaveform(int waveformResolution);
        void audioIn(ofSoundBuffer & input);
        void audioIn(float * input, int bufferSize, int nChannels);

        void audioOut(float * output, int bufferSize, int nChannels);
//        void audioOut(ofSoundBuffer &outBuffer);
        void keyPressed(int key);
    
        std::vector<float> waveform;// this is the lookup table
        
        double phase;
    
        ofSoundStream soundStream;
    
        vector <float> left;
        vector <float> right;
        vector <float> volHistory;
    
        int 	bufferCounter;
        int 	drawCounter;
    
        float smoothedVol;
        float scaledVol;
    


    
        float volume;
        float frequency;
        float frequencyTarget;
    
        ofMutex waveformMutex;
        ofPolyline waveLine;
        ofPolyline outLine;
        float threshold1;
        float minimumThreshold;
        float decayRate;
  
    ofTexture texture;
    ofTexture tex1;
    
    std::vector<ofTexture>textures;
    
    
    glm::vec3 boundingSize;
    
    
    ofSoundPlayer soundPlayer;
    ofVideoGrabber grabber;
    ofPixels grabberPixels;
    ofTexture grabberTexture;
    ofTexture newTexture;
 
    

    
    int threshold= 100;
    
    uint64_t time=0;
    ofTrueTypeFont frabk;
    
    ofImage image;
    
    void	drawText();
    void	typeKey(int key);
    string	text;
    int		position;
    int		cursorx, cursory;
    
    
     
    
        
    
};
