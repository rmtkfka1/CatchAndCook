#include "pch.h"
#include "RendererBase.h"


void RendererBase::AddSetter(const std::shared_ptr<RenderObjectSetter>& setter)
{
	auto it = ranges::find(_setters, setter);
	if (it == _setters.end())
		_setters.push_back(setter);
}

void RendererBase::RemoveSetter(const shared_ptr<RenderObjectSetter>& object)
{
	auto it = ranges::find(_setters, object);
	if (it != _setters.end())
		_setters.erase(it);
}
