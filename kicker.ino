#include "kicker.h"

int segments[7] = {2, 3, 4, 5, 6, 7, 8};
Display display1 = Display(9, segments);
Display display2 = Display(10, segments);

BallDetector detector1 = BallDetector(11);
BallDetector detector2 = BallDetector(12);

GoalAnalyzer analyzer = GoalAnalyzer();

int digit1, digit2;
boolean enter1, enter2;
boolean prev1, prev2;
boolean win = false;
boolean goal = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (win) {
    display1.writeDigit(0);
    display2.setEnabled(false);
    display1.setEnabled(true);
    delay(300);
    display1.setEnabled(false);
    delay(300);
  }
  activeDelay();
  goal = false;
  if (digit1 == 9) {
    win = true;
  } else {
    digit1++;
  }
}

void activeDelay() {
  do {
    display2.setEnabled(false);
    display1.setEnabled(true);
    display1.writeDigit(digit1);
    delay(10);
    display1.setEnabled(false);
    display2.setEnabled(true);
    display2.writeDigit(digit2);
    delay(10);
    goal = analyzer.accumulate(detector1.getState(), detector2.getState());
  } while (!(goal));
}
