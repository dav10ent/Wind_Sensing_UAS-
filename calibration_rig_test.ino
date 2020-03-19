
#include <math.h>
#define ttStepPin 7
#define ttDirPin 6
#define ttMicroSteps 32
#define ttGearRatio 5.18

#define rotorStepPin 5
#define rotorDirPin 4
#define rotorMicroSteps 32
#define rotorGearRatio 10.2

#define pulseOutPin 3

int ttStepsCircle = (360/1.8) * ttGearRatio * ttMicroSteps;

double rotorStepsCircle = (360/1.8) * rotorGearRatio * rotorMicroSteps;

int ttSpeed = 100;
int rotorSpeed = 150;

int angleIncrement = 5;
int sweepLimit = 45;

int numIncrements = 2*sweepLimit/angleIncrement;

int settlingTime = 10000;

void setup() {
  // put your setup code here, to run once:
  pinMode(ttStepPin, OUTPUT);
  pinMode(ttDirPin, OUTPUT);
  pinMode(rotorStepPin, OUTPUT);
  pinMode(rotorDirPin, OUTPUT);

  pinMode(pulseOutPin, OUTPUT);
  Serial.begin(9600);

  runCalibration();
  
}



void loop() {
  

}

void runCalibration() {

  //Assuming Rig is zeroed
  rotateTT(0, 45, ttSpeed);
  rotateRotor(0, 45, rotorSpeed);

  delay(2000);
  
  for(int rot = 0; rot < numIncrements; rot++) {

    if(rot != 0) {
      rotateRotor(1, angleIncrement, rotorSpeed);
      delay(settlingTime);
    }
  
    
   
    for(int tt = 0; tt < numIncrements; tt++) {
        rotateTT(1, angleIncrement, ttSpeed);
        delay(1000);
        digitalWrite(pulseOutPin, HIGH);
        delay(100);
        digitalWrite(pulseOutPin, LOW);
        delay(settlingTime);
    }
    rotateTT(0, 90, ttSpeed);
    delay(settlingTime);   
  }

  //Return to Zero
  rotateRotor(0, 45, rotorSpeed);
  rotateTT(1, 45, ttSpeed);
   
}

void rotateTT(int dir, double degrees, int speed) {
  int steps = round(ttStepsCircle * (degrees/360));
  digitalWrite(ttDirPin, dir);

  for(int i = 0; i < steps; i++) {
    digitalWrite(ttStepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(ttStepPin, LOW);
    delayMicroseconds(speed);
  }
  
}

void rotateRotor(int dir, double degrees, int speed) {
  long steps = round(rotorStepsCircle * (degrees/360));  

  digitalWrite(rotorDirPin, dir);
  for(long i = 0; i < steps; i++) {
    digitalWrite(rotorStepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(rotorStepPin, LOW);
    delayMicroseconds(speed);
  }
  
}

void rotateSimultaneous(int ttDir, int rotorDir, double ttDegrees, double rotorDegrees, int ttSpeed, int rotorSpeed) {
  long ttSteps = round(ttStepsCircle * (ttDegrees/360));
  long rotorSteps = round(rotorStepsCircle * (rotorDegrees/360));
  Serial.println(ttSteps);
  

  long longestSteps = rotorSteps > ttSteps ? rotorSteps : ttSteps;
  long shortestSteps = rotorSteps > ttSteps ? ttSteps : rotorSteps;
  boolean ttShortest = rotorSteps > ttSteps;

  digitalWrite(ttDirPin, ttDir);
  digitalWrite(rotorDirPin, rotorDir);
  
  for(long i = 0; i < longestSteps; i++) {
    if(i <= shortestSteps) {
      digitalWrite(ttStepPin, HIGH);
      digitalWrite(rotorStepPin, HIGH);
      delayMicroseconds(ttSpeed);
      digitalWrite(ttStepPin, LOW);
      digitalWrite(rotorStepPin, LOW);
      delayMicroseconds(ttSpeed);
    } else if (i > shortestSteps && ttShortest) {
        digitalWrite(rotorStepPin, HIGH);
        delayMicroseconds(ttSpeed);
        digitalWrite(rotorStepPin, LOW);
        delayMicroseconds(ttSpeed);
    } else if (i > shortestSteps && !ttShortest) {
        digitalWrite(ttStepPin, HIGH);
        delayMicroseconds(ttSpeed);
        digitalWrite(ttStepPin, LOW);
        delayMicroseconds(ttSpeed);
    }
  }
}
