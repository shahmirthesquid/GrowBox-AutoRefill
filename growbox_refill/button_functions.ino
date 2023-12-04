float floatStepAccel(){
  float step = 0.01;
  unsigned long second = 1000;
  int debug =0;

  if(holdStarted){
    if(hasBeenUL(second*7,sinceUpDownHoldStarted)){
      step=1;
    }
    else if(hasBeenUL(second*5,sinceUpDownHoldStarted)){
      step=0.5;
    }
    else if(hasBeenUL(second*3,sinceUpDownHoldStarted)){
      step=0.1;
    }
    else if(hasBeenUL(second*1,sinceUpDownHoldStarted)){
      step=0.05;
    }

  }

  if(debug){
    Serial.print("floatStepAccel(): Step is ");
    Serial.println(step);
  }

  return step;
}

int intStepAccel(){
  int step = 1;
  unsigned long second = 1000;
  int debug =0;

  if(holdStarted){
    if(hasBeenUL(second*7,sinceUpDownHoldStarted)){
      step=50;
    }
    else if(hasBeenUL(second*5,sinceUpDownHoldStarted)){
      step=10;
    }
    else if(hasBeenUL(second*3,sinceUpDownHoldStarted)){
      step=5;
    }
    else if(hasBeenUL(second*1,sinceUpDownHoldStarted)){
      step=2;
    }

  }

  if(debug){
    Serial.print("intStepAccel(): Step is ");
    Serial.println(step);
  }

  return step;
}

void ph_up() {

	if (config.targetPH < 14) {
		config.targetPH += floatStepAccel();
	} else {
		config.targetPH = 14;
	}
}

// Function to be attached to the pwm_line object.
void ph_down() {
	if (config.targetPH > 0.1) {
		config.targetPH -= floatStepAccel();
	} else {
		config.targetPH = 0.1;
	}
}

void water_up() {
	if (config.waterMins < 1440) {
		config.waterMins += floatStepAccel();
	} else {
		config.waterMins = 1440;
	}
  fiveMins = config.waterMins * 60000;
}

void water_down() {
	if (config.waterMins > 0) {
		config.waterMins -= floatStepAccel();
	} else {
		config.waterMins = 0;
	}
  fiveMins = config.waterMins * 60000;
}

void cd_up() {
	if (config.valveCDMins < 1440) {
		config.valveCDMins += floatStepAccel();
	} else {
		config.valveCDMins = 1440;
	}
  thirtyMins = config.valveCDMins * 60000;
}

void cd_down() {
	if (config.valveCDMins > 0) {
		config.valveCDMins -= floatStepAccel();
	} else {
		config.valveCDMins = 0;
	}
  thirtyMins = config.valveCDMins * 60000;
}


void phInterval_up() {
	if (config.phMins < 1440) {
		config.phMins += floatStepAccel();
	} else {
		config.phMins = 1440;
	}
  phDosingInterval = config.phMins * 60000;
}

void phInterval_down() {
	if (config.phMins > 0) {
		config.phMins -= floatStepAccel();
	} else {
		config.phMins = 0;
	}
  phDosingInterval = config.phMins * 60000;
}

void phDownTolerance_up() {
	if (config.phDownTolerance < 1) {
		config.phDownTolerance += floatStepAccel();
	} else {
		config.phDownTolerance = 1;
	}
}

void phDownTolerance_down() {
	if (config.phDownTolerance > 0) {
		config.phDownTolerance -= floatStepAccel();
	} else {
		config.phDownTolerance = 0;
	}
}

void ec_up() {
	if (config.targetEC < 14) {
		config.targetEC += floatStepAccel();
	} else {
		config.targetEC = 14;
	}
}

// Function to be attached to the pwm_line object.
void ec_down() {
	if (config.targetEC > 0.1) {
		config.targetEC -= floatStepAccel();
	} else {
		config.targetEC = 0.1;
	}
}


void ecInterval_up() {
	if (config.ecMins < 1440) {
		config.ecMins += floatStepAccel();
	} else {
		config.ecMins = 1440;
	}
  ecDosingInterval = config.ecMins * 60000;
}

void ecInterval_down() {
	if (config.ecMins > 0) {
		config.ecMins -= floatStepAccel();
	} else {
		config.ecMins = 0;
	}
  ecDosingInterval = config.ecMins * 60000;
}

void ecTolerance_up() {
	if (config.ecTolerance < 1) {
		config.ecTolerance += floatStepAccel();
	} else {
		config.ecTolerance = 1;
	}
}

void ecTolerance_down() {
	if (config.ecTolerance > 0) {
		config.ecTolerance -= floatStepAccel();
	} else {
		config.ecTolerance = 0;
	}
}

void phOffset_up() {
	if (config.phOffset < 0.3) {
		config.phOffset += floatStepAccel();
	} else {
		config.phOffset = 0.3;
	}
}

void phOffset_down() {
	if (config.phOffset > -0.3) {
		config.phOffset -= floatStepAccel();
	} else {
		config.phOffset = -0.3;
	}
}

void phMult_up() {
	if (config.phMult < 1000) {
		config.phMult += floatStepAccel();
	} else {
		config.phMult = 1000;
	}
}

void phMult_down() {
	if (config.phMult > 0.1) {
		config.phMult -= floatStepAccel();
	} else {
		config.phMult = 0.1;
	}
}
void phMinDose_up() {
	if (config.phMinDose < 10000) {
		config.phMinDose += intStepAccel();
	} else {
		config.phMinDose = 10000;
	}
}

void phMinDose_down() {
	if (config.phMinDose > 0) {
		config.phMinDose -= intStepAccel();
	} else {
		config.phMinDose = 0;
	}
}

void ecMult_up() {
	if (config.ecMult < 1000) {
		config.ecMult += floatStepAccel();
	} else {
		config.ecMult = 1000;
	}
}

void ecMult_down() {
	if (config.ecMult > 0.1 ){
		config.ecMult -= floatStepAccel();
	} else {
		config.ecMult = 0.1;
	}
}

void ecMinDose_up() {
	if (config.ecMinDose < 10000) {
		config.ecMinDose += intStepAccel();
	} else {
		config.ecMinDose = 10000;
	}
}

void ecMinDose_down() {
	if (config.ecMinDose > 0) {
		config.ecMinDose -= intStepAccel();
	} else {
		config.ecMinDose = 0;
	}
}

void tank_up() {
	if (config.tankSizeLiters < 1000) {
		config.tankSizeLiters += floatStepAccel();
	} else {
		config.tankSizeLiters = 1000;
	}
}

void tank_down() {
	if (config.tankSizeLiters > 0.1 ){
		config.tankSizeLiters -= floatStepAccel();
	} else {
		config.tankSizeLiters = 0.1;
	}
}