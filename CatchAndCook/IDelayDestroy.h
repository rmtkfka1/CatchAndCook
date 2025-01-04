#pragma once

class IDelayDestroy
{
private:
    bool _destroy = false;

    bool IsFirst() const { return _first; };
    void FirstOff() { _first = false; };
    void SetFirst(bool first) { _first = first; };
    bool _first = true;
public:
    IDelayDestroy();
    virtual ~IDelayDestroy();
    bool IsDestroy() const;
    virtual void SetDestroy();

	friend class GameObject;
    friend class Component;
};