#pragma once

class Sprite;

class ActionFunc
{
public:
	static void OnClickAction(KeyCode key , Sprite* sprite );
	static void OnDragAction(KeyCode key, Sprite* sprite);
    static void OnClickDisableAction(KeyCode key, Sprite* sprite);
    static void OnKeySpriteEnableDisable(KeyCode key, Sprite* sprite);
};

class ActionCommand 
{
public:
    virtual ~ActionCommand() {}
    virtual void Execute(Sprite* sprite) = 0;

    static bool _UpdateEnable;

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

class DisableMouseAction : public ActionCommand
{
public:
    DisableMouseAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
         ActionFunc::OnClickDisableAction(_key, sprite);
    }

private:
    KeyCode _key;
};

class EnableKeyAction : public ActionCommand
{
public:
    EnableKeyAction(KeyCode key) : _key(key) {}

    void Execute(Sprite* sprite) override
    {
        ActionFunc::OnKeySpriteEnableDisable(_key, sprite);
    }
  
private:
    KeyCode _key;
};


