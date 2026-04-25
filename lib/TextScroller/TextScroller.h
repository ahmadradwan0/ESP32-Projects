#pragma once
#include <Arduino.h>
#include <Screen.h>

class TextScroller {
  public:
    TextScroller(Screen* screen);
    
    void SetMessage(String message);
    void SetSize(int size);
    void SetSpeed(int pixelsPerStep);
    void SetY(int y);
    
    void Update();    // call this every loop iteration
  
  private:
    Screen* _screen;
    String _message;
    int _x;
    int _y;
    int _size;
    int _speed;
    int _messageWidth;
    
    void recalculateWidth();
};