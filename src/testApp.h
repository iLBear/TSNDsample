#pragma once

#include "ofMain.h"
#include "TSNDcommand.h"


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
    
    bool isReadMsg = false;
    bool isMeasuring = false;
    
    struct termios options;
    
    TSNDcommand TSNDcmd;
    
    Byte buffer[64];
    Byte *bufptr;
    int nbytes;

    int rotate[3];  //加速度
    int pressure;   //気圧
    
    bool initializeP = false;
    int initPressure;
    float height;
    
    string messageStr;
};
