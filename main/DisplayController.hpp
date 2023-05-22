namespace Display {

enum class LedState : bool { Dark = LOW, Bright = HIGH };
LedState operator!(LedState state) {
  return (state == LedState::Dark) ? LedState::Bright : LedState::Dark;
}

constexpr uint8_t RowCount = 6;
constexpr uint8_t ColumnCount = 5;

class Controller {
private:
  LedState frameBuffer[30];
public:
  Controller(void) {
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

  /**
   * This function will update the display to show what's in the framebuffer,
   * and do so on about a 16 millisecond loop until delayTimeMs have passed. For
   * the most accurate timing, `delayTimeMs` ought to be a multiple of 16.
   *
   * @param callbackData passed to `callback`
   * @param callback called 3 times per 8ms on a loop during each refresh
   */
  void refresh(
    const unsigned int delayTimeMs,
    void *callbackData,
    void (*callback)(void *)
  ) {
    auto startTime = millis();
    auto endTime = startTime + delayTimeMs;
    while (millis() < endTime) {
      for (uint8_t row = 0; row < 6; row++) {
        callback(callbackData);
        digitalWrite(row, HIGH);
        for (uint8_t col = 0; col < 5; col++) {
          digitalWrite(A0 + col, (bool)!this->getLedState(col, row));
        }
        /* For the red rows, we wait 3 milliseconds; for the green rows,
         * we wait 1 millisecond; for the blue rows, we wait 4 milliseconds. 
         * This helps balance out the brightnesses of the different colors.
         */
        switch (row) {
          case 0: case 3:
            delay(3);
          break;
          case 2: case 5:
            delay(4);
          break;
          default:
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

} // namespace Display
