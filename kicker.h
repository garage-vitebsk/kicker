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
        byte FRONT_UP = 1;
        byte FRONT_DOWN = 2;
        bool prevStates[2] = { false, false };
        byte index = 0;
        
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
        void writeDigit(boolean *segmentValues);

    private:
        int *segmentPins;

        //@formatter:off
        boolean digits[10][4] = {
                { true,  true,  true,  true  }, // 0
                { false, true,  true,  true  }, // 1
                { true,  false, true,  true  }, // 2
                { false, false, true,  true  }, // 3
                { true,  true,  false, true  }, // 4
                { false, true,  false, true  }, // 5
                { true,  false, false, true  }, // 6
                { false, false, false, true  }, // 7
                { true,  true,  true,  false }, // 8
                { false, true,  true,  false }, // 9
        };
        //@formatter:on
};

class Button {
    #define PRESS_THRESHOLD 100
    public:
        Button(int pin);
        boolean readButton();
    private:
        boolean prevState;
        long prevPressedTime;
        int pin;
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
