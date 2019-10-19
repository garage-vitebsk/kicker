#include <Arduino.h>
#include "kicker.h"

#define DEBUG true

const int SPEAKER_PIN = 13;

int segments[7] = { 2, 3, 4, 5, 6, 7, 8 };
Display display1 = Display(9, segments);
Display display2 = Display(10, segments);

BallDetector detector1 = BallDetector(11);
BallDetector detector2 = BallDetector(12);

GoalAnalyzer analyzer = GoalAnalyzer();
ButtonPanel buttons = ButtonPanel(0);

int note1[] = { 1568, 150 };
int note2[] = { 1480, 150 };
int note3[] = { 1396, 150 };
int note4[] = { 1244, 300 };

int *notes[] = { note1, note2, note3, note4 };
//int **GOAL_NOTES = {, , , ,
//    {   1318, 300}, {830, 150}, {880, 150}, {1046, 300}, {880, 150},
//    {   1046, 150}, {1174, 150}};
Beeper beeper = Beeper(13, notes, 4);

int digit1, digit2;
boolean enter1, enter2;
boolean prev1, prev2;
int prevPressed = 0;
boolean win = false;
boolean goal = false;

void showScore() {
    display2.setEnabled(false);
    display1.setEnabled(true);
    display1.writeDigit(digit1);
    delay(10);
    display1.setEnabled(false);
    display2.setEnabled(true);
    display2.writeDigit(digit2);
    delay(10);
}

void restartGame() {
    win = false;
    digit1 = 0;
    digit2 = 0;
    display1.setBlinkMode(false);
}

void checkButtons() {
    int pressed = buttons.getKeyValue();
    if (pressed != prevPressed) {
        Serial.println(pressed);
        switch (pressed) {
        case 1:
            if (digit1 >= 9) {
                win = true;
            } else if (!win) {
                digit1++;
            }
            break;
        case 2:
            if (digit2 >= 9) {
                win = true;
            } else if (!win) {
                digit2++;
            }
            break;
        case 3:
            restartGame();
            break;
        }
        prevPressed = pressed;
    }
}

void activeDelay() {
    do {
        checkButtons();
        showScore();
        beeper.work();
        goal = analyzer.accumulate(detector1.getState(), detector2.getState());
    } while (!(goal || win));
}

void goalBeeper() {
    beeper.work();
//    unsigned long tone_time = millis();
//    for (int i = 0; i < GOAL_NOTES; i++) {
//        while (tone_time + GOAL_NOTES[i][2] < millis()) {
//            tone(SPEAKER_PIN, GOAL_NOTES[i][1], GOAL_NOTES[i][2]);
//            showScore();
//        }
//    }
//    tone(SPEAKER_PIN, 1568, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1480, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1396, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1244, 300);
//    delay(300);
//    tone(SPEAKER_PIN, 1318, 150);
//    delay(300);
//    tone(SPEAKER_PIN, 830, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 880, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1046, 150);
//    delay(300);
//    tone(SPEAKER_PIN, 880, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1046, 150);
//    delay(150);
//    tone(SPEAKER_PIN, 1174, 150);
//    delay(150);
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    while (win) {
        display1.setBlinkMode(true);
        checkButtons();
        showScore();
    }
    activeDelay();
    if (goal) {
        goal = false;
        if (digit1 >= 9) {
            digit1 = 0;
            win = true;
        } else {
            beeper.setPlay(true);
            goalBeeper();
            digit1++;
        }
    }
}
