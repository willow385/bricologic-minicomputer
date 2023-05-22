#include "DisplayController.hpp"
#include "InputDial.hpp"

using Display::LedState;
using InputDial::TurnDirection;

struct Pixel {
  uint8_t x, y;
  bool vertical;
  bool prevDialPressed;
  Pixel(void) {
    this->x = 0;
    this->y = 0;
    this->vertical = false;
  }
};

struct PixelData {
  Pixel p;
  InputDial::PinReader dial;
} pixelData;
Display::Controller display;

void setup(void) {
  return;
}

void loop(void) {
  display.clearFrameBuffer();
  display.setLedState(pixelData.p.x, pixelData.p.y, LedState::Bright);
  display.refresh(32, static_cast<void *>(&pixelData), [](void *dataPtr) -> void {
    auto pixelData = static_cast<PixelData *>(dataPtr);
    switch (pixelData->dial.getTurnDirection()) {
      case TurnDirection::Stationary:
        if (pixelData->dial.isDialPressed()) {
          if (!pixelData->p.prevDialPressed) {
            pixelData->p.vertical = !pixelData->p.vertical;
          }
          pixelData->p.prevDialPressed = true;
        } else {
          pixelData->p.prevDialPressed = false;
        }
      break;
      case TurnDirection::Left:
        if (pixelData->p.vertical) {
          if (pixelData->p.y == 0) {
            if (pixelData->p.x == 0) {
              pixelData->p.x = 4;
            } else {
              pixelData->p.x--;
            }
            pixelData->p.y = 2;
          } else {
            pixelData->p.y--;
          }
        } else {
          if (pixelData->p.x == 0) {
            if (pixelData->p.y == 0) {
              pixelData->p.y = 2;
            } else {
              pixelData->p.y--;
            }
            pixelData->p.x = 4;
          } else {
            pixelData->p.x--;
          }
        }
      break;
      case TurnDirection::Right:
        if (pixelData->p.vertical) {
          if (pixelData->p.y == 2) {
            if (pixelData->p.x == 4) {
              pixelData->p.x = 0;
            } else {
              pixelData->p.x++;
            }
            pixelData->p.y = 0;
          } else {
            pixelData->p.y++;
          }
        } else {
          if (pixelData->p.x == 4) {
            if (pixelData->p.y == 2) {
              pixelData->p.y = 0;
            } else {
              pixelData->p.y++;
            }
            pixelData->p.x = 0;
          } else {
            pixelData->p.x++;
          }
        }
    }
  });
}
