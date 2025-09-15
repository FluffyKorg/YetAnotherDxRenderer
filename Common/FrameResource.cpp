#include "FrameResource.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount) {
    // Create command allocator for this frame
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    // Create upload buffers for constant data
    if (passCount > 0) {
        PassCB = UniquePtr<UploadBuffer<PassConstants>>(
            new UploadBuffer<PassConstants>(device, passCount, true));
    }

    if (objectCount > 0) {
        ObjectCB = UniquePtr<UploadBuffer<ObjectConstants>>(
            new UploadBuffer<ObjectConstants>(device, objectCount, true));
    }

    if (materialCount > 0) {
        MaterialCB = UniquePtr<UploadBuffer<MaterialConstants>>(
            new UploadBuffer<MaterialConstants>(device, materialCount, true));
    }
}

FrameResource::~FrameResource() {
    // UniquePtr will automatically clean up the upload buffers
    // ComPtr will automatically release the command allocator
}