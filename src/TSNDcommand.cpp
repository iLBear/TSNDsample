//
//  TSNDcommand.cpp
//  Created by iLBe@r on 2013/01/25.
//

#include "TSNDcommand.h"

void TSNDcommand::makeBCC(Byte *sendData){
    sendData[commandLength-1] = 0x00;
    for(int i = 0; i < commandLength-1; i++){
        sendData[commandLength-1] = (sendData[commandLength-1] ^ sendData[i]);	//BCC
    }
}

void TSNDcommand::stopMeasure(int fd){
    commandLength = 4;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;
    sendData[1] = 0x15;
    sendData[2] = 0x00;
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x15]: stopMeasure\n");
}

void TSNDcommand::getTime(int fd){
    commandLength = 4;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;
    sendData[1] = 0x12;
    sendData[2] = 0x00;
    makeBCC(sendData);
    if(write(fd, sendData, commandLength) < 0){
        printf("err\n");  
    }else{
        printf("- send[0x12]: getTime\n");
    }
}

void TSNDcommand::startMeasure(int fd, int measure_sec){
    commandLength = 17;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;         //Header (0x9a fixed) [1 byte]
    sendData[1] = 0x13;         //Command Code		  [1 byte]
    sendData[2] = 0x00;         //相対(0) or 絶対時刻(1)
    sendData[3] = 0x0c;         //開始年
    sendData[4] = 0x0c;         //開始月
    sendData[5] = 0x01;         //開始日
    sendData[6] = 0x00;         //開始時
    sendData[7] = 0x00;         //開始分
    sendData[8] = 0x00;         //開始秒
    sendData[9] = 0x00;         //相対(0) or 絶対時刻(1)
    sendData[10] = 0x0c;        //終了年
    sendData[11] = 0x0c;        //終了月
    sendData[12] = 0x01;        //終了日
    sendData[13] = 0x00;        //終了時
    sendData[14] = 0x00;        //終了分
    sendData[15] = measure_sec; //終了秒
    sendData[16] = 0x00;
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x13]: startMeasure\n");
}

void TSNDcommand::setAccelMeasurement(int fd, int cycle_ms, int average, int recordAverage){
    if(cycle_ms < 0 || cycle_ms > 255)              cycle_ms = 20;
    if(average < 0 || average > 255)                average = 10;
    if(recordAverage < 0 || recordAverage > 255)	recordAverage = 10;
    
    commandLength = 6;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;             //Header (0x9a fixed)		[1 byte]
    sendData[1] = 0x16;             //Command Code				[1 byte]
    sendData[2] = cycle_ms;         //計測周期 					[1-255ms]
    sendData[3] = average;          //計測データ送信設定・平均回数 	[1-255回]
    sendData[4] = recordAverage;	//データ記録時平均回数設定 		[0:しない, 1-255:する[回]]
    sendData[5] = 0x00;
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x16]: setAccelMeasurement\n");
}

//地磁気計測設定
void TSNDcommand::setGeometricMeasurement(int fd, int cycle_ms, int average, int recordAverage){
    if((cycle_ms != 0 && cycle_ms < 10) || cycle_ms > 255)  cycle_ms = 20;
    if(average < 0 || average > 255)                        average = 10;
    if(recordAverage < 0 || recordAverage > 255)            recordAverage = 10;
    
    commandLength = 6;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;             //Header (0x9a fixed) 		[1 byte]
    sendData[1] = 0x18;             //Command Code		  		[1 byte]
    sendData[2] = cycle_ms;         //計測周期 					[0-255ms]
    sendData[3] = average;          //計測データ送信設定・平均回数 	[1-255回]
    sendData[4] = recordAverage;	//データ記録設定 				[0:しない, 1-255:する[回]]
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x18]: setGeometricMeasurement\n");
}

//気圧計測設定（計測周期[ミリ秒]，データ平均回数[回]）
void TSNDcommand::setPressureMeasurement(int fd, int cycle_ms, int average, int recordAverage){
    if((cycle_ms != 0 && cycle_ms < 4) || cycle_ms > 255)   cycle_ms = 40;
    if(average < 0 || average > 255)                        average = 10;
    if(recordAverage < 0 || recordAverage > 255)            recordAverage = 10;
    
    commandLength = 6;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;             //Header (0x9a fixed) 		[1 byte]
    sendData[1] = 0x1a;             //Command Code		  		[1 byte]
    sendData[2] = cycle_ms;         //計測周期÷10 				[40-2550ms]
    sendData[3] = average;          //計測データ送信設定・平均回数 	[1-255回]
    sendData[4] = recordAverage;	//データ記録設定 				[0:しない]
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x1a]: setPressureMeasurement\n");
}

//オプションボタン押下時の動作設定
void TSNDcommand::setOptionButtonMode(int fd, int mode){
    if(mode < 0 | mode > 4)     mode = 0;
    commandLength = 4;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;             
    sendData[1] = 0x2C;             
    sendData[2] = mode;             //モード[0-4]
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x2C]: setOptionButtonMode\n");
}

//バッテリ電圧計測設定
void TSNDcommand::setBatteryMeasurement(int fd, bool send, bool record){
    commandLength = 5;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;
    sendData[1] = 0x1C;
    /*
     true = 1; false = 0;であるので
     sendData[2] = send; sendData[3] = record;
     でも良いと思われるが，念のため．
     */
    if(send){
        sendData[2] = 0x01;
    }else{
        sendData[2] = 0x00;
    }
    if(record){
        sendData[3] = 0x01;
    }else{
        sendData[3] = 0x00;
    }
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x1C]: setBatteryMeasurement\n");
}

//バッテリ残量取得
void TSNDcommand::getBatteryRemain(int fd){
    commandLength = 4;
    Byte sendData[commandLength];
    sendData[0] = 0x9a;
    sendData[1] = 0x3B;
    sendData[2] = 0x00;             //モード[0-4]
    makeBCC(sendData);
    write(fd, sendData, commandLength);
    printf("- send[0x3B]: getBatteryRemain\n");
}