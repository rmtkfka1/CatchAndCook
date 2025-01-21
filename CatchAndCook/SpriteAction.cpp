#include "pch.h"
#include "SpriteAction.h"
#include "Sprite.h"

void ActionFunc::OnClickAction(KeyCode key, Sprite* sprite)
{
	if (Input::main->GetMouseDown(key))
	{
		auto pos = Input::main->GetMouseDownPosition(key);

		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		if (normalizedX >= (sprite->_ndcPos.x) &&
			normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
			normalizedY >= (sprite->_ndcPos.y) &&
			normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
		{
			sprite->_spriteWorldParam.alpha -= 0.1f;
		}
	};
}

void ActionFunc::OnDragAction(KeyCode key, Sprite* sprite)
{
	static Sprite* _dragSprtie = nullptr;

	// 우클릭 시작
	if (Input::main->GetMouseDown(key))
	{
		vec2 pos = Input::main->GetMouseDownPosition(key);

		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		if (normalizedX >= (sprite->_ndcPos.x) &&
			normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
			normalizedY >= (sprite->_ndcPos.y) &&
			normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
		{
			_dragSprtie = sprite;
		}
	};

	// 드래그 중
	if (_dragSprtie && Input::main->GetMouse(key))
	{
		vec2 pos = Input::main->GetMousePosition();
		auto size = _dragSprtie->_screenSize;
		_dragSprtie->SetPos(vec3(pos.x - size.x / 2, pos.y - size.y / 2, _dragSprtie->_spriteWorldParam.ndcPos.z));
	}

	// 우클릭 종료
	if (_dragSprtie && Input::main->GetMouseUp(key))
	{
		_dragSprtie = nullptr;
	};
}
