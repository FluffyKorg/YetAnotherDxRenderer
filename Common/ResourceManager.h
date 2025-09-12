#pragma once

#include "RenderComponents.h"

class ResourceManager {
public:
    ResourceManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
        : m_device(device), m_commandList(cmdList) {}
    
    ~ResourceManager() = default;
    
    // Mesh management
    SharedPtr<MeshGeometry> GetMesh(const String& name) {
        auto it = m_meshes.find(name);
        return (it != m_meshes.end()) ? it->second : nullptr;
    }
    
    void AddMesh(const String& name, SharedPtr<MeshGeometry> mesh) {
        m_meshes[name] = mesh;
    }
    
    SharedPtr<MeshGeometry> CreateBoxMesh(const String& name,
                                                 float width = 2.0f,
                                                 float height = 2.0f,
                                                 float depth = 2.0f);
    
    SharedPtr<MeshGeometry> CreateSphereMesh(const String& name,
                                                   float radius = 1.0f,
                                                   uint32 sliceCount = 20,
                                                   uint32 stackCount = 20);
    
    SharedPtr<MeshGeometry> CreateCylinderMesh(const String& name,
                                                     float bottomRadius = 0.5f,
                                                     float topRadius = 0.5f,
                                                     float height = 3.0f,
                                                     uint32 sliceCount = 20,
                                                     uint32 stackCount = 20);
    
    SharedPtr<MeshGeometry> CreatePlaneMesh(const String& name,
                                                  float width = 10.0f,
                                                  float depth = 10.0f,
                                                  uint32 m = 2,
                                                  uint32 n = 2);
    
    // Texture management
    SharedPtr<Texture> GetTexture(const String& name) {
        auto it = m_textures.find(name);
        return (it != m_textures.end()) ? it->second : nullptr;
    }
    
    void AddTexture(const String& name, SharedPtr<Texture> texture) {
        m_textures[name] = texture;
    }
    
    SharedPtr<Texture> LoadTextureFromFile(const String& name,
                                                 const WString& filename);
    
    // Pipeline State Object management
    ComPtr<ID3D12PipelineState> GetPSO(const String& name) {
        auto it = m_psos.find(name);
        return (it != m_psos.end()) ? it->second : nullptr;
    }
    
    void AddPSO(const String& name, ComPtr<ID3D12PipelineState> pso) {
        m_psos[name] = pso;
    }
    
    ComPtr<ID3D12PipelineState> CreatePSO(const String& name,
                                          const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
    
    // Shader management
    ComPtr<ID3DBlob> GetShader(const String& name) {
        auto it = m_shaders.find(name);
        return (it != m_shaders.end()) ? it->second : nullptr;
    }
    
    void AddShader(const String& name, ComPtr<ID3DBlob> shader) {
        m_shaders[name] = shader;
    }
    
    ComPtr<ID3DBlob> CompileShader(const String& name,
                                   const WString& filename,
                                   const String& entrypoint,
                                   const String& target);
    
    // Material component factory
    SharedPtr<IMaterialComponent> CreateMaterial(const String& psoName,
                                                       const MaterialConstants& constants = MaterialConstants());
    
    // Mesh component factory
    SharedPtr<IMeshComponent> CreateMeshComponent(const String& meshName);
    
    // Cleanup upload buffers after GPU upload is complete
    void CleanupUploadBuffers() {
        for (auto& [name, mesh] : m_meshes) {
            if (mesh) {
                mesh->DisposeUploaders();
            }
        }
        
        for (auto& [name, texture] : m_textures) {
            if (texture && texture->uploadHeap) {
                texture->uploadHeap = nullptr;
            }
        }
    }
    
    // Get all resource names for debugging
    Vector<String> GetMeshNames() const {
        Vector<String> names;
        for (const auto& [name, mesh] : m_meshes) {
            names.push_back(name);
        }
        return names;
    }
    
    Vector<String> GetTextureNames() const {
        Vector<String> names;
        for (const auto& [name, texture] : m_textures) {
            names.push_back(name);
        }
        return names;
    }
    
    Vector<String> GetPSONames() const {
        Vector<String> names;
        for (const auto& [name, pso] : m_psos) {
            names.push_back(name);
        }
        return names;
    }
    
    ID3D12Device* GetDevice() const { return m_device; }
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList; }
    
private:
    ID3D12Device* m_device;
    ID3D12GraphicsCommandList* m_commandList;
    
    HashMap<String, SharedPtr<MeshGeometry>> m_meshes;
    HashMap<String, SharedPtr<Texture>> m_textures;
    HashMap<String, ComPtr<ID3D12PipelineState>> m_psos;
    HashMap<String, ComPtr<ID3DBlob>> m_shaders;
    
    // Helper function to create default buffer on GPU
    ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData,
                                               UINT64 byteSize,
                                               ComPtr<ID3D12Resource>& uploadBuffer);
};