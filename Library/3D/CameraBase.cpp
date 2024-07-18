#include "CameraBase.h"

#include "../RegisterNum.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/RenderContext.h"


CameraBase::CameraBase()
{
	//Graphics* gfx = &Graphics::Instance();

	//std::lock_guard<std::mutex> lock(gfx->GetMutex()); // 排他制御
	//ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// --- プロジェクション座標変換行列作成 ---
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	//dc->RSGetViewports(&numViewports, &viewport);

	aspect = 1.7777777778f;

	Update();
}


// --- カメラ行列の更新 ---
void CameraBase::Update()
{
	Update(position, target, up, fov, nearZ, farZ, aspect);
}


void CameraBase::Update(const Vector3& position, const Vector3& target, const Vector3& up, const float fov, const float nearZ, const float farZ, const float aspect)
{	
	// --- 行列の計算 ---
	view.MakeLookAt(position, target, up);
	projection.MakePerspective(fov, aspect, nearZ, farZ);

	// --- ビュー行列を逆行列化して、ワールド行列を計算する ---
	Matrix world = Matrix::Inverse(view);

	// --- 行列から方向ベクトルを取り出す ---
	frontVec = Vector3::Normalize(world.v_[2].xyz());
	upVec = Vector3::Normalize(world.v_[1].xyz());
	rightVec = Vector3::Normalize(world.v_[0].xyz());

}


// --- 定数バッファの更新 ---
void CameraBase::UpdateConstants()
{
	UpdateConstants(position);
}


void CameraBase::UpdateConstants(const Vector3& position)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	RenderContext* rc = &RenderContext::Instance();

	// --- 情報の計算と設定 ---
	Matrix viewProjection = view * projection;
	Matrix InverseViewProjection = Matrix::Inverse(viewProjection);
	rc->cameraConstant.viewProjection = viewProjection.mat_;
	rc->cameraConstant.inverseViewProjection = InverseViewProjection.mat_;
	rc->cameraConstant.cameraPosition = { position.x, position.y, position.z, 1.0f/*0*/ };

	// --- 定数バッファの更新とバインド ---
	dc->UpdateSubresource(rc->cameraConstantBuffer.Get(), 0, 0, &rc->cameraConstant, 0, 0);
	dc->VSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());
	//dc->CSSetConstantBuffers(_cameraConstant, 1, rc->cameraConstantBuffer.GetAddressOf());

}
