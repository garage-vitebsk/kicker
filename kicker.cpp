#include "kicker.h"

bool isDebug = false;

//-------BallDetector-------

BallDetector::BallDetector(int pin) {
    this->pin = pin;
    pinMode(pin, INPUT);
}

boolean BallDetector::getState() {
    return digitalRead(pin);
}

//-------GoalAnalyzer-------
GoalAnalyzer::GoalAnalyzer() {
    triggerTime = millis();
}

boolean GoalAnalyzer::accumulate(boolean state1, boolean state2) {
    if (!findCascades(state1, state2)) {
        return false;
    }
    triggerTime = millis();
    prevStates[0] = state1;
    prevStates[1] = state2;

#if DEBUG
        Serial.println("---------enablingOrder---------");
        for (int i = 0; i < 2; i++) {
            Serial.println(enablingOrder[i]);
        }

        Serial.println("--------disablingOrder----------");
        for (int i = 0; i < 2; i++) {
            Serial.println(disablingOrder[i]);
        }
        Serial.println("-------------------------------------------");
#endif
    long up1 = 0;
    long up2 = 0;
    long down1 = 0;
    for (int i = 0; i < 4; i++) {
      Event *event = events[(index - i) % 4];
      if (event->detectorNumber == 1) {
        if (event->type == FRONT_UP) {
          up1 = event->time;
        } else {
          down1 = event->time;
        }
      } else {
        if (event->type == FRONT_UP) {
          up2 = event->time;
        }
      }
    }
    bool goal = (up2 - up1 < 500) && (down1 - up1 < 500)&& (up2 - down1 < 500);
    if (goal) {
      index = 0;
      for (int i = 0; i < 4; i++) {
        delete events[i];
        events[i] = NULL;
      }
    }
    return goal;
//    if (enablingOrder[0] == 1 && enablingOrder[1] == 2 && disablingOrder[0] == 1
//            && disablingOrder[1] == 2) {
//        enablingOrder[0] = 0;
//        enablingOrder[1] = 0;
//        disablingOrder[0] = 0;
//        disablingOrder[1] = 0;
//        return true;
//    }
//    if (enablingOrder[0] != 0 && enablingOrder[1] != 0 && disablingOrder[0] != 0
//            && disablingOrder[1] != 0) {
//        enablingOrder[0] = 0;
//        enablingOrder[1] = 0;
//        disablingOrder[0] = 0;
//        disablingOrder[1] = 0;
//    }
//
//    return false;
}

boolean GoalAnalyzer::findCascades(boolean state1, boolean state2) {
//    if (millis() - triggerTime > 500) {
//        enablingOrder[0] = 0;
//        enablingOrder[1] = 0;
//        disablingOrder[0] = 0;
//        disablingOrder[1] = 0;
//#if DEBUG
//            Serial.println("---------clear by timeout---------");
//#endif
//    }
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

void GoalAnalyzer::registerCascade(byte number, byte type) {
    Event *event = new Event;
    event->detectorNumber = number;
    event->type = type;
    event->time = millis();
    events[index] = event;
    index = (index + 1) % 4;
}

//-------Display-------
Display::Display(int *segmentPins) {
    for (int i = 0; i < 4; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }
}

void Display::writeDigit(int digit) {
    writeDigit(digits[digit]);
}

void Display::writeDigit(boolean *segmentValues) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(segmentPins[i], segmentValues[i]);
    }
}

// ButtonPanel
Button::Button(int pin) {
    this->pin = pin;
    prevPressedTime = millis();
    pinMode(pin, INPUT);
}

boolean Button::readButton(){
  boolean state = digitalRead(pin);
  Serial.println(state);
  if (state && !prevState) {
    long pressedTime = millis();
    if (pressedTime - prevPressedTime > PRESS_THRESHOLD) {
      prevPressedTime = pressedTime;
      prevState = state;
      return true;
    }
  }
  prevState = state;
  return false;
}

// Beeper
Beeper::Beeper(int pin, int *tones[], int tonesLength) {
    this->pin = pin;
    this->tones = tones;
    this->tonesLength = tonesLength;
}

void Beeper::setPlay(bool play) {
    this->play = play;
}

void Beeper::work() {
    if (!play) {
        return;
    }
    if (currentTone >= tonesLength - 1) {
        play = false;
        currentTone = 0;
//        digitalWrite(pin, false);
    }
    unsigned long currentTime = millis();
    if (currentTime > nextSwichTime) {
        nextSwichTime = millis + tones[currentTone][1];
        tone(pin, tones[currentTone][0], tones[currentTone][1]);
        currentTone++;
    }
}

//-----other functions-----
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
