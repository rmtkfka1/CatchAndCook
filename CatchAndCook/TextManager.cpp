#pragma once
#include "pch.h"
#include "TextManager.h"

using namespace D2D1;

unique_ptr<TextManager> TextManager::main=nullptr;

void TextManager::Init()
{
    InitD2D();

}

TextHandle* TextManager::AllocTextStrcture(int width, int height, FontColor color, float size)
{
    TextHandle* textHandle = new TextHandle();

    textHandle->width = width;
    textHandle->height = height;
    textHandle->fontSize = size;

    uint32 dpi = ::GetDpiForWindow(Core::main->GetHandle());

    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpi,
            dpi
        );


    return textHandle;

}

void TextManager::InitD2D()
{
    // D3D12 장치 및 명령 큐 가져오기.
    auto d3d_device = Core::main->GetDevice().Get();
    auto d3d_cmdQueue = Core::main->GetCmdQueue().Get();

    uint32 d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};  // D2D1 팩토리 옵션 (디버그 레벨 설정)

    // D2D1 팩토리 및 D3D11 관련 인터페이스 포인터 선언
    ID2D1Factory3* factory = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* d3d11Context = nullptr;
    ID3D11On12Device* d3d11On12Device = nullptr;

    // D2D1 팩토리에서 디버그 레벨을 설정합니다.
    d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

    // D3D11On12 장치를 생성합니다. D3D12 장치를 사용하여 D3D11 장치를 만든 후, D3D11On12Device를 얻습니다.
    ThrowIfFailed(D3D11On12CreateDevice(
        d3d_device,                           // 기존 D3D12 장치
        d3d11DeviceFlags,                     // D3D11 장치 플래그
        nullptr,                              // 추가적인 전역 리소스 (여기서는 사용하지 않음)
        0,                                     // 전역 리소스 개수
        (IUnknown**)&d3d_cmdQueue,             // D3D12 명령 큐
        1,                                     // 명령 큐 개수
        0,                                     // 큐의 타이밍 (사용하지 않음)
        &device,                               // 반환될 D3D11 장치
        &d3d11Context,                         // 반환될 D3D11 장치 컨텍스트
        nullptr                                // 반환될 DXGI 장치 (여기서는 사용하지 않음)
    ));

    // D3D11On12 장치에서 D3D11 장치 인터페이스를 가져옵니다.
    ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&d3d11On12Device)));

    // D2D1 장치 생성 옵션 설정
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;

    // D2D1 팩토리를 생성합니다.
    ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, (void**)&factory));

    // D3D11On12 장치에서 IDXGIDevice를 얻어옵니다. (D2D가 D3D11On12 장치를 사용할 수 있도록 하기 위함)
    IDXGIDevice* pDXGIDevice = nullptr;
    ThrowIfFailed(d3d11On12Device->QueryInterface(IID_PPV_ARGS(&pDXGIDevice)));

    // D2D1 팩토리에서 D3D11On12 장치를 기반으로 D2D1 장치 생성
    ThrowIfFailed(factory->CreateDevice(pDXGIDevice, _device.GetAddressOf()));

    // D2D1 장치로부터 D2D1 디바이스 컨텍스트 생성
    ThrowIfFailed(_device->CreateDeviceContext(deviceOptions, _context.GetAddressOf()));

    ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&_factory));

    // D3D11On12 장치 및 관련 객체들의 메모리 해제
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (pDXGIDevice)
    {
        pDXGIDevice->Release();
        pDXGIDevice = nullptr;
    }

    if (factory)
    {
        factory->Release();
        factory = nullptr;
    }

    if (d3d11On12Device)
    {
        d3d11On12Device->Release();
        d3d11On12Device = nullptr;
    }

    if (d3d11Context)
    {
        d3d11Context->Release();
        d3d11Context = nullptr;
    }
}

