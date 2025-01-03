#pragma once

class IDelayDestroy
{
private:
    bool _destroy = false;
public:
    IDelayDestroy();
    virtual ~IDelayDestroy();
    bool IsDestroy() const;
    virtual void SetDestroy();
};