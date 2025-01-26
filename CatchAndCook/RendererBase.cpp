#include "pch.h"
#include "RendererBase.h"



void RendererBase::RemoveSetters(shared_ptr<RenderObjectSetter> object)
{
	auto it = std::find(setters.begin(), setters.end(), object);

	if (it != setters.end())
	{
		setters.erase(it);
	}
}
