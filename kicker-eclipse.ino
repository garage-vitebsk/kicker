#include <Arduino.h>
#include "kicker.h"

#define DEBUG true

GoalAnalyzer analyzer = GoalAnalyzer();

Player player1 = Player(1,
                        new Display(new int[4] {4, 5, 6, 7}),
                        new BallDetector(9),
                        new BallDetector(10),
                        new Button(2),
                        new Button(3)
                       );

// player 2
//int segments2[4] = { 19, 18, 17, 16};
//Display display2 = Display(segments2);
//BallDetector detector21 = BallDetector(11);
//BallDetector detector22 = BallDetector(12);
//Button increment2 = Button(14);
//Button decrement2 = Button(15);

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
int *winTones[] = {winTones1, winTones2, winTones3, winTones4,winTones5,winTones6,winTones7,winTones8,winTones9,winTones10,winTones11};
Beeper winBeeper = Beeper(8, 11, winTones);

boolean win = false;
boolean goal = false;

void showScore() {
  player1.display->writeDigit(player1.score);
  //  display2.writeDigit(digit1);
}

void restartGame() {
  win = false;
  player1.score = 0;
  //    display1.setBlinkMode(false);
}

void checkButtons(Player player) {
  boolean incPressed = player.increment->readButton();
  boolean decPressed = player.decrement->readButton();
  if (incPressed && decPressed) {
#if DEBUG
    Serial.print("Game was restarted by player ");
    Serial.println(player.number);
#endif
    restartGame();
  } else if (incPressed && player1.score < 9 && !win) {
#if DEBUG
    Serial.print("Score was increased by player ");
    Serial.println(player.number);
#endif
    player1.score++;
  } else if (decPressed && player1.score > 0 && !win) {
#if DEBUG
    Serial.print("Score was dereased by player ");
    Serial.println(player.number);
#endif
    player1.score--;
  }
}

void activeDelay() {
  do {
    checkButtons(player1);
    showScore();
    goalBeeper.work();
    goal = analyzer.accumulate(player1.detector1->getState(), player1.detector2->getState());
  } while (!(goal || win));
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (win) {
    //    display1.setBlinkMode(true);
    winBeeper.work();
    checkButtons(player1);
    showScore();
  }
  activeDelay();
  if (goal) {
    goal = false;
    if (player1.score >= 9) {
      win = true;
      winBeeper.setPlay(true);
    } else {
      goalBeeper.setPlay(true);
      player1.score++;
    }
  }
}
