#include "ResourceManager.h"

using namespace DirectX;

struct Vertex {
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
    XMFLOAT2 TexCoord;
};

ComPtr<ID3D12Resource> ResourceManager::CreateDefaultBuffer(const void* initData,
                                                           UINT64 byteSize,
                                                           ComPtr<ID3D12Resource>& uploadBuffer) {
    ComPtr<ID3D12Resource> defaultBuffer;

    // Create the actual default buffer resource
    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    m_device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.GetAddressOf()));

    // Create upload buffer
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);

    m_device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

    // Copy data to upload buffer
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    // Schedule copy from upload buffer to default buffer
    CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST);
    m_commandList->ResourceBarrier(1, &barrier1);

    UpdateSubresources<1>(m_commandList, defaultBuffer.Get(), uploadBuffer.Get(),
                         0, 0, 1, &subResourceData);

    CD3DX12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ);
    m_commandList->ResourceBarrier(1, &barrier2);

    return defaultBuffer;
}

SharedPtr<MeshGeometry> ResourceManager::CreateBoxMesh(const String& name,
                                                            float width,
                                                            float height,
                                                            float depth) {
    float w2 = width * 0.5f;
    float h2 = height * 0.5f;
    float d2 = depth * 0.5f;

    std::array<Vertex, 24> vertices = {{
        // Front face
        Vertex({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }),

        // Back face
        Vertex({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }),
        Vertex({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }),

        // Top face
        Vertex({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }),

        // Bottom face
        Vertex({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }),
        Vertex({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }),

        // Left face
        Vertex({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }),
        Vertex({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }),

        // Right face
        Vertex({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }),
        Vertex({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) })
    }};

    std::array<std::uint16_t, 36> indices = {
        // Front face
        0, 1, 2,
        0, 2, 3,

        // Back face
        4, 5, 6,
        4, 6, 7,

        // Top face
        8, 9, 10,
        8, 10, 11,

        // Bottom face
        12, 13, 14,
        12, 14, 15,

        // Left face
        16, 17, 18,
        16, 18, 19,

        // Right face
        20, 21, 22,
        20, 22, 23
    };

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto mesh = SharedPtr<MeshGeometry>(new MeshGeometry());
    mesh->Name = name;

    // Create CPU memory buffers
    D3DCreateBlob(vbByteSize, &mesh->VertexBufferCPU);
    CopyMemory(mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    D3DCreateBlob(ibByteSize, &mesh->IndexBufferCPU);
    CopyMemory(mesh->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    // Create GPU buffers
    mesh->VertexBufferGPU = CreateDefaultBuffer(vertices.data(), vbByteSize, mesh->VertexBufferUploader);
    mesh->IndexBufferGPU = CreateDefaultBuffer(indices.data(), ibByteSize, mesh->IndexBufferUploader);

    mesh->VertexByteStride = sizeof(Vertex);
    mesh->VertexBufferByteSize = vbByteSize;
    mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
    mesh->IndexBufferByteSize = ibByteSize;

    // Create submesh
    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mesh->DrawArgs["box"] = submesh;

    m_meshes[name] = mesh;
    return mesh;
}

SharedPtr<MeshGeometry> ResourceManager::CreatePlaneMesh(const String& name,
                                                              float width,
                                                              float depth,
                                                              uint32 m,
                                                              uint32 n) {
    uint32 vertexCount = m * n;
    uint32 faceCount = (m - 1) * (n - 1) * 2;

    float halfWidth = 0.5f * width;
    float halfDepth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    float du = 1.0f / (n - 1);
    float dv = 1.0f / (m - 1);

    Vector<Vertex> vertices(vertexCount);
    for (uint32 i = 0; i < m; ++i) {
        float z = halfDepth - i * dz;
        for (uint32 j = 0; j < n; ++j) {
            float x = -halfWidth + j * dx;

            vertices[i * n + j].Pos = XMFLOAT3(x, 0.0f, z);
            vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            vertices[i * n + j].TexCoord = XMFLOAT2(j * du, i * dv);
        }
    }

    Vector<std::uint16_t> indices(faceCount * 3);

    uint32 k = 0;
    for (uint32 i = 0; i < m - 1; ++i) {
        for (uint32 j = 0; j < n - 1; ++j) {
            indices[k] = i * n + j;
            indices[k + 1] = i * n + j + 1;
            indices[k + 2] = (i + 1) * n + j;

            indices[k + 3] = (i + 1) * n + j;
            indices[k + 4] = i * n + j + 1;
            indices[k + 5] = (i + 1) * n + j + 1;

            k += 6;
        }
    }

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto mesh = SharedPtr<MeshGeometry>(new MeshGeometry());
    mesh->Name = name;

    D3DCreateBlob(vbByteSize, &mesh->VertexBufferCPU);
    CopyMemory(mesh->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    D3DCreateBlob(ibByteSize, &mesh->IndexBufferCPU);
    CopyMemory(mesh->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mesh->VertexBufferGPU = CreateDefaultBuffer(vertices.data(), vbByteSize, mesh->VertexBufferUploader);
    mesh->IndexBufferGPU = CreateDefaultBuffer(indices.data(), ibByteSize, mesh->IndexBufferUploader);

    mesh->VertexByteStride = sizeof(Vertex);
    mesh->VertexBufferByteSize = vbByteSize;
    mesh->IndexFormat = DXGI_FORMAT_R16_UINT;
    mesh->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mesh->DrawArgs["plane"] = submesh;

    m_meshes[name] = mesh;
    return mesh;
}

ComPtr<ID3DBlob> ResourceManager::CompileShader(const String& name,
                                               const WString& filename,
                                               const String& entrypoint,
                                               const String& target) {
    UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> byteCode = nullptr;
    ComPtr<ID3DBlob> errors;

    HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                    entrypoint.c_str(), target.c_str(), compileFlags, 0,
                                    &byteCode, &errors);

    if (errors != nullptr) {
        OutputDebugStringA((char*)errors->GetBufferPointer());
    }

    if (SUCCEEDED(hr)) {
        m_shaders[name] = byteCode;
    }

    return byteCode;
}

ComPtr<ID3D12PipelineState> ResourceManager::CreatePSO(const String& name,
                                                      const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
    ComPtr<ID3D12PipelineState> pso;
    HRESULT hr = m_device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));

    if (SUCCEEDED(hr)) {
        m_psos[name] = pso;
    }

    return pso;
}

SharedPtr<IMaterialComponent> ResourceManager::CreateMaterial(const String& psoName,
                                                                   const MaterialConstants& constants) {
    auto material = SharedPtr<BasicMaterialComponent>(new BasicMaterialComponent());

    auto pso = GetPSO(psoName);
    if (pso) {
        material->SetPSO(pso);
    }

    material->SetMaterialConstants(constants);
    material->InitializeMaterialBuffer(m_device);

    return material;
}

SharedPtr<IMeshComponent> ResourceManager::CreateMeshComponent(const String& meshName) {
    auto mesh = GetMesh(meshName);
    if (!mesh) return nullptr;

    return SharedPtr<BasicMeshComponent>(new BasicMeshComponent(mesh));
}