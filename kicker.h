#include <Arduino.h>

// ball detector class
class BallDetector {
  public:
    BallDetector(int pin);
    bool getState();

  private:
    int pin; // sensor pin
};

class GoalAnalyzer {
  public:
    GoalAnalyzer(BallDetector *detector1, BallDetector *detector2);
    bool accumulate();
  private:
    static const byte FRONT_UP = 1;
    static const byte FRONT_DOWN = 2;

    bool prevStates[2] = { false, false };
    byte index = 0;
    BallDetector *detector1;
    BallDetector *detector2;

    bool findCascades(boolean state1, boolean state2);
    void registerCascade(byte number, byte event);
    long triggerTime;

    struct Event {
      byte detectorNumber;
      byte type;
      long time;
    } *events[4];
};

// Indicator
class Display {
  public:
    Display(int *segmentPins);

    void writeDigit(int digit);
  private:
    boolean digits[10][4] = {
      { false, false, false, false  }, // 0
      { true,  false, false, false  }, // 1
      { false, true,  false, false  }, // 2
      { true,  true,  false, false  }, // 3
      { false, false, true,  false  }, // 4
      { true,  false, true,  false  }, // 5
      { false, true,  true,  false  }, // 6
      { true,  true,  true,  false  }, // 7
      { false, false, false, true   }, // 8
      { true,  false, false, true   }, // 9
    };

    int *segmentPins;
    void writeDigit(boolean *segmentValues);
};

class Button {
#define PRESS_THRESHOLD 100
  public:
    Button(int pin);
    boolean readButton();
  private:
    boolean prevState = false;
    long prevPressedTime;
    int pin;
};

class Beeper {
  public:
    Beeper(int pin, int tonesLength, int **tones);
    void setPlay(bool play);
    void work();
  private:
    int pin;
    int **tones;
    int tonesLength;
    unsigned long nextSwichTime = 0;
    int currentTone = 0;
    bool play = false;
};

class Player {
  private:
    byte number;
    Display *display;
    Button *increment;
    Button *decrement;
    byte score;
    boolean restartPressed;
    GoalAnalyzer *analyzer;

  public:
    Player(byte number, Display *display, BallDetector *detector1, BallDetector *detector2, Button *increment, Button *decrement);

    void showScore();
    void checkButtons();
    boolean checkGoal();
    void restart();
    boolean isWon();
    boolean isRestartPressed();
};

int sign(int number);
int sign(double number);
