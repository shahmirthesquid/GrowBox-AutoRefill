/*
 * Created by 
 * Ark Bramwell, July 2010
 * Modified by keen 
 * Date: 05/05/2017
 * Function: this program will test the LCD panel and the buttons
 */ 

#define VERSION "Beta 0.1"
#include "refill_functions.h"
//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <LiquidMenu.h>

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

LiquidLine welcome_line1(1, 0, "GrowBox-Refill");
LiquidLine welcome_line2(0, 1, " ",VERSION);
LiquidScreen welcome_screen(welcome_line1, welcome_line2);

LiquidLine analog_line(0, 0, "pH:", pHValue, waterFullText);
LiquidLine ledState_line(0, 1, "EC:", ecValue,"us/cm", waterLowText  );
LiquidScreen screen2(analog_line, ledState_line);

LiquidLine pH_line(0, 0, "Target pH:", targetPH);
LiquidLine pHInterval_line(0,1, "Intvl:", phMins , " mins");
LiquidScreen pH_screen(pH_line,pHInterval_line);

LiquidLine waterValve_line(0,0,"WMax:", waterMins, " mins");
LiquidLine valveCooldown_line(0,1,"CD:", valveCDMins, " mins");
LiquidScreen water_screen(waterValve_line,valveCooldown_line);

LiquidMenu menu(lcd);

void ph_up() {
	if (targetPH < 14) {
		targetPH += 0.001;
	} else {
		targetPH = 14;
	}
}

// Function to be attached to the pwm_line object.
void ph_down() {
	if (targetPH > 0.1) {
		targetPH -= 0.001;
	} else {
		targetPH = 0.1;
	}
}

void water_up() {
	if (waterMins < 1440) {
		waterMins += 0.01;
	} else {
		waterMins = 1440;
	}
  fiveMins = waterMins * 60000;
}

void water_down() {
	if (waterMins > 0) {
		waterMins -= 0.01;
	} else {
		waterMins = 0;
	}
  fiveMins = waterMins * 60000;
}

void cd_up() {
	if (valveCDMins < 1440) {
		valveCDMins += 0.01;
	} else {
		valveCDMins = 1440;
	}
  thirtyMins = valveCDMins * 60000;
}

void cd_down() {
	if (valveCDMins > 0) {
		valveCDMins -= 0.01;
	} else {
		valveCDMins = 0;
	}
  thirtyMins = valveCDMins * 60000;
}


void phInterval_up() {
	if (phMins < 1440) {
		phMins += 0.01;
	} else {
		phMins = 1440;
	}
  phDosingInterval = phMins * 60000;
}

void phInterval_down() {
	if (phMins > 0) {
		phMins -= 0.01;
	} else {
		phMins = 0;
	}
  phDosingInterval = phMins * 60000;
}


void setup(void){
  //Set pinModes for all pins------
  for(int i =22;i<=37;i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,HIGH);
  }
  pinMode(LED,OUTPUT);
  pinMode(waterLowSensor,INPUT);
  pinMode(waterFullSensor,INPUT);
  //-------------------------------


  //Begin LCD
  lcd.begin(16, 2);              // start the library

  //Begin Serial
  Serial.begin(115200);
  Serial.print("GrowBox-Refill ");    //Test the serial monitor
  Serial.println(VERSION);

  //Begin Sensors
  ec.begin();
  //ph.begin();

  //Dosing---------------------------------------
  sinceLastpHDose = millis();
  sinceLastNutrientDose = millis();
  sinceWaterStarted = millis();
  sinceLastFullWatering = millis(); //-1 wraps to largest number, actually just set to millis()
  
  //Sampling sensors --------------------------------
  sinceLastPHSamplingTime = millis();
  sinceLastECSamplingTime = millis();
  sinceLastPrintTime = millis();

  //pH_screen----------------------------------------------
  pH_line.attach_function(1, ph_up);
	pH_line.attach_function(2, ph_down);
  pHInterval_line.attach_function(1, phInterval_up);
  pHInterval_line.attach_function(2, phInterval_down);
  //-----------------------------------------------------

  //water_screen----------------------------------------------
  waterValve_line.attach_function(1,water_up);
  waterValve_line.attach_function(2,water_down);
  valveCooldown_line.attach_function(1, cd_up);
  valveCooldown_line.attach_function(2, cd_down);
  //----------------------------------------------------------

  menu.add_screen(welcome_screen);
	menu.add_screen(screen2);
	menu.add_screen(pH_screen);
  menu.add_screen(water_screen);

  menu.update();
  
}
void loop(void){
  
  checkSensors();

  tryFillingWater();

  tryDosing();

  menuReactTo(read_LCD_buttons()); // read the buttons

  menuPrint();
  
  ec.calibration(voltageEC,25.0);

}




