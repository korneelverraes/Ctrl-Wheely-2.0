#include <SimpleFOC.h>

int steps[] = { 10, 8, 8, 8, 15 };
float strengths[] = { 5, 3, 3, 3, 8 };

String functions[] = { "nav", "tempL", "tempR", "music", "phone" };
String buttons[] = { "press", "left", "right" };
String RPIdata = "";

int sensorPin[] = { A0, A1, A2, A3, A4 };
int sensorValue = 0;
int buttonPin[] = { 2, 3, 4 };
int buttonState[] = { 0, 0, 0 };
int buttonLastState[] = { -1, -1, -1 };

int position = 0;
int lastPosition = 5;
bool home = false;       //voor timer
bool longPress = false;  //voor timer
int currentStep = 20;
bool limitccw = false;
bool limitcw = false;
bool spotify = false;
bool turnSent = false;

unsigned long t0 = 0;  //time-out timer
unsigned long t1 = 0;
unsigned long t2 = 0;  //longpress

float attractAngle = 0;
float lastAngle = 0;
float offsetAngle = 0;
//float localAngle = 0;
//float torqueVoltage = 0;

MagneticSensorSPI sensor = MagneticSensorSPI(10, 14, 0x3FFF);
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 5, 6, 8);
BLDCMotor motor = BLDCMotor(11);
PIDController P_haptic{ .P = 5, .I = 0, .D = 0, .output_ramp = 100000, .limit = 5 };

void setup() {
  Serial.begin(115200);

  sensor.init();

  driver.pwm_frequency = 20000;
  driver.voltage_power_supply = 11,2;
  driver.voltage_limit = 5;
  driver.init();

  motor.linkSensor(&sensor);
  motor.linkDriver(&driver);
  motor.controller = MotionControlType::torque;
  motor.init();
  motor.initFOC();

  for (int i = 0; i < 3; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void loop() {
  motor.loopFOC();

  t1 = _micros();

  //POSITION
  for (int i = 0; i < 5; i++) {
    sensorValue = analogRead(sensorPin[i]);
    if (sensorValue > 350) {
      position = i;
      currentStep = steps[i];
    }
    /*Serial.print(sensorValue);
    Serial.print(" | ");*/
  }
  if (home == false && t1 - t0 >= 8000000) {  //8s home timer
    home = true;
    Serial.println("home");
  }
  if (position != lastPosition) {
    Serial.println(functions[position]);
    offsetAngle = motor.shaft_angle - round(motor.shaft_angle / (2 * _PI / currentStep)) * (2 * _PI / currentStep);
    //P_haptic.P = strengths[position];
    lastAngle = attractAngle;  //elimineert minimale verdraaiing tijdens verschuiven
    limitccw = false;
    limitcw = false;
    spotify = false;
    t0 = t1;
    home = false;
    lastPosition = position;
  }

  //TURNING
  if (attractAngle > lastAngle + 0.05) { //links draaien
    if (home == true) Serial.println(functions[position]);
    Serial.println("ccw");
    limitcw = false;
    t0 = t1;
    home = false;
    lastAngle = attractAngle;
  } else if (attractAngle < lastAngle - 0.05) { //rechts draaien
    if (home == true) Serial.println(functions[position]);
    Serial.println("cw");
    limitccw = false;
    t0 = t1;
    home = false;
    lastAngle = attractAngle;
  }

  if (spotify) {  //magnetisch midden voor vorig/volgend nummer
    if (turnSent == false && motor.shaft_angle - attractAngle > 0.8) {
      if (home == true) Serial.println(functions[position]);
      Serial.println("ccw");
      limitcw = false;
      t0 = t1;
      home = false;
      turnSent = true;
    } else if (turnSent == false && attractAngle - motor.shaft_angle > 0.8) {
      if (home == true) Serial.println(functions[position]);
      Serial.println("cw");
      limitcw = false;
      t0 = t1;
      home = false;
      turnSent = true;
    } else if (abs(motor.shaft_angle - attractAngle) < 0.2) {
      turnSent = false;
    }
  }

  //CLICKING
  for (int i = 0; i < 3; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);
    if (buttonState[i] == 0 && buttonLastState[i] == 1) {
      if (home == true) Serial.println(functions[position]);
      t2 = t1;
      t0 = t1;
    }
    if (i == 0 && buttonState[i] == 0) {
      if (t1 - t2 >= 1500000) {  //+/- 2s long press
        Serial.println("longpress");
        longPress = true;
        t2 = t1;
        t0 = t1;
        home = false;
      }
    }
    if (buttonState[i] == 1 && buttonLastState[i] == 0) {
      if (longPress == false) {
        Serial.println(buttons[i]);
        limitccw = false;
        limitcw = false;
        spotify = false;
        t0 = t1;
        home = false;
      } else {
        longPress = false;
      }
    }
    buttonLastState[i] = buttonState[i];
  }
  //Serial.println();

  //MOTOR
  float target = findAttractor(motor.shaft_angle);

  if (Serial.available() > 0) {
    RPIdata = Serial.readStringUntil('\n');
    RPIdata.trim();
    if (RPIdata == "limietccw") limitccw = true;
    if (RPIdata == "limietcw") limitcw = true;
    if (RPIdata == "spotify") spotify = true;
  }

  if (limitccw && target > lastAngle) { //limieten instellen
    attractAngle = lastAngle;
  } else if (limitcw && target < lastAngle) {
    attractAngle = lastAngle;
  } else if (spotify) {
    attractAngle = lastAngle;
  } else {
    attractAngle = target;
  }

  P_haptic.P = map(abs(sensor.getVelocity()), 0, 15, strengths[position], 1); //kracht van de tikjes verminderen wanneer je sneller draait
  motor.move(P_haptic(attractAngle - motor.shaft_angle));

  //Serial.println(sensor.getVelocity());
  //Serial.println(attractAngle);
  //Serial.println(P_haptic(attractAngle - motor.shaft_angle));

  /*localAngle = fmod(motor.shaft_angle, 2 * _PI / currentStep);
  if (localAngle < 0) localAngle += 2 * _PI / currentStep;
  torqueVoltage = (_PI / currentStep - localAngle) / (_PI / currentStep) * driver.voltage_limit;

  motor.move(torqueVoltage);

  Serial.print(localAngle);
  Serial.print("\t");
  Serial.print(torqueVoltage);
  Serial.print("\t");
  Serial.println(steps);*/

  //delay(100); //button debounce
}

float findAttractor(float currentAngle) {
  return round((currentAngle - offsetAngle) / (2 * _PI / currentStep)) * (2 * _PI / currentStep) + offsetAngle;
}
