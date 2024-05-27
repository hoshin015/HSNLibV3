#include <filesystem>
// --- Library ---
#include "../../Library/ErrorLogger.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/Shader.h"
#include "../../Library/Graphics/Texture.h"
#include "../../Library/RegisterNum.h"
// --- Game ---
#include "StaticModel.h"



StaticModel::StaticModel(const char* filename)
{
	modelResource = ResourceManager::Instance().LoadModelResource(filename);

	CreateComObject();
}

void StaticModel::CreateComObject()
{
	// --- Graphics 取得 ----
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();


	//---  vertexBuffer と indexBuffer の作成 ---
	for (ModelResource::Mesh& mesh : modelResource->GetMeshes())
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subresourceData{};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ModelResource::Vertex) * mesh.vertices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = mesh.vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresourceData.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

#if 0
		// 使用した 頂点情報とインデックス情報をクリアする
		mesh.vertices.clear();
		mesh.indices.clear();
#endif
	}

	//--- inputLayout と vertexShader の作成 ---
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	CreateVsFromCso("Data/Shader/StaticPhongVS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	//--- pixelShader の作成 ---
	CreatePsFromCso("Data/Shader/StaticPhongPS.cso", pixelShader.ReleaseAndGetAddressOf());

	//--- constantBuffer の作成 ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// material の名前に対応する テクスチャからshaderResourceViewの生成
	for (auto& [name, material] : modelResource->GetMaterials())		// 構造化束縛
	{
		for (int textureIndex = 0; textureIndex < 4; textureIndex++)
		{
			if (material.textureFilenames[textureIndex].size() > 0)
			{
				std::filesystem::path path(modelResource->GetFilePath());
				path.replace_filename(material.textureFilenames[textureIndex]);
				D3D11_TEXTURE2D_DESC texture2dDesc{};
				LoadTextureFromFile(path.c_str(), material.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
			}
			else
			{
				DWORD color = 0xFFFFFFFF;
				// normal
				if (textureIndex == 1) color = 0xFFFF7F7F;
				// emissive
				if (textureIndex == 3) color = 0x00000000;
				LoadFbx::Instance().MakeDummyTexture(material.shaderResourceViews[textureIndex].GetAddressOf(), color, 16);
			}
		}
	}
}


void StaticModel::Render(int instancing, DirectX::XMFLOAT4X4* instancingTransform, bool isShadow)
{
	// --- Graphics 取得 ---
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// 一旦ゼロクリア
	for (int index = 0; index < MAX_INSTANCE; index++)
	{
		data.transforms[index] = {};
	}
	

	// --- mesh ごとの描画 ---
	for (const ModelResource::Mesh& mesh : modelResource->GetMeshes())
	{
		UINT stride = sizeof(ModelResource::Vertex);
		UINT offset = 0;

		dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dc->IASetInputLayout(inputLayout.Get());

		// 影ならシェーダーをセットしない
		if (!isShadow)
		{
			dc->VSSetShader(vertexShader.Get(), nullptr, 0);
			dc->PSSetShader(pixelShader.Get(), nullptr, 0);
		}


		// インスタンスの各姿勢を保存
		data.instancingCount = instancing;
		for (int index = 0; index < instancing; index++)
		{
			DirectX::XMMATRIX M = XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&instancingTransform[index]);
			XMStoreFloat4x4(&data.transforms[index], M);
		}

		// --- subset ごとの描画 ---
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			// マテリアルの取得
			const ModelResource::Material& material = modelResource->GetMaterials().at(subset.materialName);
			data.materialColorKd = material.Kd;
			data.materialColorKs = material.Ks;
			data.materialColorKa = material.Ka;

			dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			dc->VSSetConstantBuffers(_objectConstant, 1, constantBuffer.GetAddressOf());

			dc->PSSetShaderResources(_deffuseTexture, 1, material.shaderResourceViews[_deffuseTexture].GetAddressOf());
			dc->PSSetShaderResources(_normalTexture, 1, material.shaderResourceViews[_normalTexture].GetAddressOf());
			dc->PSSetShaderResources(_specularTexture, 1, material.shaderResourceViews[_specularTexture].GetAddressOf());
			dc->PSSetShaderResources(_emissiveTexture, 1, material.shaderResourceViews[_emissiveTexture].GetAddressOf());

			// インスタンシング描画
			dc->DrawIndexedInstanced(subset.indexCount, instancing, subset.startIndex, 0, 0);
		}
	}
}
