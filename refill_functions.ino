
 
// read the buttons
int read_LCD_buttons()
{
  lcd_key = analogRead(0);      // read the value from the sensor 
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  int tol = 50;

  unsigned long leftRightDelay = 220;
  bool allowedToLeftRight = hasBeenUL(leftRightDelay,sinceLastLeftRightKey);

  if (lcd_key > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  else if ( lcd_key>100-tol  && lcd_key < 100+tol) {
    return btnUP;
  } 
  else if (lcd_key>256-tol  && lcd_key < 256+tol) {
    return btnDOWN;
  } 
  else if (lcd_key>640-tol  && lcd_key < 640+tol) {
    return btnSELECT;
  } 
  else if (lcd_key < 0+tol) {
    //return btnRIGHT;
    if(allowedToLeftRight){
      Serial.println(allowedToLeftRight);
      sinceLastLeftRightKey = millis();
      return btnRIGHT;
    }
  } 
  else if (lcd_key>407-tol  && lcd_key < 407+tol) {
    if(allowedToLeftRight){
      Serial.println(allowedToLeftRight);
      sinceLastLeftRightKey = millis();
      return btnLEFT;
    }
  } 
  else {
    return btnNONE;
  }
  return btnNONE;  // when all others fail, return this...
}

bool hasBeen(int seconds,unsigned long sinceLast){
  if( (unsigned long)   (millis() - sinceLast) > (unsigned long) seconds){
    return true;
  }
  return false;
}

bool hasBeenUL(unsigned long seconds,unsigned long sinceLast){
  if( (unsigned long)   (millis() - sinceLast) > (unsigned long) seconds){
    return true;
  }
  return false;
}

double averagearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

char* waterFullText(){
  if(digitalRead(waterFullSensor)){
    strcpy(waterFullChar,"       H");
  }
  else{
    strcpy(waterFullChar," ");
  }
  return waterFullChar;

}
char* waterLowText(){
  if(digitalRead(waterLowSensor)){
    strcpy(waterLowChar,"   L");
  }
  else{
    strcpy(waterLowChar," ");
  }
  return waterLowChar;

}

int getButtons()
{
    static int keyLast = btnNONE;
    static unsigned long keyTimeLast = 0;
    int analogKey;
    int key = btnNONE;

    analogKey = analogRead(0);

    //if ((millis() - keyTimeLast) <200) {
    //    return keyLast;
    //}

    //keyTimeLast = millis();

    unsigned long leftRightDelay = 220;
    bool allowedToLeftRight = hasBeenUL(leftRightDelay,sinceLastLeftRightKey);

    int tol = 5;

    if (analogKey < 0+tol) {
        if(allowedToLeftRight){
        key = btnRIGHT;
        Serial.println(allowedToLeftRight);
        sinceLastLeftRightKey = millis();
      }
    } else if ( analogKey>100-tol  && analogKey < 100+tol) {
        key = btnUP;
    } else if (analogKey>256-tol  && analogKey < 256+tol) {
        key = btnDOWN;
    } else if (analogKey>407-tol  && analogKey < 407+tol) {
      if(allowedToLeftRight){
        key = btnLEFT;
        sinceLastLeftRightKey = millis();
        Serial.println(allowedToLeftRight);
      }
    } else if (analogKey>640-tol  && analogKey < 640+tol) {
        key = btnSELECT;
    } else {
        key = btnNONE;
    }

    if (key == keyLast) {
        return key;
    } else {
        keyLast = key;
        return btnNONE;
    }
}



void checkSensors(){

  // if Time to sample PH.... samplingInterval is an int
  if(hasBeen(samplingInterval, sinceLastPHSamplingTime) ){
      delay(1);
      pHArray[pHArrayIndex++]=analogRead(SensorPin);
      if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
      voltage = averagearray(pHArray, ArrayLenth)*5.0/1024;
      pHValue = 3.5*voltage+Offset;
      sinceLastPHSamplingTime=millis();
      /*
      delay(50);
      voltage = analogRead(PH_PIN)/1024.0*5000;
      delay(50);
      pHValue = ph.readPH(voltage,25.0);
      */
  }
  //pHValue = 6.9; //this is used to test 

  // if time to sample EC
  if( hasBeen( 1000 , sinceLastECSamplingTime)){//time interval: 1s
    delay(1);
    sinceLastECSamplingTime = millis();
    voltageEC = analogRead(EC_PIN)/1024.0*5000;   // read the voltage
    //temperature = readTemperature();          // read your temperature sensor to execute temperature compensation
    //Serial.println(voltageEC);
    ecValue =  ec.readEC(voltageEC,25.0);  // convert voltage to EC with temperature compensation
  }

}


void tryFillingWater(){
  int debug = 0;
  bool wateringLimitReached = hasBeenUL(fiveMins,sinceWaterStarted);
  if(wateringLimitReached){
    sinceLastFullWatering = millis();
    if(debug){
      Serial.println("\n\n\n\033[31m FULL WATERING DETECTED COOLDOWN STARTED \033[0m\n\n\n");
    }
    //delay(2000);
  }
  
  bool cooldownHasFinished =  hasBeenUL(thirtyMins, sinceLastFullWatering);
  if( !wateringLimitReached && cooldownHasFinished ){

    //lcd.print("W1:");
    if(digitalRead(waterFullSensor)==LOW){
      //lcd.print("F");
      digitalWrite(tapWaterValve,LOW);
    }
    else{
      //lcd.print("T");
      digitalWrite(tapWaterValve,HIGH);
      sinceWaterStarted = millis();
    }
    //Serial.print("sinceWaterStarted:");
    //Serial.println(sinceWaterStarted);


  }
  else{
    
    digitalWrite(tapWaterValve,HIGH);
    sinceWaterStarted = millis();

    if(debug){
      Serial.println("FULL WATERING DETECTED");
      Serial.print("currenttime:");
      Serial.println(millis());
      Serial.print("sinceLastFullWatering:");
      Serial.println(sinceLastFullWatering);
      Serial.print("sinceWaterStarted:");
      Serial.println(sinceWaterStarted);
      Serial.print("hasBeen SinceWaterStarted:");
      Serial.println(hasBeen(3000,sinceWaterStarted));
    }

  }

}



void tryDosing(){

  float tolerance = 0.01;
  //lcd.print("W2:");
  if(pHValue>(targetPH+tolerance)  && hasBeenUL( phDosingInterval, sinceLastpHDose) ){
    
    unsigned long dosingTime =  ( (pHValue - targetPH) * 1000 ) +100  ;

    if(!hasBeenUL(dosingTime,sinceLastpHDose+phDosingInterval)){
      digitalWrite(phDownPump,LOW);
    }
    else{
      sinceLastpHDose = millis();
      //lcd.print("F");
      digitalWrite(phDownPump,HIGH);

    }
  }
  
  else if(pHValue<(targetPH-tolerance)  && hasBeenUL( phDosingInterval, sinceLastpHDose) ){
    
    unsigned long dosingTime =  ( ( targetPH - pHValue) * 1000 ) +100  ;

    if(!hasBeenUL(dosingTime,sinceLastpHDose+phDosingInterval)){
      digitalWrite(phUpPump,LOW);
    }
    else{
      sinceLastpHDose = millis();
      //lcd.print("F");
      digitalWrite(phUpPump,HIGH);

    }
  }
  
  else{
    //lcd.print("T");
    digitalWrite(phDownPump,HIGH);
    digitalWrite(phUpPump,HIGH);
  }
}

void menuReactTo(int lcd_key){


  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      {
      //lcd.print("RIGHT ");
      Serial.println(F("RIGHT button pressed"));
		  menu.next_screen();
      menu.update();
      break;
      }
    case btnLEFT:
      {
      //lcd.print("LEFT   ");
      Serial.println(F("LEFT button pressed"));
		  menu.previous_screen();
      menu.update();
      break;
      }
    case btnUP:
      {
      //lcd.print("UP    ");
      Serial.println(F("UP button pressed"));
		  menu.call_function(1);
      break;
      }
    case btnDOWN:
      {
      //lcd.print("DOWN  ");
      Serial.println(F("DOWN button pressed"));
		  menu.call_function(2);
      break;
      }
    case btnSELECT:
      {
      //lcd.print("SELECT");
      Serial.println(F("SELECT button pressed"));
		  menu.switch_focus();
      menu.update();
      break;
      }
    case btnNONE:
      {
      //lcd.print("NONE  ");
      break;
      }
  }

}

void menuPrint(){

  int debug = 0;

  if(hasBeen(printInterval, sinceLastPrintTime)){   //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
    sinceLastPrintTime=millis();
    digitalWrite(LED,digitalRead(LED)^1);
    
    /*
    lcd.setCursor(0,0);  
    //lcd.print("V:");
        //lcd.print(voltage);
    lcd.print(" pH:");
    lcd.println(pHValue);
    
    
    lcd.setCursor(0,1);  
    //lcd.print(temperature,1);
    lcd.print("EC:");
    lcd.print(ecValue,2);
    //Serial.println(ecValue);
    lcd.print("ms/cm");
    */
    //menu.next_screen();
    menu.update();
    
    if(debug){
      Serial.print("Water Valve CoolDown Time Remaining: ");
      int tmp =(thirtyMins - (millis() - sinceLastFullWatering) ) / 1000;
      Serial.println( tmp );
      Serial.print("Time since Water Valve Enabled: ");
      tmp = (millis() - sinceWaterStarted)/1000; 
      Serial.println(tmp);
      Serial.print("Current time:  ");
      Serial.print(millis());
      Serial.print("   sinceLastFullWatering:  ");
      Serial.println(sinceLastFullWatering);

    }
    
  }
}
