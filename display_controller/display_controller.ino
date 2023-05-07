enum class LedState : boolean { Dark = LOW, Bright = HIGH };
LedState operator!(LedState state) {
  return (state == LedState::Dark) ? LedState::Bright : LedState::Dark;
}

constexpr uint8_t RowCount = 6;
constexpr uint8_t ColumnCount = 5;

class DisplayController {
private:
  LedState frameBuffer[30];
public:
  DisplayController(void) {
    for (uint8_t i = 0; i < 30; i++) {
      this->frameBuffer[i] = LedState::Dark;
    }
    // Pins A0 through A4 correspond to columns; pins 0 through 5 correspond to rows.
    for (auto pin = A0; pin < A5; pin++) {
      pinMode(pin, OUTPUT);
      /* The column pins connect to the negative terminals of the LEDs, so setting them HIGH
       * will turn them OFF. */
      digitalWrite(pin, HIGH);
    }
    for (auto pin = 0; pin < 5; pin++) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
  }

  void setLedState(const uint8_t col, const uint8_t row, LedState state) {
    // The constant 5 here means the number of columns.
    this->frameBuffer[col + row * 5] = state;
  }

  LedState getLedState(const uint8_t col, const uint8_t row) const {
    return this->frameBuffer[col + row * 5];
  }

  void refresh(const unsigned int delayTimeMs) {
    for (unsigned int elapsedTime = 0; elapsedTime < delayTimeMs; elapsedTime += 12) {
      for (uint8_t row = 0; row < 6; row++) {
        digitalWrite(row, HIGH);
        for (uint8_t col = 0; col < 5; col++) {
          digitalWrite(A0 + col, (boolean)!this->getLedState(col, row));
        }
        /* For the red rows, we wait 4 milliseconds; for the green and blue rows,
         * we wait 1 millisecond. This helps balance out the brightnesses of the
         * different colors.
         */
        if (row == 0 || row == 3) {
          delay(4);
        } else {
          delay(1);
        }
        digitalWrite(row, LOW);
      }
    }
  }

  // This method will turn every pixel in the framebuffer dark.
  void clearFrameBuffer(void) {
    for (uint8_t i = 0; i < 30; i++) {
      this->frameBuffer[i] = LedState::Dark;
    }
  }

  // This method will invert every pixel in the framebuffer, making a negative image.
  void negateFrameBuffer(void) {
    for (uint8_t i = 0; i < 30; i++) {
      this->frameBuffer[i] = !this->frameBuffer[i];
    }
  }

  // This method will set a dark LED to bright and a bright one to dark.
  void negateLedState(const uint8_t col, const uint8_t row) {
    this->frameBuffer[col + row * 5] = !this->frameBuffer[col + row * 5];
  }
};


DisplayController disp;
uint8_t col, row;
void setup() {
  col = 0;
  row = 0;
}

void loop() {
  disp.setLedState(col, row, LedState::Bright);
  disp.refresh(100);
  disp.setLedState(col, row, LedState::Dark);
  if (col == 4) {
    row = !row;
    col = 0;
  } else {
    col++;
  }
}
