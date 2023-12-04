
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>   
#define ONE_WIRE_BUS 60
#define TIME_INTERVAL 1500
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	
// Pass oneWire reference to DallasTemperature library
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas sensors(&dallasTemp);
unsigned long sinceLastTempSamplingTime;


#define SECONDS_IN_DAY 86400UL
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60

struct Config{
  float targetPH;
  float phDownTolerance;
  float phMins;
  float phOffset;
  float phMult;
  int phMinDose;


  float targetEC;
  float ecTolerance;
  float ecMins;
  float ecMult;
  int ecMinDose;

  float waterMins;
  float valveCDMins; 
  float tankSizeLiters; 
};


//LCD and BUTTONS Setup-----------------------------------------------------------------------------------------
static unsigned long sinceLastPrintTime;
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

static unsigned long sinceLastLeftRightKey = 0;
static unsigned long sinceLastUpDownKey = 0;
static unsigned long since2ndLastUpDownKey = 0;
static unsigned long sinceUpDownHoldStarted = 0;
bool holdStarted = false;
//--------------------------------------------------------------------------------


//DFROBOT PH SETUP------------------------------------------------------------------------------------------
//#include "DFRobot_PH.h"
#define PH_PIN A4
float voltage,ecValue,temperature,voltageEC = 25.0;

//DFRobot_PH ph;

#define SensorPin A4            //pH meter Analog output to Arduino Analog Input 0
//#define Offset 0.0           //deviation compensate
#define LED 13
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;

unsigned long sinceLastpHDose;
unsigned long sincePHDosingAllowed;
unsigned long phDosingInterval = 30000;


static unsigned long sinceLastPHSamplingTime;
#define phDownPump 23
#define phUpPump 24

static float pHValue;


//float targetPH = 7;
//float phDownTolerance = 0.05;
//float phMins = 0.5;
//-----------------------------------------------------------------------------------



//DFROBOT EC SETUP------------------------------------------------------------------------------------------
#include "DFRobot_EC.h"
#define EC_PIN A5
DFRobot_EC ec;
static unsigned long sinceLastECSamplingTime;

#define ecPump 25
unsigned long sinceLastECDose = 0;
unsigned long ecDosingInterval = 30000;

//float targetEC = 1.8;
//float ecTolerance = 0.01;
//float ecMins = 0.5;
//-------------------------------------------------------------------------------------------



//Tap Water Dosing------------------------------------------------------------------------------------------
#define tapWaterValve 29
#define waterLowSensor 2
#define waterFullSensor 3
#define waterFlowSensor 18
double flowRate = 0.0;
volatile int flowCount = 0;

unsigned long sinceWaterStarted;
unsigned long sinceLastFullWatering;
unsigned long thirtyMins = 60000*30; //1800000;
unsigned long fiveMins = 60000*5;//10000;//300000;
bool cooldownHasFinished;

//float waterMins = 5.0;
//float valveCDMins = 30.0;
char waterFullChar[64];
char waterLowChar[32];
char cooldownChar[64];
//-------------------------------------------------------------------------------------------


char* waterFullText();
char* waterLowText();
char* cooldownText();

void handleIntervalElapsed(float temp, bool valid, int deviceIndex);

bool overwriteSD(const char*, const char*);
bool writeSD(const char*, const char*);
String readSD(const char*);

void saveSettings();
void loadSettings();


//TimeKeeping
bool hasBeen(int seconds,unsigned long sinceLast);
bool hasBeenUL(unsigned long seconds,unsigned long sinceLast);

//Buttons
int read_LCD_buttons();
int getButtons();

//DFRobot PH Sensor
double averagearray(int* arr, int number);

//Loop
void checkSensors();
void tryFillingWater();
void tryPHDosing();
void tryECDosing();
void menuReactTo(int);
void menuPrint();
