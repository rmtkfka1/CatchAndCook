#include "pch.h"
#include "CoreObject.h"
#include "guid_utility.h"


std::unordered_map<std::wstring, std::weak_ptr<EObject>> EObject::_EObjectTable{};

void EObject::StaticInit()
{
    _EObjectTable.reserve(100000);
}

void EObject::StaticRelease()
{
    _EObjectTable.clear();
}

EObject::EObject()
{
    this->SetGUID(Guid::GetNewGuid());
}

EObject::~EObject()
{
	RemoveGuid(this->guid);
}

EObject::EObject(const std::wstring& guid)
{
    this->SetGUID(guid);
}

EObject::EObject(const EObject& eObject)
{
    this->SetGUID(eObject.guid);
}

EObject::EObject(EObject&& eObject) noexcept
{
    this->SetGUID(std::move(eObject.guid));
}

EObject& EObject::operator=(const EObject& eObject)
{
    if (this == &eObject)
        return *this;

    this->SetGUID(eObject.guid);
    return *this;
}

EObject& EObject::operator=(EObject&& eObject) noexcept
{
    if (this == &eObject)
        return *this;

    this->SetGUID(std::move(eObject.guid));
    return *this;
}


void EObject::SetGUID(const std::wstring& str)
{
    auto& prevGuid = this->guid;
    bool isFindPrevGuid = this->guid.empty() ? false : ContainsByGuid(prevGuid);
    if (isFindPrevGuid)
    {
        RemoveGuid(prevGuid);
        AddObject(this->shared_from_this());
    }

    this->guid = str;
}

std::wstring& EObject::GetGUID()
{
    return this->guid;
}

bool EObject::operator==(const EObject& other) const
{
    return other.guid == this->guid;
}

bool EObject::operator<(const EObject& other) const
{
    return other.guid < this->guid;
}
