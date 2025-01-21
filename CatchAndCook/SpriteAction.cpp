#include "pch.h"
#include "SpriteAction.h"
#include "Sprite.h"

void SpriteAction::OnClickAction()
{
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		auto pos = Input::main->GetMouseDownPosition(KeyCode::LeftMouse);
			float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
			float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

			for (auto& [rect, sprite] : Sprite::_collisionMap)
			{
				if (normalizedX >= rect.left && normalizedX <= rect.right &&
					normalizedY >= rect.top && normalizedY <= rect.bottom)
				{
					sprite->_spriteWorldParam.alpha *= 0.99f;
					break;
				}
			}
	}
}

void SpriteAction::OnDragAction()
{
	static Sprite* _dragSprtie = nullptr;
	static CollisionRect* _dragRect = nullptr;

	// 우클릭 시작
	if (Input::main->GetMouseDown(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMouseDownPosition(KeyCode::RightMouse);
		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		// 충돌된 스프라이트 검색
		for (auto& [rect, sprite] : Sprite::_collisionMap)
		{
			if (normalizedX >= rect.left && normalizedX <= rect.right &&
				normalizedY >= rect.top && normalizedY <= rect.bottom)
			{
				_dragSprtie = sprite;
				_dragRect = &rect;
				break; // 첫 번째로 충돌된 스프라이트만 선택
			}
		}
	}

	// 드래그 중
	if (_dragSprtie && Input::main->GetMouse(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMousePosition();
		auto size = _dragSprtie->_screenSize;
		_dragSprtie->SetPos(vec3(pos.x - size.x / 2, pos.y - size.y / 2, _dragSprtie->_spriteWorldParam.ndcPos.z));
	}

	// 우클릭 종료
	if (_dragSprtie && Input::main->GetMouseUp(KeyCode::RightMouse))
	{
		_dragRect->left = _dragSprtie->_ndcPos.x;
		_dragRect->top = _dragSprtie->_ndcPos.y;
		_dragRect->right = _dragSprtie->_ndcPos.x + _dragSprtie->_ndcSize.x;
		_dragRect->bottom = _dragSprtie->_ndcPos.y + _dragSprtie->_ndcSize.y;

		// 드래그 상태 종료
		_dragSprtie = nullptr;
		_dragRect = nullptr;
	};
}
