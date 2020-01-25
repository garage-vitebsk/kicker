#include "kicker.h"

#define DEBUG false

//-------BallDetector-------

BallDetector::BallDetector(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT);
}

boolean BallDetector::getState() {
  return digitalRead(pin);
}

//-------GoalAnalyzer-------
GoalAnalyzer::GoalAnalyzer(BallDetector *detector1, BallDetector *detector2) {
  this->detector1 = detector1;
  this->detector2 = detector2;
  triggerTime = millis();
  goalTime = millis();
  for (int i = 0; i < 4; i++) {
    events[i] = NULL;
  }
}

boolean GoalAnalyzer::accumulate() {
  boolean state1 = detector1->getState();
  boolean state2 = detector2->getState();
  if (!findCascades(state1, state2)) {
    return false;
  }
  triggerTime = millis();
  prevStates[0] = state1;
  prevStates[1] = state2;

  long up1 = 0;
  long up2 = 0;
  long down1 = 0;
#if DEBUG
  Serial.print("AnazierState:");
#endif
  for (int i = 0; i < 4; i++) {
    Event *event = events[(i + index + 1) % 4];
#if DEBUG
    Serial.print("[index=");
    Serial.print((i + index + 1) % 4);
    Serial.print(",&");
    Serial.print((unsigned)event);
#endif
    if (event != NULL) {
#if DEBUG
      Serial.print(",number=");
      Serial.print(event->detectorNumber);
      Serial.print(",type=");
      Serial.print(event->type == FRONT_UP ? "FRONT_UP" : "FRONT_DOWN");
      Serial.print(",time=");
      Serial.print(event->time);
#endif
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
#if DEBUG
    Serial.print("]");
#endif
  }
#if DEBUG
  Serial.println();
#endif
  bool goal = (up2 - up1 < 500) && (down1 - up1 < 500) && (up2 - down1 < 500);
  if (goal) {
    index = 0;
#if DEBUG
    Serial.print("deleting:");
#endif
    for (int i = 0; i < 4; i++) {
#if DEBUG
      Serial.print(" [");
      Serial.print(i);
      Serial.print("] &");
      Serial.print((unsigned)events[i]);
#endif
      if (events[i] != NULL) {
        delete events[i];
        events[i] = NULL;
      }
    }
#if DEBUG
    Serial.println();
#endif
    long curTime = millis();
    long timeSinceLastGoal = curTime - goalTime;
    goalTime = curTime;
#if DEBUG
    Serial.print("timeSinceGoal");
    Serial.println(timeSinceLastGoal);
#endif
    if (timeSinceLastGoal > GOAL_THRESHOLD) {
      return true;
    }
  }
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

void GoalAnalyzer::registerCascade(byte number, byte type) {
  Event *event = new Event;
#if DEBUG
  Serial.print("Event address: &");
  Serial.println((unsigned)event);
#endif
  event->detectorNumber = number;
  event->type = type;
  event->time = millis();
  if (events[index] != NULL) {
    delete events[index];
  }
  events[index] = event;
  index = (index + 1) % 4;
}

//-------Display-------
Display::Display(int *segmentPins) {
  this->segmentPins = segmentPins;
  for (int i = 0; i < 4; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  this->disabledTime = millis();
}

void Display::writeDigit(int digit) {
  if (blinkMode) {
    long curTime = millis();
    if (curTime - disabledTime > BLINK_TIME) {
      disabledTime = curTime;
      enabled = !enabled;
    }
  }
  if (!enabled) {
    writeDigit(digits[10]);
  } else if (digit < 0) {
    writeDigit(digits[0]);
  } else if (digit > 9) {
    writeDigit(digits[9]);
  } else {
    writeDigit(digits[digit]);
  }
}

void Display::writeDigit(boolean *segmentValues) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(segmentPins[i], segmentValues[i]);
  }
}

void Display::setBlinkMode(boolean enabled) {
  blinkMode = enabled;
}

// ButtonPanel
Button::Button(int pin) {
  this->pin = pin;
  pressedTime = millis();
  pinMode(pin, INPUT);
}

void Button::work() {
  //  delay(100);
  prevState = state;
  state = digitalRead(pin);
  if (state && !prevState) {
    pressedTime = millis();
  }
}

boolean Button::isPressed() {
  long pressTimeDif = pressedTime - prevPressedTime;
  prevPressedTime = pressedTime;
  return state && !prevState && pressTimeDif > PRESS_THRESHOLD;
}

boolean Button::isHold() {
  return state && millis() - pressedTime > HOLD_THRESHOLD;
}

// Beeper
Beeper::Beeper(int pin, int tonesLength, int **tones) {
  this->pin = pin;
  this->tones = tones;
  this->tonesLength = tonesLength;
}

void Beeper::setPlay(bool play) {
  this->play = play;
}

void Beeper::work() {
  if (!play) {
    noTone(pin);
    return;
  }
  if (currentTone >= tonesLength - 1) {
    play = false;
    currentTone = 0;
  }
  unsigned long currentTime = millis();
  if (currentTime > nextSwichTime) {
    nextSwichTime = currentTime + tones[currentTone][1];
    tone(pin, tones[currentTone][0]);
    currentTone++;
  }
}

// Player
Player::Player(byte number, Display *display, BallDetector *detector1, BallDetector *detector2, Button *increment, Button *decrement) {
  this->number = number;
  this->display = display;
  this->increment = increment;
  this->decrement = decrement;
  this->score = 0;
  this->restartPressed = false;
  this->analyzer = new GoalAnalyzer(detector1, detector2);
}

void Player::showScore() {
  display->writeDigit(score);
}

void Player::checkButtons() {
  increment->work();
  decrement->work();
  if (increment->isHold() || decrement->isHold()) {
#if DEBUG
    Serial.print("Game was restarted by player ");
    Serial.println(number);
#endif
    restartPressed = true;
  } else if (increment->isPressed() && score < 9 && !buttonsBlocked) {
#if DEBUG
    Serial.print("Score was increased by player ");
    Serial.println(number);
#endif
    score++;
  } else if (decrement->isPressed() && score > 0 && !buttonsBlocked) {
#if DEBUG
    Serial.print("Score was dereased by player ");
    Serial.println(number);
#endif
    score--;
  }
}

boolean Player::checkGoal() {
  boolean goal = analyzer->accumulate();
  if (goal) {
    score++;
  }
  return goal;
}

void Player::restart() {
  restartPressed = false;
  buttonsBlocked = false;
  display->setBlinkMode(false);
  score = 0;
}

boolean Player::isWon() {
  return score >= 10;
}

boolean Player::isRestartPressed() {
  return restartPressed;
}

void Player::blockButtons() {
  display->setBlinkMode(true);
  buttonsBlocked = true;
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
