#include "testApp.h"

int testApp::open_port(){
    fd = open("/dev/tty.TSND121_BT", O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1){
        perror("open_port: unable to open port");
        ofExit();
    }else{
        fcntl(fd, F_SETFL, 0);
    }
    
    return (fd);
}

//--------------------------------------------------------------
void testApp::setup(){
    /* byte型の確認 */
//    char ch = 1;
//    printf("   0:%x\n", 0);
//    printf("   1:%x\n", 1);
//    printf(" 126:%x\n", 126);
//    printf(" 127:%x\n", 127);
//    printf(" 128:%x\n", 128);
//    printf(" 129:%x\n", 129);
//    printf("  -1:%x\n", -1);
//    printf("  -2:%x\n", -2);
//    printf("-126:%x\n", -126);
//    printf("-127:%x\n", -127);
//    printf("-128:%x\n", -128);
//    printf("-255:%x\n", -255);
//    long hh = 0xfffffeff;
//    long gg;
//    const char *str = "0xfffffeff";
//    char *ch;
//   printf("-257:%#x\n", -257);
//   printf("hh:%ld\n", hh);
//   gg = strtol(str, &ch, 16);  //なぜかunsigned longに変換されてしまう
//   printf("lu:%lu\n", gg);
//   printf("ld:%ld\n", gg);
//   printf("%ld\n", ~gg);
//   printf("t:%d ", true);
//    printf("f:%d\n", false);
    
    
    /* OFの設定 */
    ofSetFrameRate(30);
    ofBackground(0);
    
    /*ポートのオープン*/
    open_port();
    printf("CONNECTED.\n\n");
    
    /* ポートのカレントのオプションを取得 */
    tcgetattr(fd, &options);
    
    /* ボーレートを設定 */
    cfsetispeed(&options, B57600);  //input speed
    cfsetospeed(&options, B57600);  //output speed
    
    /* レシーバの有効化およびローカルモードの設定 */
    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_cc[VMIN]   = 0;
    options.c_cc[VTIME]  = 10;      //タイムアウト10ms
    
    /* ポートの新しいオプションの設定 */
    tcsetattr(fd, TCSANOW, &options);
    
}

//--------------------------------------------------------------
void testApp::update(){
//    if(isReadMsg){
        bufptr = buffer;    //無駄な操作かもしれぬ
        //パケット読み取り．一気に送られてくるから1回呼び出しでOK
        nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1);
        
        //bufferに溜まった分(nbytes)だけループを回してメッセージ内容表示
//        for(int i = 0; i < nbytes; i++){
//            printf("%2d: %x\n", i, buffer[i]);
//        }
//        char str[10];
//        char *ch;
        
        /* 以下メッセージの中身を解析 */
    if(nbytes > 0){
        switch(buffer[1]){
            case 0x80:  //加速度・角速度計測メッセージ
                for(int i = 0; i < 2; i++){
                    if(buffer[6+(3*i)] >= 0x80){
                        buffer[6+(3*i)] ^= 0xff;    //0~255を128~0, 0~127に変換
                    }
                    if(buffer[7+(3*i)] >= 0x80){
                        buffer[7+(3*i)] ^= 0xff;
                    }
                    if(buffer[8+(3*i)] >= 0xF0){    //最上位4ビットが1ならマイナス（にした）
                        rotate[i] = -(256*buffer[7+(3*i)]+buffer[6+(3*i)])*0.009;   //-10k~10kの値を取るらしいので,0.009を掛けることにより-90~90[deg]に変換した
                    }else{
                        rotate[i] = (256*buffer[7+(3*i)]+buffer[6+(3*i)])*0.009;
                    }
                }

//                printf("X:0x%02x%02x%02x\n", buffer[8], buffer[7], buffer[6]);
//                sprintf(str, "0x%02x%02x%02x", buffer[8], buffer[7], buffer[6]);
//                printf("%s\n", str);
//                printf("%ld\n", strtol(str, &ch, 16));　//なぜかunsigned longに変換されてしまう
                printf("x:%3d, y:%3d\n", rotate[0], rotate[1]);
                break;
                
            case 0x82:  //気圧計速メッセージ
                if(initializeP){
                    initPressure = 256*256*buffer[8]+256*buffer[7]+buffer[6];
                    initializeP = false;
                }
                printf("Msg[0x%02x%02x%02x]\n", buffer[8], buffer[7], buffer[6]);
                pressure = 256*256*buffer[8]+256*buffer[7]+buffer[6];
                
                height = 44330.77*(1.0-pow((pressure/101325.0), 0.1902632)); //海抜からの高度算出
                
                printf(" Pressure:%d[Pa] delta:%d\n", pressure, initPressure-pressure);
                printf(" Height:%.3f[m]\n", height);
                break;
            
            case 0x85:  //エッジ検出（オプションボタン押下など）
                if(buffer[8] == 0x01){
                    printf("Pressed button.\n");
                }
                break;
            
            case 0x87:  //計測エラー通知
                switch (buffer[6]) {
                    case 0x80:
                        messageStr = "Acceleration";
                        break;
                        
                    case 0x81:
                        messageStr = "Geomagnetism";
                        break;
                        
                    case 0x82:
                        messageStr = "Pressure";
                        break;
                        
                    case 0x86:
                        messageStr = "Extern I2C";
                        break;
                        
                    default:
                        messageStr = "UNDEFINED";
                        break;
                }
                printf("Error occured in measuring %s.\n", messageStr.c_str());
                break;
                
            case 0x88:  //計測開始通知
                printf("Start measuring.\n");
                break;
                
            case 0x89:  //計測終了通知
                isReadMsg = false;
                isMeasuring = false;
                printf("Stop measuring.\n");
                break;
                
            case 0x8f:  //コマンドレスポンス
                if(buffer[2] == 0x00){
                    printf(" -> OK!\n");
                }else{  //0x01
                    printf(" -> NG\n");
                }
                isReadMsg = false;
                break;
                
            case 0x92:  //現在時刻メッセージ
                printf("<%4d/%02d/%02d %02d:%02d:%02d>\n", buffer[2]+2000, buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
                isReadMsg = false;
                break;
                
            case 0x93:  //計測時刻応答
//                printf("stop measuring\n");
                break;
                
            case 0xbb:  //計測時刻応答
                printf("Battery %d%%\n", buffer[4]);
                break;
                
            default:
                printf("Got undefined command:%#02x\n", buffer[1]);
                break;
        }
    }
//    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255);
    if(isMeasuring){
        ofDrawBitmapString("isMeasuring:true", 0,15);
    }else{
        ofDrawBitmapString("isMeasuring:false", 0,15);
    }
    if(isReadMsg){
        ofDrawBitmapString("isReadMsg:true", 0,30);
    }else{
        ofDrawBitmapString("isReadMsg:false", 0,30);
    }
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateZ(rotate[0]);
    ofSetColor(255);
    ofLine(-ofGetWidth()/2, 0, ofGetWidth()/2, 0);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){
    TSNDcmd.stopMeasure(fd);
    printf("\nEXIT.\n");
    close(fd);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    int cmdLength = 0;
//    if(!isMeasuring){
//        isReadMsg = true;
//    }
    
    switch(key){
        case 't':   //現在時刻取得
            TSNDcmd.getTime(fd);
            break;
            
        case OF_KEY_RETURN:
            if(isMeasuring){    //計測停止
                isMeasuring = false;
                TSNDcmd.stopMeasure(fd);
            }else{              //計測開始
                initializeP = true;
                isMeasuring = true;
                TSNDcmd.startMeasure(fd, 10);
            }
            break;
            
        case ' ':
            break;
            
        case 'o':
            TSNDcmd.setOptionButtonMode(fd, 3);
            break;
            
        case 'b':
            TSNDcmd.getBatteryRemain(fd);
            break;
            
        case 'B':
            TSNDcmd.setBatteryMeasurement(fd, false, false);
            break;
            
        case 'a':   //加速度計測設定ON
            TSNDcmd.setAccelMeasurement(fd, 5, 10, 0);
            break;
        case 'A':   //加速度計測設定OFF
            TSNDcmd.setAccelMeasurement(fd, 0, 10, 0);
            break;
            
        case 'g':   //地磁気計測設定ON
            TSNDcmd.setGeometricMeasurement(fd, 10, 10, 0);
            break;
        case 'G':   //地磁気計測設定OFF
            TSNDcmd.setGeometricMeasurement(fd, 0, 10, 0);
            break;
            
        case 'p':   //気圧計測設定ON
            initializeP = true;
            TSNDcmd.setPressureMeasurement(fd, 4, 5, 0);
            break;
        case 'P':   //気圧計測設定OFF
            initializeP = false;
            TSNDcmd.setPressureMeasurement(fd, 0, 5, 0);
            break;
            
        default:
//            isReadMsg = false;
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}