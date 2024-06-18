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

// --- c++ ---
#ifdef __cplusplus

// ====== �萔�o�b�t�@ ======
#define _cameraConstant		0		// �J�����̃f�[�^
#define _objectConstant		1		// 3d ��Ԃɏo���S�Ẵ��f�������� 
#define _lightConstant		2		// ���C�g�̃f�[�^
#define _timerConstant		3		// �^�C�}�[�̃f�[�^
#define _shadowConstant		6		// �e�̃f�[�^
#define _emitterConstant	7		// �G�~�b�^�[�̃f�[�^

#define _luminanceExtractionConstant	5	// ���P�x���o�p
#define _gaussianConstant				5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			5	// �K�E�V�A���t�B���^���ϗp

#define _radialBlurConstant		5	// ���W�A���u���[�p

// ====== �e�N�X�`���ԍ� ======
#define _deffuseTexture		0
#define _normalTexture		1
#define _specularTexture	2
#define _emissiveTexture	3

#define _shadowTexture		4	// �������� SHADOWMAP_COUNT ���̃T�C�Y���g�p

// sprite
#define _spriteTexture		0
#define _dissolveTexture	1

// wboit
#define _accumTexture		0
#define _revealTexture		1

// bloom
#define _hdrColorBufferTexture 0
#define _downSampledTexture 0

// radialBlur
#define _radialBlurTexture	0

// Gpu Particle (�����͕ʂ̃e�N�X�`���ōX�V���Ȃ��悤�ɂ���)
#define _particlesTexture	9


// --- hlsl ---
#else

// ====== �萔�o�b�t�@ ======
#define _cameraConstant		b0		// �J�����̃f�[�^
#define _objectConstant		b1		// 3d ��Ԃɏo���S�Ẵ��f��������
#define _lightConstant		b2		// ���C�g�̃f�[�^
#define _timerConstant		b3		// �^�C�}�[�̃f�[�^
#define _shadowConstant		b6		// �e�̃f�[�^
#define _emitterConstant	b7		// �G�~�b�^�[�̃f�[�^

#define _luminanceExtractionConstant	b5	// ���P�x���o�p
#define _gaussianConstant				b5	// �K�E�V�A���t�B���^�p
#define _gaussianAvgConstant			b5	// �K�E�V�A���t�B���^���ϗp

#define _radialBlurConstant		b5	// ���W�A���u���[�p

// ====== �e�N�X�`���ԍ� ======
#define _deffuseTexture		t0
#define _normalTexture		t1
#define _specularTexture	t2
#define _emissiveTexture	t3
#define _shadowTexture		t4		// �������� SHADOWMAP_COUNT ���̃T�C�Y���g�p

// sprite
#define _spriteTexture		t0
#define _dissolveTexture	t1

// wboit
#define _accumTexture		t0
#define _revealTexture		t1

// bloom
#define _hdrColorBufferTexture t0
#define _downSampledTexture t0

// radialBlur
#define _radialBlurTexture	t0

// Gpu Particle (�����͕ʂ̃e�N�X�`���ōX�V���Ȃ��悤�ɂ���)
#define _particlesTexture	t9

#endif


#endif
