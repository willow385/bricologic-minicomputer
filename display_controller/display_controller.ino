enum class Row {
  // r0 represents the top row, and r5 represents the bottom row.
  r0, r1, r2, r3, r4, r5
};

enum class Column {
  // c0 represents the leftmost column, and c4 represents the rightmost.
  c0, c1, c2, c3, c4
};

class DisplayController {
  // Pins A0 through A4 correspond to columns; pins 0 through 5 correspond to rows.
private:
  boolean pinStates[30];
public:
  DisplayController() {
    for (unsigned int i = 0; i < 30; i++) {
      this->pinStates[i] = false;
    }
    
  }
};

void setup() {
  for (unsigned int pin = 0; pin < 4; pin++) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

// display a spinner animation on the four LEDs
void loop() {
  setLight(Row::Top,    Column::Right, LOW );
  setLight(Row::Top,    Column::Left,  HIGH);
  delay(100);
  setLight(Row::Top,    Column::Left,  LOW );
  setLight(Row::Bottom, Column::Left,  HIGH);
  delay(100);
  setLight(Row::Bottom, Column::Left,  LOW );
  setLight(Row::Bottom, Column::Right, HIGH);
  delay(100);
  setLight(Row::Bottom, Column::Right, LOW );
  setLight(Row::Top,    Column::Right, HIGH);
  delay(100);
}
