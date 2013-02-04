#include "testApp.h"
#include "inttypes.h"
//test from MBA


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
//    int32_t hh = 0xfffffeff;
//    int gg;
//	int32_t ff = -257;
//    const char *str = "0xfffffeff";
//    char *ch;
//	printf("-257:%#x\n", ff);
//	printf("hh:%d\n", hh);
//	gg = strtoimax(str, &ch, 16);
//	printf("strtoimax:%d\n", gg);
//	
//	gg = strtol(str, &ch, 16);//上手く変換されない
//   printf("strtol:%d\n", gg+1);
	
	
//	printf("size:%ld\n", sizeof(gg));
//	printf("\n   int:%ld\n", sizeof(int));
//	printf(" int32:%ld\n", sizeof(int32_t));
//	printf(" int64:%ld\n", sizeof(int64_t));
//	printf("  long:%ld\n", sizeof(long));
//	printf(" long2:%ld\n", sizeof(long long));
//	printf("double:%ld\n", sizeof(double));
//	printf("\n intMax:%d\n", INT_MAX);
//	printf("longMax:%ld\n", LONG_MAX);
//	printf("longlongMax:%lld\n", LONG_LONG_MAX);
//   printf("%d\n", ~gg);
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
	
	/* 加速度設定を取得する為にメッセージを送信 */
	sleep(2);	//コマンドが高確率で届くように，接続してから数秒待つ
	TSND.command.getAccelMeasurement(fd);
    
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
        char str[10];
        char *ch;
        
        /* 以下メッセージの中身を解析 */
    if(nbytes > 0){
        switch(buffer[1]){
            case 0x80:  //加速度・角速度計測メッセージ
//                for(int i = 0; i < 3; i++){
//                    if(buffer[6+(3*i)] >= 0x80){
//                        buffer[6+(3*i)] ^= 0xff;    //0~255を128~0, 0~127に変換
//                    }
//                    if(buffer[7+(3*i)] >= 0x80){
//                        buffer[7+(3*i)] ^= 0xff;
//                    }
//                    if(buffer[8+(3*i)] >= 0xF0){    //最上位4ビットが1ならマイナス（にした）
//						TSND.accel[i] = -(256*buffer[7+(3*i)]+buffer[6+(3*i)]);
//                    }else{
//						TSND.accel[i] = (256*buffer[7+(3*i)]+buffer[6+(3*i)]);
//                    }
//					TSND.rotate[i] = TSND.accel[i]*0.009;	//-10k~10kの値を取るらしいので,0.009を掛けることにより-90~90[deg]に変換した
//                }
	
				for(int i = 0; i < 3; i++){
                    if(buffer[8+(3*i)] >= 0xF0){    //最上位4ビットが1ならマイナス（にした）
						sprintf(str, "0xff%02x%02x%02x", buffer[8+(3*i)], buffer[7+(3*i)], buffer[6+(3*i)]);
                    }else{
						sprintf(str, "0x00%02x%02x%02x", buffer[8+(3*i)], buffer[7+(3*i)], buffer[6+(3*i)]);
                    }
					TSND.accel[i] = strtoimax(str, &ch, 16);
					TSND.rotate[i] = TSND.accel[i]*0.009;	//-10k~10kの値を取るらしいので,0.009を掛けることにより-90~90[deg]に変換した
                }

				
				//z軸方向の移動距離
				TSND.calcDistFromAccel('z');
				
//              printf("X:0x%02x%02x%02x\n", buffer[8], buffer[7], buffer[6]);
//				printf("Y:0x%02x%02x%02x\n", buffer[11], buffer[10], buffer[9]);
//				printf("Z:0x%02x%02x%02x\n", buffer[14], buffer[13], buffer[12]);
//              sprintf(str, "0x%02x%02x%02x", buffer[8], buffer[7], buffer[6]);
                printf("[acc]x:%3d, y:%3d, z;%3d\n", TSND.accel[0], TSND.accel[1], TSND.accel[2]);
//				printf("[rot]x:%3f, y:%3f, z;%3f\n", TSND.rotate[0], TSND.rotate[1], TSND.rotate[2]);
//				printf("            dist_z:%.3f, v0:%.3f\n", TSND.distance[2], TSND.velocity[2]);
				TSND.height_delta += TSND.distance[2];
                break;
                
            case 0x82:  //気圧計速メッセージ
                if(TSND.initializeP){
                    TSND.initPressure = 256*256*buffer[8]+256*buffer[7]+buffer[6];
                    TSND.initializeP = false;
                }
                printf("Msg[0x%02x%02x%02x]\n", buffer[8], buffer[7], buffer[6]);
                TSND.pressure = 256*256*buffer[8]+256*buffer[7]+buffer[6];
                
                TSND.height = 44330.77*(1.0-pow((TSND.pressure/101325.0), 0.1902632)); //海抜からの高度算出
                
                printf(" Pressure:%d[Pa] delta:%d\n", TSND.pressure, TSND.initPressure-TSND.pressure);
                printf(" Height:%.3f[m]\n", TSND.height);
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
                isMeasuring = false;
                printf("Stop measuring.\n");
                break;
                
            case 0x8f:  //コマンドレスポンス
                if(buffer[2] == 0x00){
                    printf(" -> OK!\n");
                }else{  //0x01
                    printf(" -> NG\n");
                }
                break;
                
            case 0x92:  //現在時刻メッセージ
                printf("<%4d/%02d/%02d %02d:%02d:%02d>\n", buffer[2]+2000, buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
                break;
                
            case 0x93:  //計測時刻応答
//                printf("stop measuring\n");
                break;
                
			case 0x97:
				TSND.setAccelCycle(buffer[2]);
				TSND.setAccelAverage(buffer[3]);
				printf("=== set accel Cy:%d, Av:%d\n", buffer[2], buffer[3]);
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
	
	ofDrawBitmapString("Height_delta"+ofToString(TSND.height_delta, 3)+"[cm]", 20,60);
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    ofRotateZ(TSND.rotate[0]);
    ofSetColor(255);
    ofLine(-ofGetWidth()/2, 0, ofGetWidth()/2, 0);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){
    TSND.command.stopMeasure(fd);
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
            TSND.command.getTime(fd);
            break;
            
        case OF_KEY_RETURN:
            if(isMeasuring){    //計測停止
                isMeasuring = false;
                TSND.command.stopMeasure(fd);
            }else{              //計測開始
                TSND.initializeP = true;
                isMeasuring = true;
				TSND.height_delta = 0;
                TSND.command.startMeasure(fd, 10);
            }
            break;
            
        case ' ':
            break;
            
        case 'o':
            TSND.command.setOptionButtonMode(fd, 3);
            break;
            
        case 'b':
            TSND.command.getBatteryRemain(fd);
            break;
            
        case 'B':
            TSND.command.setBatteryMeasurement(fd, false, false);
            break;
            
        case 'a':   //加速度計測設定ON
            TSND.command.setAccelMeasurement(fd, 5, 10, 0);
			TSND.command.getAccelMeasurement(fd);
            break;
        case 'A':   //加速度計測設定OFF
            TSND.command.setAccelMeasurement(fd, 0, 10, 0);
            break;
            
        case 'g':   //地磁気計測設定ON
            TSND.command.setGeometricMeasurement(fd, 10, 10, 0);
            break;
        case 'G':   //地磁気計測設定OFF
            TSND.command.setGeometricMeasurement(fd, 0, 10, 0);
            break;
            
        case 'p':   //気圧計測設定ON
            TSND.initializeP = true;
            TSND.command.setPressureMeasurement(fd, 4, 5, 0);
            break;
        case 'P':   //気圧計測設定OFF
            TSND.initializeP = false;
            TSND.command.setPressureMeasurement(fd, 0, 5, 0);
            break;
			
		case 'r':
			TSND.command.setAccelRange(fd, 0);
			break;
			
		case 'c':
			TSND.command.collectAccelMeasurement(fd, 1, 1, 1, 0, 0, 0);
			break;
			
		case 'z':
			TSND.command.setBuzzerVolume(fd, 0);
			break;
			
		case '1':
			TSND.command.playBuzzer(fd, 1);
			break;
		case '2':
			TSND.command.playBuzzer(fd, 2);
			break;
		case '3':
			TSND.command.playBuzzer(fd, 3);
			break;
		case '4':
			TSND.command.playBuzzer(fd, 4);
			break;
		case '5':
			TSND.command.playBuzzer(fd, 5);
			break;
		case '6':
			TSND.command.playBuzzer(fd, 6);
			break;
		case '7':
			TSND.command.playBuzzer(fd, 7);
			break;
		case '0':
			TSND.command.playBuzzer(fd, 0);
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