#include <Arduino.h>

// класс детектора шара
class BallDetector {
  public:
    BallDetector(int pin);
    boolean getState();

  private:
    int pin; // пин, на котором висит датчик
};

class GoalAnalyzer {
  public:
    boolean accumulate(boolean state1, boolean state2);
  private:
    int FRONT_UP = 0;
    int FRONT_DOWN = 1;
    boolean prevStates[2] = {false, false};
    int enablingOrder[2] = {0, 0};
    int disablingOrder[2] = {0, 0};

    boolean findCascades(boolean state1, boolean state2);
    void registerCascade(int number, int event);
};

// класс 8-сегментного индикатора
class Display {
  public:
    Display(int powerPin, int* segmentPins);

    void writeDigit(int digit);
    void setEnabled(boolean enabled);

  private:
    int* segmentPins;
    int powerPin;

    boolean digits[10][7] = {
      {true,  true,  true,  true,  true,  true,  false}, // 0
      {false, true,  true,  false, false, false, false}, // 1
      {true,  true,  false, true,  true,  false, true }, // 2
      {true,  true,  true,  true,  false, false, true }, // 3
      {false, true,  true,  false, false, true,  true }, // 4
      {true,  false, true,  true,  false, true,  true }, // 5
      {true,  false, true,  true,  true,  true,  true }, // 6
      {true,  true,  true,  false, false, false, false}, // 7
      {true,  true,  true,  true,  true,  true,  true }, // 8
      {true,  true,  true,  true,  false, true,  true }, // 9
    };

    void writeDigit(boolean* segmentValues);
};

int sign(int number);
int sign(double number);
