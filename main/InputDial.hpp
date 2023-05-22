namespace InputDial {

enum class TurnDirection {
  Stationary, Left, Right, Unclear
};

/**
 * This class reads and interprets the state of the pins
 * that are meant to connect to the input dial.
 */
class PinReader {
private:
  bool latestClk;
  bool priorClk;
public:
  PinReader(void) {
    // Pin 6 is the GND (ground) pin, so it stays low.
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
    // Pin 7 is the positive pin, so it stays high.
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);

    pinMode(8,  INPUT_PULLUP);
    pinMode(9,  INPUT);
    pinMode(10, INPUT);

    this->latestClk = digitalRead(10);
    this->priorClk = this->latestClk;
  }

  bool isDialPressed(void) {
    return !digitalRead(8);
  }

  TurnDirection getTurnDirection(void) {
    this->priorClk = this->latestClk;
    this->latestClk = digitalRead(10);
    if (this->latestClk != this->priorClk) {
      if (digitalRead(9) != this->latestClk) {
        return TurnDirection::Left;
		  } else {
        return TurnDirection::Right;
      }
    } else {
      return TurnDirection::Stationary;
    }
  }
};

}
