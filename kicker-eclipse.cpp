#include <Arduino.h>
#include "kicker.h"

const int SPEAKER_PIN = 13;

int segments[7] = { 2, 3, 4, 5, 6, 7, 8 };
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

void goal_beeper() {
  tone(SPEAKER_PIN, 1568, 150);
  delay(150);
  tone(SPEAKER_PIN, 1480, 150);
  delay(150);
  tone(SPEAKER_PIN, 1396, 150);
  delay(150);
  tone(SPEAKER_PIN, 1244, 300);
  delay(300);
  tone(SPEAKER_PIN, 1318, 150);
  delay(300);
  tone(SPEAKER_PIN, 830, 150);
  delay(150);
  tone(SPEAKER_PIN, 880, 150);
  delay(150);
  tone(SPEAKER_PIN, 1046, 150);
  delay(300);
  tone(SPEAKER_PIN, 880, 150);
  delay(150);
  tone(SPEAKER_PIN, 1046, 150);
  delay(150);
  tone(SPEAKER_PIN, 1174, 150);
  delay(150);
}

void setup() {
	Serial.begin(9600);
}

void loop() {
	while (win) {
		display1.writeDigit(0);
		display2.writeDigit(0);
		display2.setEnabled(true);
		display1.setEnabled(true);
		delay(300);
		display1.setEnabled(false);
		display2.setEnabled(false);
		delay(300);
	}
	activeDelay();
	goal = false;
	if (digit1 == 9) {
		win = true;
	} else {
		goal_beeper();
		digit1++;
	}
}

