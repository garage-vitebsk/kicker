#include "kicker.h"


//-------BallDetector-------

BallDetector::BallDetector(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
};

boolean BallDetector::getState() {
  return digitalRead(pin);
}


//-------GoalAnalyzer-------
boolean GoalAnalyzer::accumulate(boolean state1, boolean state2) {
  if (!findCascades(state1, state2)) {
    return false;
  }
  prevStates[0] = state1;
  prevStates[1] = state2;
  //TODO find which situation is goal
  if (enablingOrder[0] == 1 && enablingOrder[1] == 2 && disablingOrder[0] == 1 && disablingOrder[2] == 2) {
    return true;
  }
  if (enablingOrder[0] != 0 && enablingOrder[1] != 0 && disablingOrder[0] != 0 && disablingOrder[1] != 0) {
    enablingOrder[0] = 0;
    enablingOrder[1] = 0;
    disablingOrder[0] = 0;
    disablingOrder[1] = 0;
  }
//  Serial.print(enablingOrder);
//  Serial.println(disablingOrder);
  return false;
}

boolean GoalAnalyzer::findCascades(boolean state1, boolean state2) {
  boolean found = false;
  boolean old1 = prevStates[0];
  if (state1 && !old1) {
    found = true;
    registerCascade(1, FRONT_UP);
  }
  if (!state1 && old1) {
    found = true;
    registerCascade(1, FRONT_DOWN);
  }
  boolean old2 = prevStates[1];
  if (state2 && !old2) {
    found = true;
    registerCascade(2, FRONT_UP);
  }
  if (!state2 && old2) {
    found = true;
    registerCascade(2, FRONT_DOWN);
  }
  return found;
}

void GoalAnalyzer::registerCascade(int number, int event) {
  if (event == FRONT_UP) {
    if (enablingOrder[0] == 0) {
      enablingOrder[0] = number;
    } else {
      enablingOrder[1] = number;
    }
  } else {
    if (disablingOrder[0] == 0) {
      disablingOrder[0] = number;
    } else {
      disablingOrder[1] = number;
    }
  }
}
//-------Display-------
Display::Display(int powerPin, int* segmentPins) {
  this->powerPin = powerPin;
  pinMode(powerPin, OUTPUT);
  this->segmentPins = segmentPins;
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void Display::writeDigit(int digit) {
  writeDigit(digits[digit]);
}

void Display::writeDigit(boolean* segmentValues) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], segmentValues[i]);
  }
}

void Display::setEnabled(boolean enabled) {
  digitalWrite(powerPin, enabled);
}
//-----прочие функции-----
int sign(int number) {
  if (number < 0) {
    return -1;
  }
  return 1;
}

int sign(double number) {
  if (number < 0) {
    return -1;
  }
  return 1;
}
