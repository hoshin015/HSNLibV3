#ifndef RegisterNum_h
#define RegisterNum_h

////////////////////////////////////////////////////////////////////
//
//　c++ 側を変えたら hlsl 側も手動で変える
//
////////////////////////////////////////////////////////////////////

// 定数
#define MAX_INSTANCE					512
#define GAUSSIAN_DOWNSAMPLING_COUNT		1
#define SHADOWMAP_COUNT					4

// サンプラー
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

// 定数バッファ
#define _cameraConstant		0		// カメラのデータ
#define _objectConstant		1		// 3d 空間に出す全てのモデルが持つ 
#define _lightConstant		2		// ライトのデータ
#define _timerConstant		3		// タイマーのデータ
#define _shadowConstant		6		// 影のデータ
#define _emitterConstant	7		// エミッターのデータ

#define _luminanceExtractionConstant	5	// 高輝度抽出用
#define _gaussianConstant				5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			5	// ガウシアンフィルタ平均用

// テクスチャ番号
#define _deffuseTexture		0
#define _normalTexture		1
#define _specularTexture	2
#define _emissiveTexture	3

#define _shadowTexture		4	// ここから SHADOWMAP_COUNT 分のサイズを使用



#define _spriteTexture		0
#define _dissolveTexture	1


// --- hlsl ---
#else

// 定数バッファ
#define _cameraConstant		b0		// カメラのデータ
#define _objectConstant		b1		// 3d 空間に出す全てのモデルが持つ
#define _lightConstant		b2		// ライトのデータ
#define _timerConstant		b3		// タイマーのデータ
#define _shadowConstant		b6		// 影のデータ
#define _emitterConstant	b7		// エミッターのデータ

#define _luminanceExtractionConstant	b5	// 高輝度抽出用
#define _gaussianConstant				b5	// ガウシアンフィルタ用
#define _gaussianAvgConstant			b5	// ガウシアンフィルタ平均用

// テクスチャ番号
#define _deffuseTexture		t0
#define _normalTexture		t1
#define _specularTexture	t2
#define _emissiveTexture	t3
#define _shadowTexture		t4		// ここから SHADOWMAP_COUNT 分のサイズを使用

#define _spriteTexture		t0
#define _dissolveTexture	t1


#endif


#endif