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
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex()); // �r������

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
	while (emitRateTimer >= emitterData.burstsTime)
	{
		// �G�~�b�^�[�萔�o�b�t�@�X�V
		Graphics* gfx = &Graphics::Instance();
		ID3D11DeviceContext* dc = gfx->GetDeviceContext();

		// ���̃G�~�b�^�[�̒l������Ă�����
		emitterConstant.emitterPosition = position;
		emitterConstant.emitCount = emitCount;

		emitterConstant.particleSpeedMin = emitterData.particleSpeedMin;
		emitterConstant.particleSpeedMax = emitterData.particleSpeedMax;
		emitterConstant.particleColorMin = emitterData.particleColorMin;
		emitterConstant.particleColorMax = emitterData.particleColorMax;
		emitterConstant.particleLifeTimeMin = emitterData.particleLifeTimeMin;
		emitterConstant.particleLifeTimeMax = emitterData.particleLifeTimeMax;
		emitterConstant.particleSizeMin = emitterData.particleSizeMin;
		emitterConstant.particleSizeMax = emitterData.particleSizeMax;
		emitterConstant.particleAngleMin = emitterData.particleAngleMin;
		emitterConstant.particleAngleMax = emitterData.particleAngleMax;
		emitterConstant.particleAddAngle = emitterData.particleAddAngle;
		emitterConstant.particleGravity = emitterData.particleGravity;

		emitterConstant.particleKind = emitterData.particleKind;
		emitterConstant.particleBillboardType = emitterData.particleBillboardType;
		emitterConstant.particleTextureType = emitterData.particleTextureType;

		dc->UpdateSubresource(emitterConstantBuffer.Get(), 0, 0, &emitterConstant, 0, 0);
		dc->CSSetConstantBuffers(_emitterConstant, 1, emitterConstantBuffer.GetAddressOf());
		
		// �p�[�e�B�N���̐���
		Particle::Instance().Instance().Emit(emitterData.burstsCount);

		emitCount += Math::RandomRange(0.01, 1.0);
		emitRateTimer -= emitterData.burstsTime;


		if(emitterData.burstsOneShot > 0)
		{
			emitterData.burstsOneShot--;
			if (emitterData.burstsOneShot == 0)
			{
				// �G�~�b�^�[�j������
				EmitterManager::Instance().Remove(this);
				return;
			}
		}
	}

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

	// �p�[�e�B�N���̐����Ԋu�Ǘ��^�C�}�[���Z
	emitRateTimer += Timer::Instance().DeltaTime();
	// �G�~�b�^�[�̌o�ߎ��ԉ��Z
	emitLifeTimer += Timer::Instance().DeltaTime();
}
