#include "Emitter.h"
#include "EmitterManager.h"
#include "Particle.h"
#include "../Timer.h"
#include "../RegisterNum.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"
#include "../Math/Math.h"

// �R���X�g���N�^
Emitter::Emitter()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	// �萔�o�b�t�@�쐬
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(EmitterConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, emitterConstantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// emitterData ������ (�O������ǂݍ��߂�悤�ɂ�����)
	emitterData.duration = 5;
	emitterData.looping = true;
	emitterData.burstsTime = 0.01f;
	emitterData.burstsCount = 16;

	
}

// �X�V
void Emitter::Update()
{
	if (emitLifeTimer > emitterData.duration)
	{
		if (emitterData.looping)
		{
			// ���[�v����
			emitLifeTimer = 0;
		}
		else
		{
			// �G�~�b�^�[�j������
			EmitterManager::Instance().Remove(this);
			
			// ��Ureturn
			return;
		}
	}
	
	while (emitRateTimer > emitterData.burstsTime)
	{
		// �G�~�b�^�[�萔�o�b�t�@�X�V
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// ���̃G�~�b�^�[�̒l������Ă�����
		emitterConstant.emitterPosition = position;

		// �����p�[�e�B�N���ݒ�
		emitterConstant.particleColor.x = Math::RandomRange(emitterData.particleColorMin.x, emitterData.particleColorMax.x);
		emitterConstant.particleColor.y = Math::RandomRange(emitterData.particleColorMin.y, emitterData.particleColorMax.y);
		emitterConstant.particleColor.z = Math::RandomRange(emitterData.particleColorMin.z, emitterData.particleColorMax.z);
		emitterConstant.particleColor.w = Math::RandomRange(emitterData.particleColorMin.w, emitterData.particleColorMax.w);

		emitterConstant.particleLifeTime = Math::RandomRange(emitterData.particleLifeTimeMin, emitterData.particleLifeTimeMax);
		emitterConstant.particleSize.x = Math::RandomRange(emitterData.particleSizeMin.x, emitterData.particleSizeMax.x);
		emitterConstant.particleSize.y = Math::RandomRange(emitterData.particleSizeMin.y, emitterData.particleSizeMax.y);
		emitterConstant.particleFriction = Math::RandomRange(emitterData.particleFrictionMin, emitterData.particleFrictionMax);
		emitterConstant.particleAddAngle = emitterData.particleAddAngle;
		emitterConstant.particleGravity = emitterData.particleGravity;

		emitterConstant.particleKind = emitterData.particleStartKind;
		emitterConstant.particleBillboardType = emitterData.particleBillboardType;
		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		
		// �p�[�e�B�N���̐���
		Particle::Instance().Instance().Emit(emitterData.burstsCount);

		emitRateTimer -= emitterData.burstsTime;
	}

	// �p�[�e�B�N���̐����Ԋu�Ǘ��^�C�}�[���Z
	emitRateTimer += Timer::Instance().DeltaTime();
	// �G�~�b�^�[�̌o�ߎ��ԉ��Z
	emitLifeTimer += Timer::Instance().DeltaTime();
}
