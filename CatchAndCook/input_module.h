#pragma once

#include "pch.h"


struct WinEvent
{
    HWND hWnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    std::chrono::time_point<std::chrono::steady_clock> time;
};


enum KeyCode : short 
{
    Q = 81,
    W = 87,
    E = 69,
    R = 82,
    T = 84,
    Y = 89,
    U = 85,
    I = 73,
    O = 79,
    P = 80,

    LeftBracket = 219,
    RightBracket = 221,

    A = 65,
    S = 83,
    D = 68,
    F = 70,
    G = 71,
    H = 72,
    J = 74,
    K = 75,
    L = 76,

    SemiColon = 186,
    Quote = 222,

    Z = 90,
    X = 88,
    C = 67,
    V = 86,
    B = 66,
    N = 78,
    M = 77,

    Comma = 188,
    Dot = 190,
    Slash = 191,

    BackQoute = 192,
    Num1 = 49,
    Num2 = 50,
    Num3 = 51,
    Num4 = 52,
    Num5 = 53,
    Num6 = 54,
    Num7 = 55,
    Num8 = 56,
    Num9 = 57,
    Num0 = 48,

    Minus = 189,
    Eqauls = 187,

    Tap = 9,
    Capslock = 20,
    Shift = 16,
    CtrlL = 17,
    Win = 91,
    Alt = 18,
    Space = 32,

    AltR = 18,
    Menu = 93,
    App = 93,

    CtrlR = 25,
    ShiftR = 16,
    Enter = 13,
    BackSlash = 220,
    Backspace = 8,

    Esc = 27,
    F1 = 112,
    F2 = 113,
    F3 = 114,
    F4 = 115,
    F5 = 116,
    F6 = 117,
    F7 = 118,
    F8 = 119,
    F9 = 120,
    F10 = 121,
    F11 = 122,
    F12 = 123,

    ScreenLock = 145,
    Pause = 19,

    Insert = 45,
    Home = 36,
    PageUp = 33,

    Delete = 46,
    End = 35,
    PageDown = 34,

    UpArrow = 38,
    LeftArrow = 37,
    DownArrow = 40,
    RightArrow = 39,

    NumPad7 = 103,
    NumPad8 = 104,
    NumPad9 = 105,

    NumPad4 = 100,
    NumPad5 = 101,
    NumPad6 = 102,

    NumPad1 = 97,
    NumPad2 = 98,
    NumPad3 = 99,

    NumPadNum = 144,
    NumPadSlash = 111,
    NumPadMultiple = 106,
    NumPadMinus = 109,
    NumPadPlus = 107,
    NumPadEnter = 13,
    NumPadDot = 110,
    NumPad0 = 96,

    MOUSE_KEYCODE_OFFSET = 512,
    LeftMouse = MOUSE_KEYCODE_OFFSET + 0,
    RightMouse = MOUSE_KEYCODE_OFFSET + 1,
    CenterMouse = MOUSE_KEYCODE_OFFSET + 2,
    X1Mouse = MOUSE_KEYCODE_OFFSET + 3,
    X2Mouse = MOUSE_KEYCODE_OFFSET + 4,

    KEYBOARD_KEYCODE_ENDPOINT = 256
};


enum InputType
{
    Keyboard,
    Mouse,
    Text,
    Event
};

struct InputKeyboardDesc
{
    short repeat;
    short scanCode;
    short extendedKey;
    bool isAlt;

    bool isFirst;
    bool isDown;
    bool isUp;
};

struct InputTextDesc
{
    std::wstring str;
};

struct InputMouseDesc
{
    bool isDown;
    bool isUp;
    bool isShift;
    bool isCtrl;

    bool isInClient;
    float posX;
    float posY;
};

struct InputEventDesc
{
    bool isQuit;
};

class InputEvent
{
public:
    InputEvent() = default;
    InputType type;

    short keyCode;

    union
    {
        InputKeyboardDesc keyboard;
        InputMouseDesc mouse;
        InputEventDesc event;
    };

    std::chrono::time_point<std::chrono::steady_clock> time;
};
