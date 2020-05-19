#include "adxl.h"
#include "mbed.h"

SPI acc(p11,p12,p13); //MOSI, MISO, SCL

DigitalOut cs(p14); //chip select

void adxl_init() 
{
    cs = 1;  //deactivate the ADXL345
    
    acc.format(8,3); //set the ADXL to 8 bit data, Mode 3
    acc.frequency(2000000); //set 2MHz clock rate
    
    cs = 0; //select the ADXL
    
    acc.write(0x31); //write to the data format register
    acc.write(0x0B); //format +/-16g, 0.004g/LSB
    
    cs = 1; //deactivate the ADXL345
    cs = 0; //select the ADXL
    
    acc.write(0x2D); //power up the device in stanby mode
    acc.write(0x08); //set to measure mode
    
    cs = 1; //deactivate the ADXL345
}

void adxl_readings(int *data) //creating a function 
{
    char buffer[6]; //creating a char variable for the raw data
    
    cs = 0;//select the ADXL
    
    acc.write(0x80|0x40|0x32); //set RW bit high, MB bit high and plus the address
    
    //iterrate 5 times
    for(int i = 0; i<=5; i++)
    {  
        buffer[i] = acc.write(0x00); //read back 6 data bytes
    }
        
    cs = 1; //deactivate the ADXL345
    
    //combine MSB and LSB for each data point using bitwise shift OR
    data[0] = buffer[1] << 8 | buffer[0];
    data[1] = buffer[3] << 8 | buffer[2];
    data[2] = buffer[5] << 8 | buffer[4];   
}