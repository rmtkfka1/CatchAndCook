#include "pch.h"
#include "SpriteAction.h"
#include "Sprite.h"

bool ActionCommand::_UpdateEnable=true;

void ActionFunc::OnClickAction(KeyCode key, Sprite* sprite)
{

    if (ActionCommand::_UpdateEnable == false)
        return;

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

            for (auto& child : sprite->_children)
            {
                child->_spriteWorldParam.alpha -= 0.1f;
            }
        }
    } 
}

void ActionFunc::OnDragAction(KeyCode key, Sprite* sprite)
{
    if (ActionCommand::_UpdateEnable == false)
        return;

    static Sprite* _dragSprite = nullptr;
    static vec2 _lastMousePos; // 이전 마우스 위치 추적 변수

    // 우클릭 시작
    if (Input::main->GetMouseDown(key))
    {
        vec2 pos = Input::main->GetMouseDownPosition(key);

        float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
        float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

        _lastMousePos = pos;

        if (normalizedX >= (sprite->_ndcPos.x) &&
            normalizedX <= (sprite->_ndcPos.x + sprite->_ndcSize.x) &&
            normalizedY >= (sprite->_ndcPos.y) &&
            normalizedY <= (sprite->_ndcPos.y + sprite->_ndcSize.y))
        {
            _dragSprite = sprite;

        }
    }

    // 드래그 중
    if (_dragSprite && Input::main->GetMouse(key))
    {
        vec2 pos = Input::main->GetMousePosition();
        vec2 delta = pos - _lastMousePos; 

        delta.x = delta.x * (_dragSprite->_firstWindowSize.x / WINDOW_WIDTH);
        delta.y = delta.y * (_dragSprite->_firstWindowSize.y / WINDOW_HEIGHT);

        _dragSprite->SetPos(vec3(_dragSprite->_screenPos.x + delta.x,
            _dragSprite->_screenPos.y + delta.y,
            _dragSprite->_spriteWorldParam.ndcPos.z));

        for (auto& child : _dragSprite->_children)
        {
            child->SetPos(vec3(child->_screenPos.x + delta.x,
                child->_screenPos.y + delta.y,
                child->_spriteWorldParam.ndcPos.z));
        }

        _lastMousePos = pos; 
    }

    // 우클릭 종료
    if (_dragSprite && Input::main->GetMouseUp(key))
    {
        _dragSprite = nullptr;
    }
}

void ActionFunc::DisableMouseAction(KeyCode key, Sprite* sprite)
{
    if (ActionCommand::_UpdateEnable == false)
        return;

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
            ActionCommand::_UpdateEnable = false;
            sprite->_parent.lock()->_renderEnable = false;
            sprite->_renderEnable = false;
        }
    }
}

void ActionFunc::EnableDisableKeyAction(KeyCode key, Sprite* sprite)
{
    if (Input::main->GetKeyDown(key))
    {
        ActionCommand::_UpdateEnable = !ActionCommand::_UpdateEnable;

        sprite->_renderEnable = !sprite->_renderEnable;

        for (auto& child : sprite->_children)
        {
            child->_renderEnable = !child->_renderEnable;
        }
    }
}

