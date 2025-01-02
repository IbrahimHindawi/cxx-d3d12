#define UNICODE
#define _UNICODE

#include <windows.h>
#include <windowsx.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>
#include "d3dx12.h"
// #include "DDSTextureLoader.h"
// #include "MathHelper.h"
#include "core.h"


#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

struct DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) : ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(lineNumber)
{
}

// ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename)
// {
//     std::ifstream fin(filename, std::ios::binary);
// 
//     fin.seekg(0, std::ios_base::end);
//     std::ifstream::pos_type size = (int)fin.tellg();
//     fin.seekg(0, std::ios_base::beg);
// 
//     ComPtr<ID3DBlob> blob;
//     ThrowIfFailed(D3DCreateBlob(size, blob.GetAddressOf()));
// 
//     fin.read((char*)blob->GetBufferPointer(), size);
//     fin.close();
// 
//     return blob;
// }

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

// using vector = DirectX::XMVECTOR;
static const DirectX::XMVECTORF32 g_vHalfVector = { 0.5f, 0.5f, 0.5f, 0.5f };
static const DirectX::XMVECTORU32 vGrabY = { 0x00000000,0xFFFFFFFF,0x00000000,0x00000000 };
static const DirectX::XMFLOAT4X4 mm = {};

std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::FXMVECTOR v) {
    DirectX::XMFLOAT3 dest;
    DirectX::XMStoreFloat3(&dest, v);
    os << "{" << dest.x << ", " << dest.y << ", " << dest.z << "}";
    return os;
}

std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::FXMMATRIX m) {
    for (i32 i = 0; i < 4; i++) {
        os << DirectX::XMVectorGetX(m.r[i]) << "\t";
        os << DirectX::XMVectorGetY(m.r[i]) << "\t";
        os << DirectX::XMVectorGetZ(m.r[i]) << "\t";
        os << DirectX::XMVectorGetW(m.r[i]) << "\n";
    }
    return os;
}

struct Component {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
};

/*
 * FFFGHHC : 312C
 */
DirectX::XMVECTOR XM_CALLCONV proc(DirectX::FXMVECTOR f0, DirectX::FXMVECTOR f1, DirectX::FXMVECTOR f2, DirectX::GXMVECTOR g0, DirectX::HXMVECTOR h0, DirectX::HXMVECTOR h1, DirectX::CXMVECTOR c0) {
    // std::cout << f0 << std::endl;
    // std::cout << f1 << std::endl;
    // std::cout << f2 << std::endl;
    // std::cout << g0 << std::endl;
    // std::cout << h0 << std::endl;
    // std::cout << h1 << std::endl;
    // std::cout << c0 << std::endl;
    DirectX::XMVECTOR r = DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f);
    r = DirectX::XMVectorAdd(r, f0);
    r = DirectX::XMVectorAdd(r, f1);
    r = DirectX::XMVectorAdd(r, f2);
    r = DirectX::XMVectorAdd(r, g0);
    r = DirectX::XMVectorAdd(r, h0);
    r = DirectX::XMVectorAdd(r, h1);
    r = DirectX::XMVectorAdd(r, c0);
    return r;
}

/*
 * The exception to these rules is with constructor methods. 
 * [DirectXMath] recommends using FXMVECTOR for the first three 
 * XMVECTOR parameters and CXMVECTOR for the rest when writing a constructor 
 * that takes XMVECTOR parameters.
 */

void vproc() {
    if (!DirectX::XMVerifyCPUSupport()) {
        std::cout << "dxmath not supported" << std::endl;
    }

    DirectX::XMVECTOR a = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR b = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR c = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR d = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR e = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR f = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR g = DirectX::XMVectorSet(1.f, 2.f, 3.f, 4.f);
    DirectX::XMVECTOR r = proc(a, b, c, d, e, f, g);
    std::cout << r << std::endl;
    std::cout << g_vHalfVector.v << std::endl;

    DirectX::XMMATRIX m = DirectX::XMMatrixSet(
        1.f, 0.f, 0.f, 0.f, 
        0.f, 1.f, 0.f, 0.f, 
        0.f, 0.f, 1.f, 0.f, 
        0.f, 0.f, 0.f, 1.f
    );
    // DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();
    std::cout << m << std::endl;
    m = DirectX::XMLoadFloat4x4(&mm);
    std::cout << m << std::endl;

    // f32 *b = (f32 *)&o;
    // printf("%f\n", *b);
    // b++;
    // printf("%f\n", *b);
    // b++;
    // printf("%f\n", *b);
    // b++;
    // printf("%f\n", *b);


    // DirectX::XMVECTOR a = u + v;
    // DirectX::XMVECTOR b = u - v;
    // DirectX::XMVECTOR c = u * v;
    // DirectX::XMVECTOR d = 10.f * v;

    // DirectX::XMVECTOR l = DirectX::XMVector2Length(u);
    // DirectX::XMVECTOR nor = DirectX::XMVector2Normalize(u);
    // DirectX::XMVECTOR s = DirectX::XMVector2Dot(u, v);
    // DirectX::XMVECTOR e = DirectX::XMVector2Cross(u, v);

    // std::cout << l << std::endl;

    // DirectX::XMVECTOR projW;
    // DirectX::XMVECTOR perpW;
    // DirectX::XMVector3ComponentsFromNormal(&projW, &perpW, w, n);
}

//---------------------------------------------------------------------------------------------------
// globals
//---------------------------------------------------------------------------------------------------
Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
UINT64 mCurrentFence = 0;

Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

static const int SwapChainBufferCount = 2;
int mCurrBackBuffer = 0;
Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

D3D12_VIEWPORT mScreenViewport; 
D3D12_RECT mScissorRect;

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
u32 mRtvDescriptorSize = 0;
u32 mDsvDescriptorSize = 0;
u32 mCbvSrvUavDescriptorSize = 0;

HINSTANCE mhAppInst = nullptr; // application instance handle
HWND mhMainWnd = nullptr; // main window handle
bool mAppPaused = false;  // is the application paused?
bool mMinimized = false;  // is the application minimized?
bool mMaximized = false;  // is the application maximized?
bool mResizing = false;   // are the resize bars being dragged?
bool mFullscreenState = false;// fullscreen enabled


DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
int mClientWidth = 800;
int mClientHeight = 600;
bool m4xMsaaState = false;
bool m4xMsaaQuality = 0;

struct Timer {
    f64 mSecondsPerCount;
    f64 mDeltaTime;
    u64 mBaseTime;
    u64 mPausedTime;
    u64 mStopTime;
    u64 mPrevTime;
    u64 mCurrTime;
    bool mStopped;
};
Timer mTimer;
Timer timerCreate() {
    Timer timer = { .mDeltaTime = -1.0, };
    u64 countspersecond = 0;
    QueryPerformanceCounter((LARGE_INTEGER *)&countspersecond);
    timer.mSecondsPerCount = 1.0 / (f64)countspersecond;
    return timer;
}

void timerReset(Timer *timer) {
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currTime);
	timer->mBaseTime = currTime;
	timer->mPrevTime = currTime;
	timer->mStopTime = 0;
	timer->mStopped  = false;
}

void timerStart(Timer *timer) {
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if( timer->mStopped )
	{
		timer->mPausedTime += (startTime - timer->mStopTime);	

		timer->mPrevTime = startTime;
		timer->mStopTime = 0;
		timer->mStopped  = false;
	}
}

void timerStop(Timer *timer) {
	if( !timer->mStopped )
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		timer->mStopTime = currTime;
		timer->mStopped  = true;
	}
}

void timerTick(Timer *timer) {
	if( timer->mStopped )
	{
		timer->mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	timer->mCurrTime = currTime;

	// Time difference between this frame and the previous.
	timer->mDeltaTime = (timer->mCurrTime - timer->mPrevTime)*timer->mSecondsPerCount;

	// Prepare for next frame.
	timer->mPrevTime = timer->mCurrTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if(timer->mDeltaTime < 0.0)
	{
		timer->mDeltaTime = 0.0;
	}
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
f32 timerTotalTime(Timer *timer) {
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if( timer->mStopped )
	{
		return (f32)(((timer->mStopTime - timer->mPausedTime)-timer->mBaseTime)*timer->mSecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime
	
	else
	{
		return (f32)(((timer->mCurrTime-timer->mPausedTime)-timer->mBaseTime)*timer->mSecondsPerCount);
	}
}

// void timerDeltaTime(Timer *timer) { }

std::wstring mMainWndCaption = L"d3d App";

void OnMouseDown(WPARAM btnState, int x, int y){}
void OnMouseUp(WPARAM btnState, int x, int y){}
void OnMouseMove(WPARAM btnState, int x, int y){}

void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
    UINT count = 0;
    UINT flags = 0;

    // Call with nullptr to get list count.
    output->GetDisplayModeList(format, flags, &count, nullptr);

    std::vector<DXGI_MODE_DESC> modeList(count);
    output->GetDisplayModeList(format, flags, &count, &modeList[0]);

    for(auto& x : modeList)
    {
        UINT n = x.RefreshRate.Numerator;
        UINT d = x.RefreshRate.Denominator;
        std::wstring text =
            L"Width = " + std::to_wstring(x.Width) + L" " +
            L"Height = " + std::to_wstring(x.Height) + L" " +
            L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
            L"\n";

        // ::OutputDebugString(text.c_str());
        // std::cout << text.c_str() << std::endl;
        printf("%ls\n", text.c_str());
    }
}

void LogAdapterOutputs(IDXGIAdapter* adapter)
{
    UINT i = 0;
    IDXGIOutput* output = nullptr;
    while(adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        output->GetDesc(&desc);
        
        std::wstring text = L"***Output: ";
        text += desc.DeviceName;
        text += L"\n";
        // OutputDebugString(text.c_str());
        printf("%ls\n", text.c_str());

        LogOutputDisplayModes(output, mBackBufferFormat);

        ReleaseCom(output);

        ++i;
    }
}

void LogAdapters()
{
    UINT i = 0;
    IDXGIAdapter* adapter = nullptr;
    std::vector<IDXGIAdapter*> adapterList;
    while(mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wstring text = L"***Adapter: ";
        text += desc.Description;
        text += L"\n";

        // OutputDebugString(text.c_str());
        printf("%ls\n", text.c_str());

        adapterList.push_back(adapter);
        
        ++i;
    }

    for(size_t i = 0; i < adapterList.size(); ++i)
    {
        LogAdapterOutputs(adapterList[i]);
        ReleaseCom(adapterList[i]);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() {
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void FlushCommandQueue() {
	// Advance the fence value to mark commands up to this fence point.
    mCurrentFence++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
    ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
    if(mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

        // Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
	}
}

void CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	mCommandList->Close();
}

void CreateSwapChain()
{
    // Release the previous swapchain we will be recreating.
    mSwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width = mClientWidth;
    sd.BufferDesc.Height = mClientHeight;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = mBackBufferFormat;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.OutputWindow = mhMainWnd;
    sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
    ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd, 
		mSwapChain.GetAddressOf()));
}

void CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void OnResize() {
	assert(md3dDevice);
	assert(mSwapChain);
    assert(mDirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
    mDepthStencilBuffer.Reset();
	
	// Resize the swap chain.
    ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount, 
		mClientWidth, mClientHeight, 
		mBackBufferFormat, 
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;
 
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

    depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    // ThrowIfFailed(md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));
    CD3DX12_HEAP_PROPERTIES cd3dx12heapproperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    md3dDevice->CreateCommittedResource(&cd3dx12heapproperties, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf()));

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
    md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    CD3DX12_RESOURCE_BARRIER cd3dx12resourcebarrier = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	mCommandList->ResourceBarrier(1, &cd3dx12resourcebarrier);
	
    // Execute the resize commands.
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width    = static_cast<float>(mClientWidth);
	mScreenViewport.Height   = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

    mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}

void Set4xMsaaState(bool value)
{
    if(m4xMsaaState != value)
    {
        m4xMsaaState = value;

        // Recreate the swapchain and buffers with new multisample settings.
        CreateSwapChain();
        OnResize();
    }
}

LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			mAppPaused = true;
			timerStop(&mTimer);
		}
		else
		{
			mAppPaused = false;
			timerStart(&mTimer);
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth  = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if(md3dDevice)
		{
			if( wParam == SIZE_MINIMIZED )
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( mMinimized )
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if( mMaximized )
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if( mResizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing  = true;
        timerStop(&mTimer);
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing  = false;
        timerStart(&mTimer);
		OnResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
    case WM_KEYUP:
        if(wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        else if((int)wParam == VK_F2)
            Set4xMsaaState(!m4xMsaaState);

        return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return MsgProc(hwnd, msg, wParam, lParam);
}

void CalculateFrameStats() {
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
    
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (timerTotalTime(&mTimer) - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

        std::wstring fpsStr = std::to_wstring(fps);
        std::wstring mspfStr = std::to_wstring(mspf);

        std::wstring windowText = mMainWndCaption +
            L"    fps: " + fpsStr +
            L"   mspf: " + mspfStr;

        SetWindowText(mhMainWnd, windowText.c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


ID3D12Resource *CurrentBackBuffer() {
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE( mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
}

void Update(Timer timer) {
}

void Draw(Timer timer) {
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Indicate a state transition on the resource usage.
    CD3DX12_RESOURCE_BARRIER cd3dx12resourcebarrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &cd3dx12resourcebarrier );

    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
    // Specify the buffers we are going to render to.
    D3D12_CPU_DESCRIPTOR_HANDLE currentbackbufferhandle = CurrentBackBufferView();
    D3D12_CPU_DESCRIPTOR_HANDLE depthstencilviewhandle = DepthStencilView();
	mCommandList->OMSetRenderTargets(1, &currentbackbufferhandle, true, &depthstencilviewhandle);
	
    // Indicate a state transition on the resource usage.
    CD3DX12_RESOURCE_BARRIER cd3dx12resourcebarrier2 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &cd3dx12resourcebarrier2);

    // Done recording commands.
	ThrowIfFailed(mCommandList->Close());
 
    // Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	
	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void initDX() {
    //---------------------------------------------------------------------------------------------------
    // dx initalization
    //---------------------------------------------------------------------------------------------------
    std::cout << "dx init" << std::endl;
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
    Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	assert(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) != E_FAIL);
    debugController->EnableDebugLayer();
#endif

	assert(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)) != E_FAIL);

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fallback to WARP device.
	if(FAILED(hardwareResult))
	{
        Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
        assert(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) != E_FAIL);

		assert(D3D12CreateDevice( pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice)) != E_FAIL);
	}

	assert(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) != E_FAIL);

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	assert(md3dDevice->CheckFeatureSupport( D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)) != E_FAIL);

    m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert((m4xMsaaQuality > 0) && "Unexpected MSAA quality level.");
	
#ifdef _DEBUG
    LogAdapters();
#endif

	CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
    OnResize();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {
// int main() {
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    AttachConsole(ATTACH_PARENT_PROCESS);
    //---------------------------------------------------------------------------------------------------
    // win32 initalization
    //---------------------------------------------------------------------------------------------------
    std::cout << "win32 init" << std::endl;
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = mhAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"MainWnd";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0); 
	if( !mhMainWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

    initDX();

	MSG msg = {0};
	timerReset(&mTimer);
	while(msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if(PeekMessage( &msg, 0, 0, 0, PM_REMOVE ))
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
		// Otherwise, do animation/game stuff.
		else
        {	
			timerTick(&mTimer);

			if( !mAppPaused )
			{
				CalculateFrameStats();
				Update(mTimer);	
                Draw(mTimer);
			}
			else
			{
				Sleep(100);
			}
        }
    }
	// return (int)msg.wParam;

    return 0;
}
