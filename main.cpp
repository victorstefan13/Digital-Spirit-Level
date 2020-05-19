#include "mbed.h"
#include "TextLCD.h"
#include "adxl.h" //include the header file for adxl.cpp

#define Pi 3.14159265358 //define Pi constant
#define taps 100        // define the number of taps as a constant

//prototype custom functions
void setZero();
void alert();
int movingAvg(int input[taps]);

//define variables, interrupts and GPIOs
TextLCD lcd(p19, p20, p21, p22, p23, p24); // rs, e, d4-d7

InterruptIn zero(p7);

BusOut myled(LED1, LED2, LED3, LED4);

DigitalOut buzzer(p6);

int readings[3], x[taps], y[taps], z[taps]; //arrays used for the moving average filter

double xAngle, yAngle, xAvg, yAvg, zAvg;  // variables used for the angles
static double xCalibration = 0, yCalibration = 0; //variables used for calibration


int main() {

    adxl_init(); //call the adxl_init() function from adxl.cpp file
    
    zero.rise(&setZero); /*declaring the interrupt on the rising edge
                           which calls the setZero function()*/
    
    while(1) { //infinate loop
        
        /*get the readings (taps) number of time, store them into an array
        calls the movingAvg() function which applies a moving average filter
        on the readings */        
        for(int i = 0; i < taps; i++)
        {
            adxl_readings(readings); //call the adxl_readings function from adxl.cpp file
            
            x[i] = (int16_t)readings[0]; xAvg = movingAvg(x); //store the readings into x/y/z 
            y[i] = (int16_t)readings[1]; yAvg = movingAvg(y); //at index i and calculate the 
            z[i] = (int16_t)readings[2]; zAvg = movingAvg(z); //average using movingAvg() function
        }
        
        //convert the readings into angles using arctan function
        xAngle = atan2( xAvg, sqrt((yAvg*yAvg)+(zAvg*zAvg)));
        yAngle = atan2( yAvg, sqrt((xAvg*xAvg)+(zAvg*zAvg)));  
        
        //convert from radians to angles
        xAngle = (xAngle * 180/Pi) - xCalibration;
        yAngle = (yAngle * 180/Pi) - yCalibration;
        
        alert(); //call the alert() function
        
        //display the x angle at row 0, colum 0
        lcd.locate(0,0);
        lcd.printf("X axis: %.1f\n", xAngle);
        //display the y angle at row 0, colum 1   
        lcd.locate(0,1);
        lcd.printf("Y axis %.1f\n", yAngle);   
        
        wait(0.2); //delay for 0.2 seconds
    }
}

void setZero() //creating a function to calibrate the system
{
    wait(0.3); //delay for 0.3 seconds
    xCalibration += xAngle; // xCalibration = xCalibration + xAngle
    yCalibration += yAngle; // yCalibration = yCalibration + yAngle
    wait(0.3); //delay for 0.3 seconds
}

void alert() /*creating a function to alert the user when the 
             reading is above/below 45/-45 degrees*/ 
{
    //if yAngle is bigger or equal to 45 degrees set myled bus to 3(LED1, LED2 on)
    if(yAngle >= 45) myled=3; 
    //else if y and is smaller than -45 degrees set my leed bus to 12(LED3, LED4 on)
    else if(yAngle <= -45) myled=12;
    //if neither set myled bus to 6(LED2, LED3 on)
    else myled=6;   
    //check to see if yAngle or xAngle is above or below 45/-45  and if so, set buzzer high
    if(yAngle <= -45 || yAngle >= 45 || xAngle <= -45 || xAngle >= 45) buzzer=!buzzer;
    else buzzer = 0; //if not set buzzer low
}

int movingAvg(int input[taps]) //creating a function with the argument input of the array input
{
    //declaire variables used within the function 
    int sum = 0;
    double sumAvg = 0;
    
    //iterrate through the number of taps summing the valeus at each index
    for(int j = 0; j<taps; j++)
    {
        sum += input[j];
         
    }
    
    //calculate the average and return sumAvg
    sumAvg = sum / taps; 
    
    return sumAvg;   
} 