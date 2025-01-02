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

class KeyCode
{
public:
    static short Q;
    static short W;
    static short E;
    static short R;
    static short T;
    static short Y;
    static short U;
    static short I;
    static short O;
    static short P;
    /// <summary>///[{///</summary>
    static short LeftBracket;
    /// <summary>///]}///</summary>
    static short RightBracket;

    static short A;
    static short S;
    static short D;
    static short F;
    static short G;
    static short H;
    static short J;
    static short K;
    static short L;
    /// <summary>///;:///</summary>
    static short SemiColon;
    /// <summary>///'"///</summary>
    static short Quote;

    static short Z;
    static short X;
    static short C;
    static short V;
    static short B;
    static short N;
    static short M;
    /// <summary>///,<///</summary>
    static short Comma;
    /// <summary>///.>///</summary>
    static short Dot;
    /// <summary>////?///</summary>
    static short Slash;

    /// <summary>////`~///</summary>
    static short BackQoute;
    static short Num1;
    static short Num2;
    static short Num3;
    static short Num4;
    static short Num5;
    static short Num6;
    static short Num7;
    static short Num8;
    static short Num9;
    static short Num0;
    static short Minus;
    static short Eqauls;

    static short Tap;
    static short Capslock;
    static short Shift;
    static short CtrlL;

    static short Win;
    static short Alt;
    static short Space;
    /// <summary>////Alt, 한영///</summary>
    static short AltR;
    /// <summary>////문서 버튼///</summary>
    static short Menu;
    static short App;
    //static short ;

    static short CtrlR;
    static short ShiftR;
    static short Enter;
    static short BackSlash;
    static short Backspace;


    static short Esc;
    static short F1;
    static short F2;
    static short F3;
    static short F4;
    static short F5;
    static short F6;
    static short F7;
    static short F8;
    static short F9;
    static short F10;
    static short F11;
    static short F12;


    static short ScreenLock;
    static short Pause;

    static short Insert;
    static short Home;
    static short PageUp;

    static short Delete;
    static short End;
    static short PageDown;

    static short UpArrow;
    static short LeftArrow;
    static short DownArrow;
    static short RightArrow;

    static short NumPad7;
    static short NumPad8;
    static short NumPad9;

    static short NumPad4;
    static short NumPad5;
    static short NumPad6;

    static short NumPad1;
    static short NumPad2;
    static short NumPad3;


    static short NumPadNum;
    static short NumPadSlash;
    static short NumPadMultiple;
    static short NumPadMinus;
    static short NumPadPlus;
    static short NumPadEnter;
    static short NumPadDot;
    static short NumPad0;

    static short MOUSE_KECODE_OFFSET;
    static short KEYBOARD_KECODE_ENDPOINT;
    static short LeftMouse;
    static short RightMouse;
    static short CenterMouse;
    static short X1Mouse;
    static short X2Mouse;
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
