// 円周率
static const float PI = 3.141592654f;
// ガンマ係数
static const float GammaFactor = 2.2f;

//--------------------------------------------
//	フレネル項
//--------------------------------------------
//F0	: 垂直入射時の反射率
//VdotH	: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル
float3 CalcFresnel(float3 F0, float VdotH)
{
	return F0 + (1.0f - F0) * pow(clamp(1.0f - VdotH, 0.0f, 1.0f), 5.0f);
}

//--------------------------------------------
//	拡散反射BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
//VdotH		: 視線へのベクトルとハーフベクトルとの内積
//fresnelF0	: 垂直入射時のフレネル反射色
//diffuseReflectance	: 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float VdotH, float3 fresnelF0, float3 diffuseReflectance)
{
	return (1.0f - CalcFresnel(fresnelF0, VdotH)) * (diffuseReflectance / PI);
}

//--------------------------------------------
//	法線分布関数
//--------------------------------------------
//NdotH		: 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
//roughness : 粗さ
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float b = (NdotH * NdotH) * (a - 1.0f) + 1.0f;
	return a / (PI * b * b);
}

//--------------------------------------------
//	幾何減衰項の算出
//--------------------------------------------
//NdotL		: 法線ベクトルと光源へのベクトルとの内積
//NdotV		: 法線ベクトルと視線へのベクトルとの内積
//roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
	float r         = roughness * 0.5f;
	float shadowing = NdotL / (NdotL * (1.0 - r) + r);
	float masking   = NdotV / (NdotV * (1.0 - r) + r);
	return shadowing * masking;
}

//--------------------------------------------
//	鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
//NdotV		: 法線ベクトルと視線へのベクトルとの内積
//NdotL		: 法線ベクトルと光源へのベクトルとの内積
//NdotH		: 法線ベクトルとハーフベクトルとの内積
//VdotH		: 視線へのベクトルとハーフベクトルとの内積
//fresnelF0	: 垂直入射時のフレネル反射色
//roughness	: 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	//	D項(法線分布)
	float D = CalcNormalDistributionFunction(NdotH, roughness);
	//	G項(幾何減衰項)
	float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	//	F項(フレネル反射)
	float3 F = CalcFresnel(fresnelF0, VdotH);

	return D * G * F / (NdotL * NdotV * 4.0f);
}

//--------------------------------------------
//	直接光の物理ベースライティング
//--------------------------------------------
//diffuseReflectance	: 入射光のうち拡散反射になる割合
//F0					: 垂直入射時のフレネル反射色
//normal				: 法線ベクトル(正規化済み)
//eyeVector				: 視点に向かうベクトル(正規化済み)
//lightVector			: 光源に向かうベクトル(正規化済み)
//lightColor			: ライトカラー
//roughness				: 粗さ
void DirectBDRF(float3     diffuseReflectance,
                float3     F0,
                float3     normal,
                float3     eyeVector,
                float3     lightVector,
                float3     lightColor,
                float      roughness,
                out float3 outDiffuse,
                out float3 outSpecular)
{
	float3 N = normal;           // 法線ベクトル
	float3 L = -lightVector;     // 光源への正規化済みベクトル
	float3 V = -eyeVector;       // 視線への正規化済みベクトル
	float3 H = normalize(L + V); // 光源へのベクトルと視線へのベクトルのハーフベクトル

	float NdotV = max(0.0001f, dot(N, V)); // 法線ベクトルと視線へのベクトルとの内積
	float NdotL = max(0.0001f, dot(N, L)); // 法線ベクトルと光源へのベクトルとの内積
	float NdotH = max(0.0001f, dot(N, H)); // 法線ベクトルとハーフベクトルとの内積
	float VdotH = max(0.0001f, dot(V, H)); // 視線ベクトルとハーフベクトルとの内積

	// 照度(入射光)
	float3 irradiance = lightColor * NdotL;

	//	拡散反射BRDF
	outDiffuse = DiffuseBRDF(VdotH, F0, diffuseReflectance) * irradiance;

	//	鏡面反射BRDF
	outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}


//--------------------------------------------
//	ルックアップテーブルからGGX項を取得
//--------------------------------------------
//brdfSamplePoint	: サンプリングポイント
//lutGgxMap         : GGXルックアップテーブル
//state             : 参照時のサンプラーステート
float4 SampleLutGGX(float2 brdfSamplePoint, Texture2D lutGgxMap, SamplerState state)
{
	return lutGgxMap.Sample(state, brdfSamplePoint);
}

//--------------------------------------------
//	キューブマップから照度を取得
//--------------------------------------------
//v                     : サンプリング方向
//diffuseIemCubeMap     : 事前計算拡散反射IBLキューブマップ
//state                 : 参照時のサンプラーステート
float4 SampleDiffuseIEM(float3 v, TextureCube diffuseIemCubeMap, SamplerState state)
{
	return diffuseIemCubeMap.Sample(state, v);
}

//--------------------------------------------
//	キューブマップから放射輝度を取得
//--------------------------------------------
//v	                        : サンプリング方向
//roughness                 : 粗さ
//specularPmremCubeMap		: 事前計算鏡面反射IBLキューブマップ
//state                     : 参照時のサンプラーステート
float4 SampleSpecularPMREM(float3 v, float roughness, TextureCube specularPmremCubeMap, SamplerState state)
{
	//  ミップマップによって粗さを表現するため、段階を算出
	uint width, height, mip_maps;
	specularPmremCubeMap.GetDimensions(0, width, height, mip_maps);
	float lod = roughness * float(mip_maps - 1);
	return specularPmremCubeMap.SampleLevel(state, v, lod);
}

//--------------------------------------------
//	粗さを考慮したフレネル項の近似式
//--------------------------------------------
//F0	    : 垂直入射時の反射率
//VdotN 	: 視線ベクトルと法線ベクトルとの内積
//roughness	: 粗さ
float3 CalcFresnelRoughness(float3 f0, float NdotV, float roughness)
{
	return f0 + (max((float3)(1.0f - roughness), f0) - f0) * pow(saturate(1.0f - NdotV), 5.0f);
}

//--------------------------------------------
//	拡散反射IBL
//--------------------------------------------
//normal                : 法線(正規化済み)
//eyeVector		        : 視線ベクトル(正規化済み)
//roughness				: 粗さ
//diffuseReflectance	: 入射光のうち拡散反射になる割合
//F0					: 垂直入射時のフレネル反射色
//diffuseIemCubeMap     : 事前計算拡散反射IBLキューブマップ
//state                 : 参照時のサンプラーステート
float3 DiffuseIBL(float3      normal, float3 eyeVector, float roughness, float3 diffuseReflectance, float3 f0,
                  TextureCube diffuseIemCubeMap, SamplerState state)
{
	float3 N = normal;
	float3 V = -eyeVector;

	//  間接拡散反射光の反射率計算
	float  NdotV = max(0.0001f, dot(N, V));
	float3 kD    = 1.0f - CalcFresnelRoughness(f0, NdotV, roughness);

	// 事前計算部分(積分部分は計算に時間がかかるため事前にテクスチャに書きこんでいる)
	float3 irradiance = SampleDiffuseIEM(normal, diffuseIemCubeMap, state).rgb;
	return diffuseReflectance * irradiance * kD;
}

//--------------------------------------------
//	鏡面反射IBL
//--------------------------------------------
//normal				    : 法線ベクトル(正規化済み)
//eyeVector					: 視線ベクトル(正規化済み)
//roughness				    : 粗さ
//F0					    : 垂直入射時のフレネル反射色
//lutGgxMap					: GGXルックアップテーブル
//specularPmremCubeMap		 : 事前計算鏡面反射IBLキューブマップ
//state                     : 参照時のサンプラーステート
float3 SpecularIBL(float3      normal, float3 eyeVector, float roughness, float3 f0, Texture2D lutGgxMap,
                   TextureCube specularPmremCubeMap, SamplerState state)
{
	float3 N = normal;
	float3 V = -eyeVector;

	float  NdotV         = max(0.0001f, dot(N, V));
	float3 R             = normalize(reflect(-V, N));
	float3 specularLight = SampleSpecularPMREM(R, roughness, specularPmremCubeMap, state).rgb;

	float2 brdfSamplePoint = saturate(float2(NdotV, roughness));
	float2 envBrdf         = SampleLutGGX(brdfSamplePoint, lutGgxMap, state).rg;
	return specularLight * (f0 * envBrdf.x + envBrdf.y);
}

