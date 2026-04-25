#include "TextScroller.h"

TextScroller::TextScroller(Screen* screen) {
  _screen = screen;
  _message = "";
  _x = 128;
  _y = 30;
  _size = 3;
  _speed = 2;
  _messageWidth = 0;
}

void TextScroller::SetMessage(String message) {
  _message = message;
  _x = 128;
  recalculateWidth();
}

void TextScroller::SetSize(int size) {
  _size = size;
  recalculateWidth();
}

void TextScroller::SetSpeed(int pixelsPerStep) {
  _speed = pixelsPerStep;
}

void TextScroller::SetY(int y) {
  _y = y;
}

void TextScroller::Update() {
  _screen->Clear();
  _screen->ShowText(_message, _x, _y, _size);
  _screen->Show();
  
  _x -= _speed;
  
  if (_x < -_messageWidth) {
    _x = 128;
  }
}

void TextScroller::recalculateWidth() {
  _messageWidth = _screen->GetTextPixelWidth(_message, _size);
}