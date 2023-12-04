/*
 * Created by 
 * Ark Bramwell, July 2010
 * Modified by keen 
 * Date: 05/05/2017
 * Function: this program will test the LCD panel and the buttons
 */ 

#define VERSION "Beta 0.3"
//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <LiquidMenu.h>

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
LiquidSystem menu((uint8_t)1);
LiquidMenu main_menu(lcd);
LiquidMenu pH_menu(lcd);
LiquidMenu ec_menu(lcd);
LiquidMenu water_menu(lcd);

bool firstTimePH = true;
bool firstTimeEC = true;
bool firstTimeWater = true;

#include "refill_functions.h"
Config config;
//Welcome Screen-----
LiquidLine welcome_line1(1, 0, "GrowBox-Refill");
LiquidLine welcome_line2(0, 1, " ",VERSION);
LiquidScreen welcome_screen(welcome_line1, welcome_line2);
//status screen--------
LiquidLine pH_line(0, 0, "pH:", pHValue,waterFullText);
LiquidLine ec_line(0, 1, "EC:", ecValue,"ms", waterLowText  );
LiquidScreen status_screen(pH_line, ec_line);

LiquidLine back_line(0,1,"  <Back>");


//PH menu ------------
LiquidLine pHTarget_line(0, 0, "Target pH:", config.targetPH);
LiquidLine pHTolerance_line(0,1, "Devi:", config.phDownTolerance,"pH");
LiquidScreen pH_screen(pHTarget_line,pHTolerance_line);

LiquidLine pHInterval_line(0,0, "PHIntv:", config.phMins , "mins");
LiquidLine pHOffset_line(0,1, "Offset:", config.phOffset,"pH");
LiquidScreen pH_screen2(pHInterval_line,pHOffset_line);

LiquidLine pHMult_line(0,0,"Mult:",config.phMult,"x");
LiquidLine pHMinDose_line(0,1,"MinDose:",config.phMinDose,"ms");
LiquidScreen pH_screen3(pHMult_line,pHMinDose_line);

LiquidLine pHBack_line(0,0,"pH Settings->");
LiquidScreen pHBack_screen(pHBack_line,back_line);



//EC menu ----------
LiquidLine ecTarget_line(0, 0, "Target EC:", config.targetEC);
LiquidLine ecTolerance_line(0,1, "Devi:", config.ecTolerance,"ms/cm");
LiquidScreen ec_screen(ecTarget_line,ecTolerance_line);

LiquidLine ecInterval_line(0,0, "ECIntv:", config.ecMins , "mins");
LiquidLine ecMult_line(0,1,"Mult:",config.ecMult,"x");
LiquidScreen ec_screen2(ecInterval_line,ecMult_line);

LiquidLine ecMinDose_line(0,0,"MinDose:",config.ecMinDose,"ms");
LiquidScreen ec_screen3(ecMinDose_line);

LiquidLine ecCal_line1(0,0,"<Cal 1413us>");
LiquidLine ecCal_line2(0,1,"<Cal 12.88ms>");
LiquidScreen ecCal_screen(ecCal_line1,ecCal_line2);

LiquidLine ecBack_line(0,0,"EC Settings->");
LiquidScreen ecBack_screen(ecBack_line,back_line);



//Water menu -------
LiquidLine cdRemaining_line(0,0,"CD left:",cooldownText);
LiquidLine flowRate_line(0,1,"Flow:",waterFullText);
LiquidScreen waterStatus_screen(cdRemaining_line,flowRate_line);

LiquidLine waterValve_line(0,0,"Limit:", config.waterMins, " mins");
LiquidLine valveCooldown_line(0,1,"CD:", config.valveCDMins, " mins");
LiquidScreen water_screen(waterValve_line,valveCooldown_line);
LiquidLine tankSize_line (0,0,"Tank:",config.tankSizeLiters,"L");
LiquidScreen water_screen2(tankSize_line);

LiquidLine waterBack_line(0,0,"Water Settings->");
LiquidScreen waterBack_screen(waterBack_line,back_line);

//Save Screen ----------
LiquidLine save_line(0,0,"  Save");
LiquidLine load_line(0,1,"  Load");
LiquidScreen save_screen(save_line,load_line);

//LiquidLine curScreen_line(0,0,curScreen);
//LiquidLine back_line(0,1,"  <Back>");
LiquidScreen back_screen(back_line);

#include "button_functions.h"


void backToMainMenu(){
  Serial.print("backToMainMenu()");
  Serial.println(menu.get_currentMenu()); //this is a custom function i implemented into the LiquidMenu library
  uint8_t phMenu =1;
  uint8_t ecMenu =2;
  uint8_t waterMenu =3;
  if(menu.get_currentMenu() == phMenu){
    menu.change_menu(main_menu);
    menu.switch_focus();
    menu.switch_focus();
  }
  else if(menu.get_currentMenu() == ecMenu){
    menu.change_menu(main_menu);
    menu.switch_focus();
  }
  else if(menu.get_currentMenu() == waterMenu){
    menu.change_menu(main_menu);
    menu.switch_focus();
    //menu.switch_focus();
  }

  menu.update();

}


void setup(void){
  sensors.begin(NonBlockingDallas::resolution_12, NonBlockingDallas::unit_C, TIME_INTERVAL);
  sensors.onIntervalElapsed(handleIntervalElapsed);
  //Set pinModes for all pins------
  for(int i =22;i<=37;i++){
    pinMode(i,OUTPUT);
    digitalWrite(i,HIGH);
  }
  pinMode(LED,OUTPUT);
  pinMode(waterLowSensor,INPUT);
  pinMode(waterFullSensor,INPUT);
  pinMode(waterFlowSensor,INPUT);
  attachInterrupt(digitalPinToInterrupt(waterFlowSensor),[](){ flowCount++; },RISING);
  flowCount = 0;
  interrupts();
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
  sinceLastECDose = millis();
  sinceWaterStarted = millis();
  sinceLastFullWatering = millis(); //-1 wraps to largest number, actually just set to millis()
  
  //Sampling sensors --------------------------------
  sinceLastPHSamplingTime = millis();
  sinceLastECSamplingTime = millis();
  sinceLastTempSamplingTime = millis();
  sinceLastPrintTime = millis();

  //status_screens--------------------------------------------------
  pH_line.attach_function(1, [](){  menu.change_menu(pH_menu); if(firstTimePH){ firstTimePH = false;} else{menu.switch_focus(); }   } );
  pH_line.attach_function(2, [](){  menu.change_menu(pH_menu); if(firstTimePH){ firstTimePH = false;} else{menu.switch_focus(); }} );
  ec_line.attach_function(1, [](){   menu.change_menu(ec_menu); if(firstTimeEC){ firstTimeEC = false;} else{menu.switch_focus(); } } );
  ec_line.attach_function(2, [](){    menu.change_menu(ec_menu); if(firstTimeEC){ firstTimeEC = false;} else{menu.switch_focus(); }  } );

  flowRate_line.attach_function(1, [](){ menu.change_menu(water_menu); if(firstTimeWater){ firstTimeWater = false;} else{menu.switch_focus();} }   );
  flowRate_line.attach_function(2, [](){ menu.change_menu(water_menu); if(firstTimeWater){ firstTimeWater = false;} else{menu.switch_focus();} }  );

  //pH_screen----------------------------------------------
  pHTarget_line.attach_function(1, ph_up);
	pHTarget_line.attach_function(2, ph_down);
  pHTolerance_line.attach_function(1, phDownTolerance_up);
  pHTolerance_line.attach_function(2, phDownTolerance_down);
  pHInterval_line.attach_function(1, phInterval_up);
  pHInterval_line.attach_function(2, phInterval_down);
  pHOffset_line.attach_function(1, phOffset_up);
  pHOffset_line.attach_function(2, phOffset_down);
  pHMult_line.attach_function(1, phMult_up);
  pHMult_line.attach_function(2, phMult_down);
  pHMinDose_line.attach_function(1, phMinDose_up);
  pHMinDose_line.attach_function(2, phMinDose_down);
  //-----------------------------------------------------


  //ec_screen----------------------------------------------
  ecTarget_line.attach_function(1, ec_up);
	ecTarget_line.attach_function(2, ec_down);
  ecTolerance_line.attach_function(1, ecTolerance_up);
  ecTolerance_line.attach_function(2, ecTolerance_down);
  ecInterval_line.attach_function(1, ecInterval_up);
  ecInterval_line.attach_function(2, ecInterval_down);
  ecCal_line1.attach_function(1,[](){ec.calibrationLCD(voltageEC,temperature,lcd);}  );
  //ecCal_line1.attach_function(2,[](){ec.ecCalibrationLCD(lcd);}  );
  ecCal_line2.attach_function(1,[](){ec.calibrationLCD(voltageEC,temperature,lcd);}  );
  //ecCal_line2.attach_function(2,[](){ec.ecCalibrationLCD(lcd);}  );
  ecMult_line.attach_function(1, ecMult_up);
  ecMult_line.attach_function(2, ecMult_down);
  ecMinDose_line.attach_function(1, ecMinDose_up);
  ecMinDose_line.attach_function(2, ecMinDose_down);

  //-----------------------------------------------------

  //water_screen----------------------------------------------
  waterValve_line.attach_function(1,water_up);
  waterValve_line.attach_function(2,water_down);
  valveCooldown_line.attach_function(1, cd_up);
  valveCooldown_line.attach_function(2, cd_down);
  tankSize_line.attach_function(1,tank_up);
  tankSize_line.attach_function(2,tank_down);

  //----------------------------------------------------------

  //Saving and Loading------------------------------------------------------------
  save_line.attach_function(1, saveSettingsJSON);
  load_line.attach_function(1, []() {loadSettingsJSON();} );
  //-------------------------------------------------------------------------------

  back_line.attach_function(1, backToMainMenu );
  back_line.attach_function(2, backToMainMenu );

  main_menu.add_screen(welcome_screen);
  main_menu.add_screen(status_screen);
  main_menu.add_screen(waterStatus_screen);
  main_menu.add_screen(save_screen);

  pH_menu.add_screen(pHBack_screen);
  pH_menu.add_screen(pH_screen);
  pH_menu.add_screen(pH_screen2);
  pH_menu.add_screen(pH_screen3);

  ec_menu.add_screen(ecBack_screen);
  ec_menu.add_screen(ec_screen);
  ec_menu.add_screen(ec_screen2);
  ec_menu.add_screen(ec_screen3);
  ec_menu.add_screen(ecCal_screen);

  water_menu.add_screen(waterBack_screen);
  water_menu.add_screen(water_screen);
  water_menu.add_screen(water_screen2);

  //water_menu.add_screen(water_screen);
  //water_menu.add_screen(back_screen);
  

  //menu.add_screen(welcome_screen);
	//menu.add_screen(status_screen);
	//menu.add_screen(pH_screen);
  //menu.add_screen(pH_screen2);
  //menu.add_screen(ec_screen);
  //menu.add_screen(ec_screen2);
  //menu.add_screen(water_screen);
  //menu.add_screen(save_screen);

  menu.add_menu(main_menu);
  menu.add_menu(pH_menu);
  menu.add_menu(ec_menu);
  menu.add_menu(water_menu);

  menu.update();


  Serial.println("Trying to load settings!");
  delay(1000);
  loadSettingsJSON();
  delay(500);

  menu.next_screen();
  
}
void loop(void){
  
  checkSensors();

  tryFillingWater();

  tryPHDosing();

  tryECDosing();

  menuReactTo(read_LCD_buttons()); // read the buttons

  menuPrint();
  
  ec.calibration(voltageEC,temperature);


}




