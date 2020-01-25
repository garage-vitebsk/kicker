#include <Arduino.h>
#include "kicker.h"

Player player1 = Player(1,
                        new Display(new int[4] {4, 5, 6, 7}),
                        new BallDetector(9),
                        new BallDetector(10),
                        new Button(2),
                        new Button(3)
                       );

Player player2 = Player(2,
                        new Display(new int[4] {19, 18, 17, 16}),
                        new BallDetector(11),
                        new BallDetector(12),
                        new Button(14),
                        new Button(15)
                       );

int goalTones1[] = { 1568, 150 };
int goalTones2[] = { 1480, 150 };
int goalTones3[] = { 1396, 150 };
int goalTones4[] = { 1244, 300 };
int *goalTones[] = { goalTones1, goalTones2, goalTones3, goalTones4 };

Beeper goalBeeper = Beeper(8, 4, goalTones);
int winTones1[] = { 1568, 150 };
int winTones2[] = { 1480, 150 };
int winTones3[] = { 1396, 150 };
int winTones4[] = { 1244, 500 };
int winTones5[] = { 1318, 150 };
int winTones6[] = { 830,  300 };
int winTones7[] = { 880, 150  };
int winTones8[] = { 1046, 150 };
int winTones9[] = { 880, 300 };
int winTones10[] = { 1046, 150 };
int winTones11[] = { 1174, 150 };
int *winTones[] = {winTones1, winTones2, winTones3, winTones4, winTones5, winTones6, winTones7, winTones8, winTones9, winTones10, winTones11};
Beeper winBeeper = Beeper(8, 11, winTones);

boolean win = false;

void restartGame() {
  win = false;
  player1.restart();
  player2.restart();
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  player1.checkButtons();
  player2.checkButtons();
  if (player1.isRestartPressed() || player2.isRestartPressed()) {
    restartGame();
  }
  player1.showScore();
  player2.showScore();
  if (win) {
    //    display1.setBlinkMode(true);
    winBeeper.work();
  } else {
    goalBeeper.work();
    boolean goal1 = player1.checkGoal();
    boolean goal2 = player2.checkGoal();
    if (goal1 || goal2) {
      if (player1.isWon() || player2.isWon()) {
        win = true;
        player1.blockButtons();
        player2.blockButtons();
        winBeeper.setPlay(true);
      } else {
        goalBeeper.setPlay(true);
      }
    }
  }
}
