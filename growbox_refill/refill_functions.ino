
 
// read the buttons
int read_LCD_buttons()
{
  lcd_key = analogRead(0);      // read the value from the sensor 
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  int tol = 20;

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

bool initSD(){
  int pinCS = 53;
  if(SD.begin(pinCS)){
    Serial.println("SD init successful!");
    return true;
  }
  Serial.println("SD init failed!");
  return false;
}

bool overwriteSD(const char* path, const char* message){

  if( !initSD() ){
    return false;
  }

  File file = SD.open(path, FILE_WRITE | O_CREAT | O_TRUNC);

  if(file){
    file.println(message);
    file.close();
    SD.end();
    return true;
  }
  Serial.print("Error writing to file ");
  Serial.println(path);
  SD.end();
  return false;
}

bool writeSD(const char* path, const char* message){

  if( !initSD() ){
    return false;
  }

  File file = SD.open(path, FILE_WRITE | O_APPEND | O_CREAT);

  if(file){
    file.println(message);
    file.close();
    SD.end();
    return true;
  }
  Serial.print("Error writing to file ");
  Serial.println(path);
  SD.end();
  return false;
}

String readSD(const char* filename) {

  if( !initSD() ){
    return "";
  }

  File file = SD.open(filename);
  String data = "";
  if (file) {
    while (file.available()) {
      data += (char)file.read();
    }
    file.close();
  } else {
    Serial.print("Error reading file ");
    Serial.println(filename);
  }
  SD.end();
  return data;
}



void loadSettings(){

  //char[64] settingsFolder;
  //strcpy(settingsFolder, "growbox-autorefill settings")
  
  int debug = 0;
  
  bool issue = false;
  bool sdPluggedIn = initSD();

  //if(issue){
    targetPH = 7;
    phDownTolerance = 0.05;
    phMins = 0.5;
    phDosingInterval = phMins * 60000;
    targetEC = 1.8;
    ecTolerance = 0.02;
    ecMins = 0.5;
    ecDosingInterval = ecMins * 60000;
    waterMins = 5.0;
    valveCDMins = 30.0;

    fiveMins = waterMins * 60000;
    thirtyMins = valveCDMins * 60000;
  //}

  if(sdPluggedIn){


    String tmp;
    tmp = readSD("growbox/phTarget.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading phtarget!");
      }
    }
    else{
      targetPH = tmp.toFloat();
    }


    tmp = readSD("growbox/phDTol.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading phtolerance!");
      }
    }
    else{
      phDownTolerance = tmp.toFloat();
    }


    tmp = readSD("growbox/phIntv.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading ph interval!");
      }
    }
    else{
      phMins = tmp.toFloat();
    }
    phDosingInterval = phMins * 60000;


    tmp = readSD("growbox/ecTarget.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading ec target!");
      }
    }
    else{
      targetEC = tmp.toFloat();
    }


    tmp = readSD("growbox/ecTol.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading ec tolerance!");
      }
    }
    else{
      ecTolerance = tmp.toFloat();
    }

    tmp = readSD("growbox/ecIntv.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading ec interval!");
      }
    }
    else{
      ecMins = tmp.toFloat();
    }
    ecDosingInterval = ecMins * 60000;

    tmp = readSD("growbox/WMax.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading WMax!");
      }
    }
    else{
      waterMins = tmp.toFloat();
    }
    fiveMins = waterMins * 60000;

    tmp = readSD("growbox/CD.txt");
    if(tmp==""){
      issue = true;
      if(debug){
        Serial.println("Issue loading valve CD!");
      }
    }
    else{
      valveCDMins = tmp.toFloat();
    }
    thirtyMins = valveCDMins * 60000;
  }


  //return issue;
  if(debug){
    Serial.print("SD card detected? ");
    Serial.println(sdPluggedIn);
    
    Serial.print("loading Issue detected? ");
    Serial.println(issue);

  }

  if(!sdPluggedIn){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" No SD card! :(");
    delay(500);
  }
  else if(issue){
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("No files to load");
    delay(500);
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Settings loaded!");
    delay(500);
  }
}

void saveSettings(){

  int pinCS = 53;

  SD.begin(pinCS);

  char settingsFolder[64];
  strcpy(settingsFolder, "growbox");
  if( !SD.exists(settingsFolder) ){
    if( SD.mkdir(settingsFolder) ){
      Serial.println("Settings Folder Created!");
    }
    else{
      Serial.println("Issue Creating Settings Folder!  :( ");
      return;
    }
  }
  SD.end();


  char data[64];
  char filePath[64];
  //sprintf(data,"%f",targetPH);
  dtostrf(targetPH,10,6,data);
  sprintf(filePath,"%s/phTarget.txt",settingsFolder);
  overwriteSD(filePath,data);
  //sprintf(data,"%f",phDownTolerance);
  dtostrf(phDownTolerance,10,6,data);
  sprintf(filePath,"%s/phDTol.txt",settingsFolder);
  overwriteSD(filePath,data);
  //sprintf(data,"%f",phMins);
  dtostrf(phMins,10,6,data);
  sprintf(filePath,"%s/phIntv.txt",settingsFolder);
  overwriteSD(filePath,data);

  //sprintf(data,"%f",targetEC);
  dtostrf(targetEC,10,6,data);
  sprintf(filePath,"%s/ecTarget.txt",settingsFolder);
  overwriteSD(filePath,data);
  //sprintf(data,"%f",ecTolerance);
  dtostrf(ecTolerance,10,6,data);
  sprintf(filePath,"%s/ecTol.txt",settingsFolder);
  overwriteSD(filePath,data);
  //sprintf(data,"%f",ecMins);
  dtostrf(ecMins,10,6,data);
  sprintf(filePath,"%s/ecIntv.txt",settingsFolder);
  overwriteSD(filePath,data);

  //sprintf(data,"%f",waterMins);
  dtostrf(waterMins,10,6,data);
  sprintf(filePath,"%s/WMax.txt",settingsFolder);
  overwriteSD(filePath,data);
  //sprintf(data,"%f",valveCDMins);
  dtostrf(valveCDMins,10,6,data);
  sprintf(filePath,"%s/CD.txt",settingsFolder);
  overwriteSD(filePath,data);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   SAVED !");
  delay(500);

  //return true;

}


/*
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
*/


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



void tryPHDosing(){

  float tolerance = phDownTolerance;
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

void tryECDosing(){

  float tolerance = ecTolerance;
  //lcd.print("W2:");

  if(ecValue<(targetEC-tolerance)  && hasBeenUL( ecDosingInterval, sinceLastECDose) ){
    
    unsigned long dosingTime =  ( ( targetEC - ecValue) * 1000 ) +100  ;

    if(!hasBeenUL(dosingTime,sinceLastECDose+ecDosingInterval)){
      digitalWrite(ecPump,LOW);
    }
    else{
      sinceLastECDose = millis();
      //lcd.print("F");
      digitalWrite(ecPump,HIGH);

    }
  }
  
  else{
    //lcd.print("T");
    digitalWrite(ecPump,HIGH);
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
