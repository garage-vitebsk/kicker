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
        //@formatter:on
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
        Beeper(int pin, int tonesLength, int  *tones[]);
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
  public:
    byte number;
    Display *display;
    BallDetector *detector1;
    BallDetector *detector2;
    Button *increment;
    Button *decrement;
    byte score;

    Player(byte number, Display *display, BallDetector *detector1, BallDetector *detector2, Button *increment, Button *decrement);
};

int sign(int number);
int sign(double number);
