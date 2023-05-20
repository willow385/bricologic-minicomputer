// change to 1 for debugging messages
#define SERIAL_DEBUG_MESSAGES_ENABLED 0

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

  /**
   * This function will update the display to show what's in the framebuffer,
   * and do so on a 16 millisecond loop until delayTimeMs have passed. For
   * the most accurate timing, delayTimeMs ought to be a multiple of 16.
   */
  void refresh(const unsigned int delayTimeMs) {
    for (unsigned int elapsedTime = 0; elapsedTime < delayTimeMs; elapsedTime += 16) {
      for (uint8_t row = 0; row < 6; row++) {
        digitalWrite(row, HIGH);
        for (uint8_t col = 0; col < 5; col++) {
          digitalWrite(A0 + col, (boolean)!this->getLedState(col, row));
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

struct Point {
  uint8_t col, row;
  uint8_t col_floor, col_ceil;
  Point(uint8_t col, uint8_t row, uint8_t col_floor, uint8_t col_ceil) {
    this->col = col;
    this->row = row;
    this->col_floor = col_floor;
    this->col_ceil = col_ceil;
  }

  void onFrameUpdate() {
    if (this->col == this->col_floor) {
      if (this->row == 0) {
        this->col++;
      } else {
        this->row--;
      }
    } else if (this->col == this->col_ceil) {
      if (this->row == 2) {
        this->col--;
      } else {
        this->row++;
      }
    } else {
      if (this->row == 0) {
        this->col++;
      } else {
        this->col--;
      }
    }
  }
};

DisplayController disp;
Point *points[2];
uint8_t cyclesLow; // counts the number of times input pin 11 read LOW
uint8_t cycleCounter;
bool invertDisplay;
void setup() {
  points[0] = new Point(0, 0, 0, 3);
  points[1] = new Point(4, 2, 1, 4);
  pinMode(11, INPUT_PULLUP);
  cycleCounter = 0;
  cyclesLow = 0;
  invertDisplay = false;
}

void loop() {
  if (digitalRead(11) == LOW) {
    cyclesLow++;
  } else if (cyclesLow > 0) {
    cyclesLow--;
  }
  for (auto point : points) {
    disp.setLedState(point->col, point->row, LedState::Bright);
    point->onFrameUpdate();
  }
  if (cycleCounter == 0 && cyclesLow != 0) {
    invertDisplay = !invertDisplay;
  }
  if (invertDisplay) {
    disp.negateFrameBuffer();
  }
  disp.refresh(80);
  disp.clearFrameBuffer();
  if (cyclesLow > 3) {
    cyclesLow = 3;
  }
  if (cycleCounter == 3) {
    #if SERIAL_DEBUG_MESSAGES_ENABLED
      Serial.print("cyclesLow == ");
      Serial.print(cyclesLow);
      Serial.println("");
    #endif
    cycleCounter = 0;
  } else {
    cycleCounter++;
  }
}
