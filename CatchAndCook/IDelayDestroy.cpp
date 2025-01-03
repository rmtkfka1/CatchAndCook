#include "pch.h"
#include "IDelayDestroy.h"

bool IDelayDestroy::IsDestroy() const
{
    return _destroy;
}

void IDelayDestroy::SetDestroy()
{
    _destroy = true;
}

IDelayDestroy::~IDelayDestroy() = default;


IDelayDestroy::IDelayDestroy() : _destroy(false)
{
}