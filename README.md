# GrowBox-AutoRefill
Arduino auto-refilling water treatment device that guarantees the correct water level, pH, EC nutrients, and plant health

Has support for flow sensors, liquid sensors, and solenoid valve cooldown periods.

Uses LiquidMenu UI with 1602 Keypad LCD screen
Uses DFRobot Industrial pH sensor: https://wiki.dfrobot.com/Analog_pH_Meter_Pro_SKU_SEN0169
Uses DFRobot Gravity analog EC sensor: https://wiki.dfrobot.com/Gravity_Analog_Electrical_Conductivity_Sensor_Meter_K=10_SKU_DFR0300-H

DFRobot EC library was modified for more reliable calibration.

## Configuration

Target pH: is the pH level the system will aim towards using ph UP and ph DOWN buffer solutions

Intvl: represents the interval between pH sensing and dosing. Make this longer to allow your ph buffer solution more time to mix in. 


Target EC: is the EC level the system will aim towards by adding plant food.

Intvl: represents the interval between EC sensing and dosing. Make this longer to allow your plant food more time to mix in.


WMax: Max number of minutes to water consecutively for. This stops the water valve from overheating.

CD: Number of minutes to let the valve cooldown after watering for WMax mins consecutively.

![image](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/dbeaf064-ccfe-4a71-b0b0-388216bf522c)
![20231105_153834](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/8a2ec76e-9069-4ba5-b3d3-eefab6408303)

![image](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/b9073c95-52e9-4a70-9d24-ab43ba686b09)
![20231105_153849](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/c1b1a33d-300a-431b-91b7-64f78ee030cb)
![20231105_153859](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/061bb740-6e46-4419-9173-316ee3f60e28)
![20231105_153915](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/0e5b96b9-e7ba-4bd0-9f7a-5f72cbc2b1b3)
![20231105_153923](https://github.com/shahmirthesquid/GrowBox-AutoRefill/assets/89569533/0c614bc8-a5d6-488f-9186-c0337ecf08ab)


