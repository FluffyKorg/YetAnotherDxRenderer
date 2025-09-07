#pragma once

#include <Types.h>

// Windows includes
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

// DirectX 12 includes
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DDSTextureLoader.h> 

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

// ComPtr
#include <wrl/client.h>
template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// RAII wrapper for Windows handles
class ScopedHandle {
public:
    ScopedHandle() noexcept : m_handle(nullptr) {}
    
    explicit ScopedHandle(HANDLE handle) noexcept : m_handle(handle) {}
    
    ~ScopedHandle() {
        if (m_handle && m_handle != INVALID_HANDLE_VALUE) {
            ::CloseHandle(m_handle);
        }
    }
    
    // Move constructor
    ScopedHandle(ScopedHandle&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }
    
    // Move assignment
    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            if (m_handle && m_handle != INVALID_HANDLE_VALUE) {
                ::CloseHandle(m_handle);
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }
    
    // Delete copy operations
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    
    HANDLE get() const noexcept { return m_handle; }
    
    bool isValid() const noexcept {
        return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
    }
    
private:
    HANDLE m_handle;
};

// Platform-specific types
using WindowHandle = HWND;
using InstanceHandle = HINSTANCE;

// Error handling
class WindowsException {
public:
    WindowsException(HRESULT hr, const String& function, const String& file, int line)
        : m_hr(hr), m_function(function), m_file(file), m_line(line) {}

    String GetMessage() const {
        LPWSTR messageBuffer = nullptr;
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, m_hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer, 0, nullptr
        );

        String message = "Windows Error in " + m_function + "\n";
        message += "File: " + m_file + "\n";
        message += "Line: " + std::to_string(m_line) + "\n";
        message += "HRESULT: 0x" + std::to_string(m_hr) + "\n";

        if (messageBuffer) {
            // Convert wide string to string (simplified)
            int size = WideCharToMultiByte(CP_UTF8, 0, messageBuffer, -1, nullptr, 0, nullptr, nullptr);
            String errorText(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, messageBuffer, -1, &errorText[0], size, nullptr, nullptr);
            message += "Description: " + errorText;
            LocalFree(messageBuffer);
        }

        return message;
    }

private:
    HRESULT m_hr;
    String m_function;
    String m_file;
    int m_line;
};

class d3dUtil {
public:

    static bool IsKeyDown(int vkeyCode);

    static std::string ToString(HRESULT hr);

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    static ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        ComPtr<ID3D12Resource>& uploadBuffer);

	static ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);
};

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index
// buffers so that we can implement the technique described by Figure 6.3.
struct SubmeshGeometry {
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;

    // Bounding box of the geometry defined by this submesh.
    // This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry {
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.
	ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
	ComPtr<ID3DBlob> IndexBufferCPU  = nullptr;

	ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

    // Data about the buffers.
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
	HashMap<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Texture {
    // Unique material name for lookup.
    String name;

    WString filename;

    ComPtr<ID3D12Resource> resource = nullptr;
    ComPtr<ID3D12Resource> uploadHeap = nullptr;
};

// Error checking macros
#define THROW_IF_FAILED(hr, function) \
    do { \
        HRESULT _hr = (hr); \
        if (FAILED(_hr)) { \
            throw WindowsException(_hr, function, __FILE__, __LINE__); \
        } \
    } while(0)

#define ThrowIfFailed(hr) THROW_IF_FAILED(hr, __FUNCTION__) // ???????

#define CHECK_WIN32_BOOL(result, function) \
    do { \
        if (!(result)) { \
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()), function); \
        } \
    } while(0)

// Utility functions
namespace Platform {
    String WStringToString(const WString& wstr);
    WString StringToWString(const String& str);

	void ShowMessageBox(const HWND hwnd, const String& title, const String& message);
    void ShowMessageBox(const String& title, const String& message);
    void OutputDebugMessage(const String& message);
}

