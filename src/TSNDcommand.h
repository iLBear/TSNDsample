//
//  TSNDcommand.h
//  Created by iLBe@r on 2013/01/25.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#ifndef TSND_TSNDcommand_h
#define TSND_TSNDcommand_h

class TSNDcommand{
private:
    int commandLength;
    void makeBCC(Byte *sendData);
    
public:
    
    void startMeasure(int fd, int second);
    void stopMeasure(int fd);
    
    void setAccelMeasurement(int fd, int cycle_ms, int average, int recordAverage);
    void setGeometricMeasurement(int fd, int cycle_ms, int average, int recordAverage);
    void setPressureMeasurement(int fd, int cycle_ms, int average, int recordAverage);
    void setOptionButtonMode(int fd, int mode);
    void setBatteryMeasurement(int fd, bool send, bool record);
    
    void getTime(int fd);
    void getBatteryRemain(int fd);
};


#endif
