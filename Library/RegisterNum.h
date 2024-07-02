#ifndef RegisterNum_h
#define RegisterNum_h

////////////////////////////////////////////////////////////////////
//
//　c++ 側を変えたら hlsl 側も手動で変える
//
////////////////////////////////////////////////////////////////////

#define USE_LinearWorkflow				0


// ====== 定数 ======
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		6
#define SHADOWMAP_COUNT					4
#define THREAD_NUM_X					256

// ====== サンプラー ======
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

// ====== 定数バッファ ======
#define _cameraConstant		0		// カメラのデータ
#define _objectConstant		1		// 3d 空間に出す全てのモデルが持つ 
#define _lightConstant		2		// ライトのデータ
#define _timerConstant		3		// タイマーのデータ
#define _shadowConstant		6		// 影のデータ
#define _emitterConstant	7		// エミッターのデータ
#define _uvScrollConstant	8		// uvスクロールのデータ
#define _dissolveConstant	9		// ディゾルブのデータ

#define _luminanceExtractionConstant	5	// 高輝度抽出用
#define _gaussianConstant				5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			5	// ガウシアンフィルタ平均用

#define _radialBlurConstant		5	// ラジアルブラー用
#define _heatHazeConstant		6	// heatHaze 用
#define _colorFilterConstant	7	// カラーフィルター用

// ====== テクスチャ番号 ======

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

// ibl (事前計算テクスチャ)
#define  _IEM	33		// Pre-filtered Irradiance Environment Map (IEM)
#define _PMREM	34		// Pre-filtered Mipmapped Radiance Environment Map (PMREM)
#define _LutGgx 35		// Look-Up Table と GGX分布 のテクスチャ

#define _shadowTexture		6	// ここから SHADOWMAP_COUNT 分のサイズを使用


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


// Gpu Particle (ここは別のテクスチャで更新しないようにする)
#define _particlesTexture		20
#define _perlinNoiseTexture		21


// --- hlsl ---
#else

// ====== 定数バッファ ======
#define _cameraConstant		b0		// カメラのデータ
#define _objectConstant		b1		// 3d 空間に出す全てのモデルが持つ
#define _lightConstant		b2		// ライトのデータ
#define _timerConstant		b3		// タイマーのデータ
#define _shadowConstant		b6		// 影のデータ
#define _emitterConstant	b7		// エミッターのデータ
#define _uvScrollConstant	b8		// uvスクロールのデータ
#define _dissolveConstant	b9		// ディゾルブのデータ

#define _luminanceExtractionConstant	b5	// 高輝度抽出用
#define _gaussianConstant				b5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			b5	// ガウシアンフィルタ平均用

#define _radialBlurConstant		b5	// ラジアルブラー用
#define _heatHazeConstant		b6	// heatHaze 用
#define _colorFilterConstant	b7	// カラーフィルター用

// ====== テクスチャ番号 ======

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

// ibl (事前計算テクスチャ)
#define  _IEM	t33		// Pre-filtered Irradiance Environment Map (IEM)
#define _PMREM	t34		// Pre-filtered Mipmapped Radiance Environment Map (PMREM)
#define _LutGgx t35		// Look-Up Table と GGX分布 のテクスチャ

#define _shadowTexture		t6		// ここから SHADOWMAP_COUNT 分のサイズを使用

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

// Gpu Particle (ここは別のテクスチャで更新しないようにする)
#define _particlesTexture		t20
#define _perlinNoiseTexture		t21

#endif


#endif
