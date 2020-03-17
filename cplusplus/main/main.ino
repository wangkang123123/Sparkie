/**
  The purpose of this project ...
  Libraries used:
  ArduinoOdrive - https://github.com/madcowswe/ODrive/tree/master/Arduino/ODriveArduino
  ArduinoJSON - https://github.com/bblanchon/ArduinoJson
  -----------------------------------------------------------
  Code by: Magnus Kvendseth Øye, Vegard Solheim, Petter Drønnen
  Date: 10.02-2020
  Version: 0.4
  Website: https://github.com/magnusoy/Sparkie
*/

// Including libraries and headers
#include <ArduinoJson.h>

#include "src/libraries/ODriveArduino/OdriveArduino.h"
#include "src/libraries/SerialHandler/SerialHandler.h"
#include "src/libraries/LegMovement/LegMovement.h"
#include "src/libraries/Timer/Timer.h"

#include "Globals.h"
#include "Constants.h"
#include "OdriveParameters.h"
#include "IO.h"


SerialHandler serial(BAUDRATE , CAPACITY);
LegMovement legMovement;

/* Variable for the intervall time for walking case*/
Timer walkIntervall;
int intervall = 1.0; //0.1

/* Variable for storing time for leg tracjetory */
unsigned long n = 0;

/* Variable for storing loop time */
unsigned long loopTime;
unsigned long walkTime;

/* Variable for jump fuction*/
Timer airTime;
Timer groundTime;
bool runned = false;
int jump = 0;

void setup() {
  Serial.println("Setup started");
  serial.initialize();
  initializeButtons();
  initializeLights();
  initializeOdrives();
  Serial.println("Setup finished");
}

void loop() {
  //loopTime = micros();
  //readOdriveMotorPositions(hwSerials, odrives);

  switch (currentState) {
    case S_IDLE:
      blinkLight(GREEN_LED);
      if (!idlePosition && calibrated) {
        armMotors();
        delay(100);
        double x = 0;
        double y = -160;
        for (int Odrive = 0; Odrive < 4; Odrive ++) {
          for (int motor = 0; motor < 2; motor++) {
            double angle = legMovement.compute(x, y, motor, Odrive);
            double motorCount = map(angle, -360, 360, -6000, 6000);
            setMotorPosition(Odrive, motor, motorCount);
          }
        }
        delay(100);
        disarmMotors();
        idlePosition = true;
      }
      break;

    case S_CALIBRATE:
      calibrateOdriveMotors();
      calibrated = true;
      changeStateTo(S_IDLE);
      break;

    case S_READY:
      break;

    case S_PAUSE:
      blinkLight(RED_LED);
      break;

    case S_WALK:
      if (walkIntervall.hasTimerExpired()) {
        // walkTime = micros();
        int Odrive = 0;
        double x = legMovement.stepX(n, LENGHT, FREQUENCY, PHASESHIFT0X);
        double y = legMovement.stepY(n, AMPLITUDEOVER, AMPLITUDEUNDER, HEIGHT, FREQUENCY, PHASESHIFT0Y);
        for (int motor = 0; motor < 2; motor++) {
          double angle = legMovement.compute(x, y, motor, Odrive);
          double motorCount = map(angle, -360, 360, -6000, 6000);
          setMotorPosition(Odrive, motor, motorCount);
        }
        Odrive = 1;
        x = legMovement.stepX(n, LENGHT, FREQUENCY, PHASESHIFT1X);
        y = legMovement.stepY(n, AMPLITUDEOVER, AMPLITUDEUNDER, HEIGHT, FREQUENCY, PHASESHIFT1Y);
        for (int motor = 0; motor < 2; motor++) {
          double angle = legMovement.compute(x, y, motor, Odrive);
          double motorCount = map(angle, -360, 360, -6000, 6000);
          setMotorPosition(Odrive, motor, motorCount);
        }
        Odrive = 2;
        x = legMovement.stepX(n, LENGHT, FREQUENCY, PHASESHIFT2X);
        y = legMovement.stepY(n, AMPLITUDEOVER, AMPLITUDEUNDER, HEIGHT, FREQUENCY, PHASESHIFT2Y);
        for (int motor = 0; motor < 2; motor++) {
          double angle = legMovement.compute(x, y, motor, Odrive);
          double motorCount = map(angle, -360, 360, -6000, 6000);
          setMotorPosition(Odrive, motor, motorCount);
        }
        Odrive = 3;
        x = legMovement.stepX(n, LENGHT, FREQUENCY, PHASESHIFT3X);
        y = legMovement.stepY(n, AMPLITUDEOVER, AMPLITUDEUNDER, HEIGHT, FREQUENCY, PHASESHIFT3Y);
        for (int motor = 0; motor < 2; motor++) {
          double angle = legMovement.compute(x, y, motor, Odrive);
          double motorCount = map(angle, -360, 360, -6000, 6000);
          setMotorPosition(Odrive, motor, motorCount);
        }
        n += 1;
        walkIntervall.startTimer(intervall);
        //  Serial.print("Walk Time: ");
        // Serial.println(micros() - loopTime);
      }

      break;

    case S_RUN:

      break;

    case S_JUMP:
      double x = 0;
      double y;
      switch (jump) {
        case 0:
          y = -80;

          if (!runned) {
            for (int Odrive = 0; Odrive < 4; Odrive ++) {
              for (int motor = 0; motor < 2; motor++) {
                double angle = legMovement.compute(x, y, motor, Odrive);
                double motorCount = map(angle, -360, 360, -6000, 6000);
                setMotorPosition(Odrive, motor, motorCount);
              }
            }
            groundTime.startTimer(500);
            runned = true;
          }
          if (groundTime.hasTimerExpired()) {
            jump = 1;
            runned = false;
          }
          break;

        case 1:
          y = -200;
          if (!runned) {
            for (int Odrive = 0; Odrive < 4; Odrive ++) {
              for (int motor = 0; motor < 2; motor++) {
                double angle = legMovement.compute(x, y, motor, Odrive);
                double motorCount = map(angle, -360, 360, -6000, 6000);
                setMotorPosition(Odrive, motor, motorCount);
              }
            }
            airTime.startTimer(1000);
            runned = true;
          }
          if (airTime.hasTimerExpired()) {
            jump = 0;
            runned = false;
          }
          break;
      }
      break;

    case S_AUTONOMOUS:

      break;

    case S_MANUAL:

      break;

    case S_BACKFLIP:

      break;

    case S_CONFIGURE:
      break;

    case S_RESET:
      //turnOffAllLights();
      checkForErrors();
      delay(200);
      resetMotorsErrors();
      //checkForErrors();
      //readConfig();
      //setPreCalibrated(true);
      //saveConfigOdrives();
      //delay(500);
      //rebootOdrives();
      //readConfig();
      changeStateTo(S_IDLE);

      break;

    case S_WARNING:
      blinkLight(ORANGE_LED);
      break;

    case S_ERROR:
      break;

    default:
      changeStateTo(S_IDLE);
      break;

  }
  readButtons();
  serial.flush();
  //Serial.print("Loop Time: ");
  //Serial.println(micros() - loopTime);
}

/**
  Generate a JSON document and sends it
  over Serial.
*/
void sendJSONDocumentToSerial() {
  DynamicJsonDocument doc(220);
  //TODO make correct JSON
  doc["state"] = currentState;
  serial.write(doc);
}

/**
  Reads a JSON document from serial
  and decode it.
*/
void readJSONDocumentFromSerial() {
  JsonObject obj = serial.read();
  //TODO decode JSON
  //recCommand = obj["command"];
}
