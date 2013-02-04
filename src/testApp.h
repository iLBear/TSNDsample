#pragma once

#include "ofMain.h"
#include "TSND121.h"

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int open_port();
    
    int fd; //ポートのファイルディスクリプタ
    
    bool isMeasuring = false;	//計測中かどうか
    struct termios options;
    
	TSND121 TSND;
    
    Byte buffer[64];
    Byte *bufptr;
    int nbytes;
    
//    bool initializeP = false;
    
    string messageStr;
};
