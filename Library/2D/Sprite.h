#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Resource/Sprite/SpriteResource.h"

class Sprite
{
public:
	Sprite(const char* filename, const char* pixelShaderPath = nullptr, bool posZ1 = false);
	~Sprite();

private:	// �J�v�Z����
	DirectX::XMFLOAT2	position = {};
	DirectX::XMFLOAT2	size = { 100, 100 };
	DirectX::XMFLOAT4	color = { 1,1,1,1 };
	float				angle = 0;
	DirectX::XMFLOAT2	texPos = {};
	DirectX::XMFLOAT2	texSize = { 100,100 };		// �g���ĂȂ��ˁH
	DirectX::XMFLOAT2	scale = { 1.0f, 1.0f };
	DirectX::XMFLOAT2	pivotPoint = { 0,0 };

	// �`�悷�邩�ǂ���
	bool isRender = true;

	// Animation
	float animationTime = 0.0f;		// �A�j���[�V�����Đ�����
	int currentAnimationIndex = 0;	// ���݂̃A�j���[�V�����ԍ�
	bool animationLoop = true;		// ���[�v�Đ��t���O

public:
	// �Z�b�^�[
	void SetPos(const DirectX::XMFLOAT2 p) { position = p; }
	void SetPosX(const float x) { position.x = x; }
	void SetPosY(const float y) { position.y = y; }
	void SetSize(DirectX::XMFLOAT2 s) { size = s; }
	void SetSizeX(const float x) { size.x = x; }
	void SetSizeY(const float y) { size.y = y; }
	void SetColor(const DirectX::XMFLOAT4 c) { color = c; }
	void SetColorR(const float r) { color.x = r; }
	void SetColorG(const float g) { color.y = g; }
	void SetColorB(const float b) { color.z = b; }
	void SetColorA(const float a) { color.w = a; }
	void SetAngle(const float a) { angle = a; }
	void SetTexPos(const DirectX::XMFLOAT2 tP) { texPos = tP; }
	void SetTexPosX(const float x) { texPos.x = x; }
	void SetTexPosY(const float y) { texPos.y = y; }
	void SetTexSize(const DirectX::XMFLOAT2 tS) { texSize = tS; }
	void SetTexSizeX(const float x) { texSize.x = x; }
	void SetTexSizeY(const float y) { texSize.y = y; }
	void SetScale(DirectX::XMFLOAT2 s) { scale = s; }
	void SetScaleX(float x) { scale.x = x; }
	void SetScaleY(float y) { scale.y = y; }
	void SetPivotPoint(DirectX::XMFLOAT2 p) { pivotPoint = p; }
	void SetPivotPointX(float x) { pivotPoint.x = x; }
	void SetPivotPointY(float y) { pivotPoint.y = y; }
	void SetIsRender(bool b) { isRender = b; }

	// �Q�b�^�[
	DirectX::XMFLOAT2	GetPos() { return position; }
	float				GetPosX() { return position.x; }
	float				GetPosY() { return position.y; }
	DirectX::XMFLOAT2	GetSize() { return size; }
	float				GetSizeX() { return size.x; }
	float				GetSizeY() { return size.y; }
	DirectX::XMFLOAT4	GetColor() { return color; }
	float				GetColorR() { return color.x; }
	float				GetColorG() { return color.y; }
	float				GetColorB() { return color.z; }
	float				GetColorA() { return color.w; }
	float				GetAngle() { return angle; }
	DirectX::XMFLOAT2	GetTexPos() { return texPos; }
	float				GetTexPosX() { return texPos.x; }
	float				GetTexPosY() { return texPos.y; }
	DirectX::XMFLOAT2	GetTexSize() { return texSize; }
	float				GetTexSizeX() { return texSize.x; }
	float				GetTexSizeY() { return texSize.y; }
	DirectX::XMFLOAT2	GetScale() { return scale; }
	float				GetScaleX() { return scale.x; }
	float				GetScaleY() { return scale.y; }
	DirectX::XMFLOAT2	GetPivotPoint() { return pivotPoint; }
	float				GetPivotPointX() { return pivotPoint.x; }
	float				GetPivotPointY() { return pivotPoint.y; }
	bool				GetIsRender() { return isRender; }

	// animation
	void SetAnimationTime(float t) { animationTime = t; }
	float GetAnimationTime() { return animationTime; }
	
	void SetCurrentAnimationIndex(int i) { currentAnimationIndex = i; }
	int GetCurrentAnimationIndex() { return currentAnimationIndex; }

	// srv
	SpriteResource* GetSpriteResource() { return spriteResource.get(); }
public:
	// �A�j���[�V�����X�V
	void UpdateAnimation();

	// �`��
	void Render();

	// �����`��
	void SprTextOut(std::string s, DirectX::XMFLOAT2 pos);

private:
	std::shared_ptr<SpriteResource> spriteResource;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	// ��Ԏ�O�ł͂Ȃ���ԉ��ɕ`�悵�����Ƃ��� true �ɂ���
	bool posZ1 = false;
};