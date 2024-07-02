#ifndef RegisterNum_h
#define RegisterNum_h

////////////////////////////////////////////////////////////////////
//
//�@c++ ����ς����� hlsl �����蓮�ŕς���
//
////////////////////////////////////////////////////////////////////

#define USE_LinearWorkflow				0


// ====== �萔 ======
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		6
#define SHADOWMAP_COUNT					4
#define THREAD_NUM_X					256

// ====== �T���v���[ ======
#define _pointSampler					0
#define _linearSampler					1
#define _anisotropicSampler				2
#define _textLinarSampler				3
#define _textLinarSampler				4
#define _linearBorderBlackSampler		5
#define _linearBorderWhiteSampler		6
#define _shadowSampler					7
#define _samplerNum						8

// ===== GpuParticle ======

// particleType
#define pk_Dust					0
#define pk_PlayerAttackSpark	1
#define pk_PlayerAttackHit		2
#define pk_smoke				6
#define pk_simpleFire			7
#define pk_novaBurst			8
#define pk_novaStartFire		9

#define pk_fireBreath			10
#define pk_beamCylinder			11

// billboardType
#define bt_View		0
#define bt_Velocity 1


// --- c++ ---
#ifdef __cplusplus

// ====== �萔�o�b�t�@ ======
#define _cameraConstant		0		// �J�����̃f�[�^
#define _objectConstant		1		// 3d ��Ԃɏo���S�Ẵ��f�������� 
#define _lightConstant		2		// ���C�g�̃f�[�^
#define _timerConstant		3		// �^�C�}�[�̃f�[�^
#define _shadowConstant		6		// �e�̃f�[�^
#define _emitterConstant	7		// �G�~�b�^�[�̃f�[�^
#define _uvScrollConstant	8		// uv�X�N���[���̃f�[�^
#define _dissolveConstant	9		// �f�B�]���u�̃f�[�^

#define _luminanceExtractionConstant	5	// ���P�x���o�p
#define _gaussianConstant				5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			5	// �K�E�V�A���t�B���^���ϗp

#define _radialBlurConstant		5	// ���W�A���u���[�p
#define _heatHazeConstant		6	// heatHaze �p
#define _colorFilterConstant	7	// �J���[�t�B���^�[�p

// ====== �e�N�X�`���ԍ� ======

// phong
#define _deffuseTexture		0
#define _normalTexture		1
#define _specularTexture	2
#define _emissiveTexture	3

// pbr
#define _baseTexture				0
#define _normalTexture				1
#define _metallicRoughnessTexture	2
#define _emissiveTexture			3
#define _occlusionTexture			4

// dissolve
#define _dissolveTexture	5

// ibl (���O�v�Z�e�N�X�`��)
#define  _IEM	33		// Pre-filtered Irradiance Environment Map (IEM)
#define _PMREM	34		// Pre-filtered Mipmapped Radiance Environment Map (PMREM)
#define _LutGgx 35		// Look-Up Table �� GGX���z �̃e�N�X�`��

#define _shadowTexture		6	// �������� SHADOWMAP_COUNT ���̃T�C�Y���g�p


// sprite
#define _spriteTexture		0

// wboit
#define _accumTexture		0
#define _revealTexture		1

// bloom
#define _hdrColorBufferTexture 0
#define _downSampledTexture 0

// radialBlur
#define _radialBlurTexture	0

// heatHaze
#define _heatHazeTexture 0

// colorFilter
#define _colorFilterTexture 0


// Gpu Particle (�����͕ʂ̃e�N�X�`���ōX�V���Ȃ��悤�ɂ���)
#define _particlesTexture		20
#define _perlinNoiseTexture		21


// --- hlsl ---
#else

// ====== �萔�o�b�t�@ ======
#define _cameraConstant		b0		// �J�����̃f�[�^
#define _objectConstant		b1		// 3d ��Ԃɏo���S�Ẵ��f��������
#define _lightConstant		b2		// ���C�g�̃f�[�^
#define _timerConstant		b3		// �^�C�}�[�̃f�[�^
#define _shadowConstant		b6		// �e�̃f�[�^
#define _emitterConstant	b7		// �G�~�b�^�[�̃f�[�^
#define _uvScrollConstant	b8		// uv�X�N���[���̃f�[�^
#define _dissolveConstant	b9		// �f�B�]���u�̃f�[�^

#define _luminanceExtractionConstant	b5	// ���P�x���o�p
#define _gaussianConstant				b5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			b5	// �K�E�V�A���t�B���^���ϗp

#define _radialBlurConstant		b5	// ���W�A���u���[�p
#define _heatHazeConstant		b6	// heatHaze �p
#define _colorFilterConstant	b7	// �J���[�t�B���^�[�p

// ====== �e�N�X�`���ԍ� ======

// phong
#define _deffuseTexture		t0
#define _normalTexture		t1
#define _specularTexture	t2
#define _emissiveTexture	t3

// pbr
#define _baseTexture				t0
#define _normalTexture				t1
#define _metallicRoughnessTexture	t2
#define _emissiveTexture			t3
#define _occlusionTexture			t4

// dissolve
#define _dissolveTexture	t5

// ibl (���O�v�Z�e�N�X�`��)
#define  _IEM	t33		// Pre-filtered Irradiance Environment Map (IEM)
#define _PMREM	t34		// Pre-filtered Mipmapped Radiance Environment Map (PMREM)
#define _LutGgx t35		// Look-Up Table �� GGX���z �̃e�N�X�`��

#define _shadowTexture		t6		// �������� SHADOWMAP_COUNT ���̃T�C�Y���g�p

// sprite
#define _spriteTexture		t0

// wboit
#define _accumTexture		t0
#define _revealTexture		t1

// bloom
#define _hdrColorBufferTexture t0
#define _downSampledTexture t0

// radialBlur
#define _radialBlurTexture	t0

// heatHaze
#define _heatHazeTexture t0

// colorFilter
#define _colorFilterTexture t0

// Gpu Particle (�����͕ʂ̃e�N�X�`���ōX�V���Ȃ��悤�ɂ���)
#define _particlesTexture		t20
#define _perlinNoiseTexture		t21

#endif


#endif
