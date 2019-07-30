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
        GoalAnalyzer();
        bool accumulate(boolean state1, boolean state2);
    private:
        int FRONT_UP = 1;
        int FRONT_DOWN = 2;
        bool prevStates[2] = { false, false };
        int enablingOrder[2] = { 0, 0 };
        int disablingOrder[2] = { 0, 0 };

        bool findCascades(boolean state1, boolean state2);
        void registerCascade(int number, int event);
        long triggerTime;
};

// Indicator
class Display {
    public:
        Display(int powerPin, int *segmentPins);

        void writeDigit(int digit);
        void writeDigit(boolean *segmentValues);
        void setEnabled(boolean enabled);
        void setBlinkMode(bool enabled);

    private:
        int *segmentPins;
        int powerPin;
        bool blinkMode = false;
        unsigned long nextSwitchTime = 0;
        bool state = false;
        int blinkDelay = 300;

        //@formatter:off
        boolean digits[10][7] = {
                { true,  true,  true,  true,  true,  true,  false }, // 0
                { false, true,  true,  false, false, false, false }, // 1
                { true,  true,  false, true,  true,  false, true  }, // 2
                { true,  true,  true,  true,  false, false, true  }, // 3
                { false, true,  true,  false, false, true,  true  }, // 4
                { true,  false, true,  true,  false, true,  true  }, // 5
                { true,  false, true,  true,  true,  true,  true  }, // 6
                { true,  true,  true,  false, false, false, false }, // 7
                { true,  true,  true,  true,  true,  true,  true  }, // 8
                { true,  true,  true,  true,  false, true,  true  }, // 9
        };
        //@formatter:on
};

class ButtonPanel {
    public:
        ButtonPanel(int pin);
        int getKeyValue();
    private:
        int pin;
        int getButtonNumberByValue(int value);
        int values[4] = { 0, 615, 769, 1023 };
        int error = 15;         // ¬еличина отклонени€ от значений - погрешность

};

class Beeper {
    public:
        Beeper(int pin, int *tones[], int tonesLength);
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

int sign(int number);
int sign(double number);
