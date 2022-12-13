
#include <M5Core2.h>
#include <Arduino.h>
#include <M5GFX.h>


//Library for bluetooth feature
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

 //Libraries for SD card opening 
#include <SPI.h>
#include <SD.h>

#include <string>

//Define global variables
int count = 0;
int vol=0;


RTC_TimeTypeDef TimeStruct;//Retrival of time in terms of seconds, minutes, hours 

void setup()
{
  M5.begin();
  M5.IMU.Init();//Gets the acceleration of device and initializes it
  SerialBT.begin("M5STACK"); // Initializes the bluetooth device and names it M5STACK
  M5.Axp.SetLDOEnable(3,false);// set the vibration motor to initially off
  printUI(); //Prints the main screen background
  //timesetup(); //This code is used only once to set the time, then RTC normalizes ad continues with that time
  printAskForHelpButton();//code for "ask for help" button
  printSchedule(); //Prints the schedule from the SD card
}

void loop()
{
  M5.update();
  M5.Rtc.GetTime(&TimeStruct); // gets the time that is stored within rtc 
  Printtime(TimeStruct.Hours, TimeStruct.Minutes, TimeStruct.Seconds); //Print time function to print time 
 
  //Check if button A is clicked 
  if(M5.BtnA.pressedFor(5000))
  {
    SerialBT.println("Patient Called for help!!");
    M5.Spk.DingDong(); // Makes a sound in M5stack 
  }
 
  //----------------------------------------------------
  //Acceleration feature 
  float accelerationx = 0;
  float accelerationy = 0;
  float accelerationz = 0;
  M5.IMU.getAccelData(&accelerationx, &accelerationy, &accelerationz); //gets the acceleration of the device and calculates the net 
  float Netacceleration = sqrt(accelerationx * accelerationx + accelerationy * accelerationy + accelerationz * accelerationz);

  File myFile=SD.open("/PI.txt", FILE_READ);
  myFile.readStringUntil('<'); //skips the schedule section and reads patient input for movement
  char movement = myFile.read(); 
  myFile.close();
  if(movement == '0') //if movement is 0(cant move)
  {

  M5.Lcd.setCursor(15,100);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.print("DON'T MOVE(NURSE CAN SEE)"); //print patient cant move 
  if(Netacceleration>2.6) 
  {
    count = count + 1; 
    if (count>8) //if the acceleration was greater than 2.6 for more than 8 times 
    {
      //Inform the nurse 
    SerialBT.println("MOVEMENT DETECTED");
    vibration(100); //call the Vibration function to make the vibration motor starts working
    SerialBT.print("The Time of Detection: ");
    SerialBT.print(TimeStruct.Hours);
    SerialBT.print(":");
    SerialBT.print(TimeStruct.Minutes);
    SerialBT.print(":");
    SerialBT.print(TimeStruct.Seconds);
    for(int i=0; i< 100; i++)
    {
    M5.Spk.DingDong() ;// Makes a sound in M5stack 
    }
    count = 0;
    }
  }
  }
  else if(movement == '1')
  {
  M5.Lcd.setCursor(95,95);
  M5.Lcd.print("You Can Move :)"); //print patient can move  
  }
 
  delay(1);


}

void printUI() //Prints the main screen background
{
  int i(0),j(0);
  while(i<=80)
  {
    while(j<=160)
    {
      M5.Lcd.drawPixel(j,i,RED);
      j++;
    }
    i++;
    j=0;
  }
  
  int l(0), m(161);
  while(l<=80)
  {
    while(m<=320)
    {
      M5.Lcd.drawPixel(m,l,BLUE);
      m++;
    }
    l++;
    m=161;
  }

  int n(81), o(0);
  while(n<=120)
  {
    while(o<=320)
    {
      M5.Lcd.drawPixel(o,n,GREEN);
      o++;
    }
    n++;
    o=0;
  }

  int k(121), p(0);
  while(k<=240)
  {
    while(p<=320)
    {
      M5.Lcd.drawPixel(p,k,WHITE);
      p++;
    }
    k++;
    p=0;
  }
}

void printSchedule()
{
    // Open port communication and check if the SD opening is succesfful, if not print initialization failed
 if (!SD.begin(4)) {
    M5.Lcd.println("initialization failed");
  }  
  
  
  //Open File 
  File myFile=SD.open("/PI.txt", FILE_READ);

  //Move cursor to the white area for schedule
    M5.Lcd.setCursor(0, 121);

  if(myFile)
  {
    String c;
    //Read data from file for Schedule
      c=myFile.readStringUntil('-');//NOTE: Each line is capped to 26 characters multiplied and 6 lines
      M5.Lcd.setTextSize(2.5);
      M5.Lcd.print(c);
      myFile.close();
  }
  else
  {
  M5.Lcd.println("error opening schedule!!");
  }

}

void printAskForHelpButton() //prints the ask for help button
{
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2.5);
  M5.Lcd.setCursor(14,20);
  M5.Lcd.print("Press A to");
  M5.Lcd.setCursor(7,50);
  M5.Lcd.print("call for help"); 
}

void vibration(int b) //Vibration function for the specified number of times
{ 
  M5.Axp.SetLDOEnable(3, true);
  delay(b);
  M5.Axp.SetLDOEnable(3, false);
}
//This is the main print time function used in the loop
void Printtime(int hr, int min, int sec)
{
  M5.Lcd.setTextSize(2.5);
  M5.Lcd.setTextColor(BLACK, BLUE);
  M5.Lcd.drawString(returntime(hr)+returntime(min)+returntime(sec),180,35);
}


String returntime(int time) { // A function to print time when its one digit and when its 2 digit as a string

    String A = "";

    if (time < 10) 
    {
    A= '0'; // display 0 when time less than 10 
    A= A+ String(time)+ ' ';
    }
    else if(time >=10)
    {
      A= A+ String(time) +' ';
    }

    return A;

}

void printBlueBackground() //just a simple function to simplify the code in the setup time by printing blue background alone
{

   int l(0), m(161);
   while(l<=80) 
   {
    while(m<=320)
    {
      M5.Lcd.drawPixel(m,l,BLUE); // draw the background of the box blue in m5stack
      m++;
    }
    l++;
    m=161;
   }
  
}

void timesetup() //function to set the time in arduiono
{
 int seconds =0;
 int minutes=0;
 int hours=0;
 int hr(0), mins(0), secs(0); 
 M5.Lcd.setTextSize(4);
 while(hr !=-1)
 {
 M5.update();

 if (M5.BtnA.isPressed())
 {
   hours = hours+1; // when A is clicked for 1 second, increment hour by 1

   //Necessary code to keep the background remain blue and same
   printBlueBackground();
   if (hours>23)
   {
     hours = 0;
     printBlueBackground();
   }

 } 

  if (M5.BtnB.isPressed())
 {
   hours = hours-1; // when A is clicked for 1 second, decrement hour by 1
   printBlueBackground();
   if(hours<0)
   {
     hours =23;
     printBlueBackground();
   }
 }

 M5.Lcd.drawString(returntime(hours), 200 , 40 ); //Draw string onto the M5Stack
 
 if(M5.BtnC.isPressed())
 {
   hr=-1;
 }
 
 }

  //Set the same for minutes
 while(mins !=-1)
 {
 M5.update();

 if (M5.BtnA.isPressed())
 {
   minutes = minutes+1; // when A is clicked for 1 second, increment minutes by 1
   printBlueBackground();

   if (minutes>59) // If minutes exceeds 59, return back to 0
   {
     minutes = 0;
     printBlueBackground();
   }

 } 

  if (M5.BtnB.isPressed())
 {
   minutes = minutes-1; // when A is clicked for 1 second, decrement minutes by 1
   printBlueBackground();
   if(minutes<0)
   {
     minutes =59;
     printBlueBackground();
   }
 }

 M5.Lcd.setCursor(200, 40);
 M5.Lcd.drawString(returntime(minutes),200, 40);

 if(M5.BtnC.pressedFor(5000))
 {
   mins=-1;
 }
 
 }

  //Set the same for seconds
 while(secs !=-1)
 {
 M5.update();

 if (M5.BtnA.isPressed())
 {
   seconds = seconds+1; // when A is clicked for 1 second, increment seconds by 1
   printBlueBackground();
   if (seconds>59)
   {
     seconds = 0;
     printBlueBackground();
   }

 } 

  if (M5.BtnB.isPressed())
 {
   seconds = seconds-1; // when A is clicked for 1 second, decrement seconds by 1
   printBlueBackground();
   if(seconds<0)
   {
     seconds =59;
     printBlueBackground();

   }
 }

 M5.Lcd.drawString(returntime(seconds), 200, 40);

 if(M5.BtnC.pressedFor(8000))
 {
   secs=-1;
 }
 
 }    
  //Assign the values for TimeStruct to the hours, minutes, seconds, provided by the user in M5stack
  TimeStruct.Hours = hours;
  TimeStruct.Minutes = minutes;
  TimeStruct.Seconds = seconds;
  M5.Rtc.SetTime(&TimeStruct);
  printBlueBackground();


}

