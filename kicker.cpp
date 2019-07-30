#include "kicker.h"

bool isDebug = false;

//-------BallDetector-------

BallDetector::BallDetector(int pin) {
    this->pin = pin;
    pinMode(pin, INPUT);
}

boolean BallDetector::getState() {
    return digitalRead(pin);
}

//-------GoalAnalyzer-------
GoalAnalyzer::GoalAnalyzer() {
    triggerTime = millis();
}

boolean GoalAnalyzer::accumulate(boolean state1, boolean state2) {
    if (!findCascades(state1, state2)) {
        return false;
    }
    triggerTime = millis();
    prevStates[0] = state1;
    prevStates[1] = state2;

    if (isDebug) {
        Serial.println("---------enablingOrder---------");
        for (int i = 0; i < 2; i++) {
            Serial.println(enablingOrder[i]);
        }

        Serial.println("--------disablingOrder----------");
        for (int i = 0; i < 2; i++) {
            Serial.println(disablingOrder[i]);
        }
        Serial.println("-------------------------------------------");
    }

    //TODO find which situation is goal
    if (enablingOrder[0] == 1 && enablingOrder[1] == 2 && disablingOrder[0] == 1
            && disablingOrder[1] == 2) {
        enablingOrder[0] = 0;
        enablingOrder[1] = 0;
        disablingOrder[0] = 0;
        disablingOrder[1] = 0;
        return true;
    }
    if (enablingOrder[0] != 0 && enablingOrder[1] != 0 && disablingOrder[0] != 0
            && disablingOrder[1] != 0) {
        enablingOrder[0] = 0;
        enablingOrder[1] = 0;
        disablingOrder[0] = 0;
        disablingOrder[1] = 0;
    }

    return false;
}

boolean GoalAnalyzer::findCascades(boolean state1, boolean state2) {
    if (millis() - triggerTime > 500) {
        enablingOrder[0] = 0;
        enablingOrder[1] = 0;
        disablingOrder[0] = 0;
        disablingOrder[1] = 0;
        if (isDebug) {
            Serial.println("---------clear by timeout---------");
        }
    }
    boolean found = false;
    boolean old1 = prevStates[0];
    if (state1 && !old1) {
        found = true;
        registerCascade(1, FRONT_UP);
    }
    if (!state1 && old1) {
        found = true;
        registerCascade(1, FRONT_DOWN);
    }
    boolean old2 = prevStates[1];
    if (state2 && !old2) {
        found = true;
        registerCascade(2, FRONT_UP);
    }
    if (!state2 && old2) {
        found = true;
        registerCascade(2, FRONT_DOWN);
    }
    return found;
}

void GoalAnalyzer::registerCascade(int number, int event) {
    if (event == FRONT_UP) {
        if (enablingOrder[0] == 0) {
            enablingOrder[0] = number;
        } else {
            enablingOrder[1] = number;
        }
    } else {
        if (disablingOrder[0] == 0) {
            disablingOrder[0] = number;
        } else {
            disablingOrder[1] = number;
        }
    }
}

//-------Display-------
Display::Display(int powerPin, int *segmentPins) {
    this->powerPin = powerPin;
    pinMode(powerPin, OUTPUT);
    this->segmentPins = segmentPins;
    for (int i = 0; i < 7; i++) {
        pinMode(segmentPins[i], OUTPUT);
    }
}

void Display::writeDigit(int digit) {
    writeDigit(digits[digit]);
}

void Display::writeDigit(boolean *segmentValues) {
    for (int i = 0; i < 7; i++) {
        digitalWrite(segmentPins[i], segmentValues[i]);
    }
}

void Display::setEnabled(boolean enabled) {
    unsigned long currentTime = millis();
    if (blinkMode && currentTime > nextSwitchTime) {
        state = enabled;
        nextSwitchTime = currentTime + blinkDelay;
        digitalWrite(powerPin, !enabled);
    }
    if (!blinkMode || state) {
        digitalWrite(powerPin, !enabled);
    }
}

void Display::setBlinkMode(bool mode) {
    blinkMode = mode;
}

// ButtonPanel
ButtonPanel::ButtonPanel(int pin) {
    this->pin = pin;
}

int ButtonPanel::getKeyValue() {         // ������� ����������� �������
    static int count;
    static int oldKeyValue; // ���������� ��� �������� ����������� �������� ��������� ������
    static int innerKeyValue;

    // ����� ��� �� ����� ������������ �������� ���, ��� ��� ��� ��������� �������� � ���� �����������
    int actualKeyValue = getButtonNumberByValue(analogRead(pin)); // ��������������� ��� � ����� ������, ��� ����� ������ �����������

    if (innerKeyValue != actualKeyValue) { // ������ �������� �������� �� �����������
        count = 0;                     // ��� �������� � �������� ������� ������
        innerKeyValue = actualKeyValue;       // ���������� ����� ��������
    } else {
        count += 1;                           // ����������� �������
    }

    if ((count >= 5) && (actualKeyValue != oldKeyValue)) {
        oldKeyValue = actualKeyValue;         // ���������� ����� ��������
    }
    return oldKeyValue;
}

int ButtonPanel::getButtonNumberByValue(int value) { // ����� ������� �� �������������� ���� ������� ������ � � �����
    for (int i = 0; i <= 3; i++) {
        // ���� �������� � �������� ��������� values[i]+/-error - �������, ��� ������ ����������
        if (value <= values[i] + error && value >= values[i] - error)
            return i;
    }
    return -1;                    // �������� �� ����������� ��������� ���������
}

// Beeper
Beeper::Beeper(int pin, int *tones[], int tonesLength) {
    this->pin = pin;
    this->tones = tones;
    this->tonesLength = tonesLength;
}

void Beeper::setPlay(bool play) {
    this->play = play;
}

void Beeper::work() {
    if (!play) {
        return;
    }
    if (currentTone >= tonesLength - 1) {
        play = false;
        currentTone = 0;
//        digitalWrite(pin, false);
    }
    unsigned long currentTime = millis();
    if (currentTime > nextSwichTime) {
        nextSwichTime = millis + tones[currentTone][1];
        tone(pin, tones[currentTone][0], tones[currentTone][1]);
        currentTone++;
    }
}

//-----other functions-----
int sign(int number) {
    if (number < 0) {
        return -1;
    }
    return 1;
}

int sign(double number) {
    if (number < 0) {
        return -1;
    }
    return 1;
}
