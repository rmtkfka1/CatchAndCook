#pragma once

class Sprite;

class ActionFunc
{
public:
	static void OnClickAction(KeyCode key , Sprite* sprite );
	static void OnDragAction(KeyCode key, Sprite* sprite);
};


class ActionCommand 
{
public:
    virtual ~ActionCommand() {}
    virtual void Execute(Sprite* sprite) = 0;
};


class ClickAction : public ActionCommand
{
public:
    ClickAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::OnClickAction(_key, sprite);
    }

private:
    KeyCode _key;
};


class DragAction : public ActionCommand 
{
public:
    DragAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override 
    {
        ActionFunc::OnDragAction(_key, sprite);
    }

private:
    KeyCode _key;
};


