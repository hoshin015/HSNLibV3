#include "Video.h"

#include <vector>

#include "../Timer.h"
#include "../../../Library/Graphics/Shader.h"

void MoveUV(BYTE* yuvBuffer, int bufferSize, int width, int height, int* movePos) {
	int yEnd = width * height;
	if (*movePos < 0) {
		size_t uvStart = yEnd;
		for (; yuvBuffer[uvStart] == 0; uvStart += width);
		*movePos = uvStart;
	}

	if (movePos != nullptr) memmove(&yuvBuffer[yEnd], &yuvBuffer[(*movePos)], bufferSize - (*movePos));
}

void Video::InitializeVideo(ID3D11Device* device) {
	HRESULT              hr = {};
	ComPtr<IMFMediaType> nativeType;
	ComPtr<IMFMediaType> videoTypeNV12;
	ComPtr<IMFMediaType> videoTypeARGB;

	GUID   subtype;
	UINT32 avgBitrate;
	UINT32 interlace;
	UINT32 frame,  ratio;
	UINT32 width,  height;
	UINT32 pixelX, pixelY;

	hr = _sourceReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nativeType.GetAddressOf());
	hr = nativeType->GetGUID(MF_MT_SUBTYPE, &subtype);
	//hr = nativeType->GetUINT32(MF_MT_SAMPLE_SIZE, &sampleSize);
	hr = nativeType->GetUINT32(MF_MT_INTERLACE_MODE, &interlace);
	hr = nativeType->GetUINT32(MF_MT_AVG_BITRATE, &avgBitrate);
	hr = MFGetAttributeRatio(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_FRAME_RATE, &frame, &ratio);
	hr = MFGetAttributeSize(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_FRAME_SIZE, &width, &height);
	hr = MFGetAttributeRatio(static_cast<IMFAttributes*>(nativeType.Get()), MF_MT_PIXEL_ASPECT_RATIO, &pixelX, &pixelY);

	_size      = {static_cast<float>(width), static_cast<float>(height)};
	_frameRate = static_cast<float>(frame) / ratio;

	MFCreateMediaType(videoTypeNV12.GetAddressOf());
	nativeType->CopyAllItems(videoTypeNV12.Get());

	//MediaTypeの設定(下は必須)
	hr = videoTypeNV12->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = videoTypeNV12->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
	hr = videoTypeNV12->SetUINT32(MF_MT_DEFAULT_STRIDE, width);
	hr = videoTypeNV12->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeNV12.Get(), MF_MT_FRAME_RATE, frame, ratio);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeNV12.Get(), MF_MT_FRAME_SIZE, width, height);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeNV12.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
	hr = _sourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, videoTypeNV12.Get());

	MFCreateMediaType(videoTypeARGB.GetAddressOf());
	nativeType->CopyAllItems(videoTypeARGB.Get());

	//MediaTypeの設定(下は必須)
	hr = videoTypeARGB->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	hr = videoTypeARGB->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_ARGB32);
	hr = videoTypeARGB->SetUINT32(MF_MT_DEFAULT_STRIDE, width * 4); // 値を入れないと何故か反転するくせに最大値入れなければ何でもいい謎
	hr = videoTypeARGB->SetUINT32(MF_MT_AVG_BITRATE, avgBitrate);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeARGB.Get(), MF_MT_FRAME_RATE, frame, ratio);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeARGB.Get(), MF_MT_FRAME_SIZE, width, height);
	hr = MFSetAttributeRatio((IMFAttributes*)videoTypeARGB.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

	//MFT
	IMFActivate** activates     = nullptr;
	UINT32        num_actuvates = 0;
	MFTEnumEx(MFT_CATEGORY_VIDEO_PROCESSOR, MFT_ENUM_FLAG_ALL, nullptr, nullptr, &activates, &num_actuvates);

	activates[0]->ActivateObject(_uuidof(IMFTransform), reinterpret_cast<void**>(_transform.GetAddressOf()));

#if _DEBUG
	LPWSTR friendlyName;
	UINT32 nameLength;
	activates[0]->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute, &friendlyName, &nameLength);
	OutputDebugString(friendlyName);
	OutputDebugString(L"\n");
#endif // _DEBUG

	activates[0]->Release();
	CoTaskMemFree(activates);

	hr = _transform->SetInputType(0, videoTypeNV12.Get(), 0);
	hr = _transform->SetOutputType(0, videoTypeARGB.Get(), 0);

	//テクスチャの作成
	CD3D11_TEXTURE2D_DESC textureDesc(
		DXGI_FORMAT_B8G8R8A8_UNORM, static_cast<UINT>(_size.x), static_cast<UINT>(_size.y)
	);
	textureDesc.Usage          = D3D11_USAGE_DYNAMIC;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MipLevels      = 1;

	std::vector<UINT32> dummyData;
	dummyData.resize(_size.x * _size.y, 0);
	D3D11_SUBRESOURCE_DATA texSubresourceData = {};
	texSubresourceData.pSysMem                = dummyData.data();
	texSubresourceData.SysMemPitch            = static_cast<size_t>(_size.x) * sizeof(UINT32);

	hr = device->CreateTexture2D(&textureDesc, &texSubresourceData, _texture.GetAddressOf());

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM, 0, 1);
	hr = device->CreateShaderResourceView(_texture.Get(), &srvDesc, _textureView.GetAddressOf());
}

void Video::Initialize(ID3D11Device* device) {
	MFStartup(MF_VERSION);
	CreateCommonBuffer(device);
}

void Video::Finalize() { MFShutdown(); }

void Video::CreateCommonBuffer(ID3D11Device* device) {
	static bool isCreated = false;
	if (isCreated)return;
	HRESULT hr = {};

	//頂点
	Vertex vertices[] =
	{
	  {{ -0.5f,  0.5f, 0 }, { 0, 0 }},
	  {{  0.5f,  0.5f, 0 }, { 1, 0 }},
	  {{ -0.5f, -0.5f, 0 }, { 0, 1 }},
	  {{  0.5f, -0.5f, 0 }, { 1, 1 }},
	};

	//頂点バッファの作成
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = {};
	vertexBufferData.pSysMem = vertices;

	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, _vertexBuffer.GetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	CreateVsFromCso("Data/Shader/Video_VS.cso", _vertexShader.GetAddressOf(), _inputLayout.GetAddressOf(), inputElementDesc,2);
	CreatePsFromCso("Data/Shader/Video_PS.cso", _pixelShader.GetAddressOf());
}

void Video::LoadFile(ID3D11Device* device, const wchar_t* filePath) {
	MFCreateSourceReaderFromURL(filePath, nullptr, _sourceReader.GetAddressOf());
	InitializeVideo(device);
}

void Video::Update(ID3D11DeviceContext* deviceContext) {
	if (!_isPlay) return;
	//double elapsedTime = Timer::LaunchTimer().GetMilisecondsElapsed() * 1000;
	_timer += Timer::Instance().UnscaledDeltaTime() * 1000;
	if (_timer * 10000 < _timeStamp) return;

	ComPtr<IMFSample> sample;
	DWORD             striamIndex, dwStreamFlags;
	HRESULT           hr = {};

	//次のフレームのTextureを取得
	hr = _sourceReader->ReadSample(
		MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &striamIndex, &dwStreamFlags, &_timeStamp, sample.GetAddressOf()
	);
	if (!(dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)) {
		//YからUVまでのデータを移動する
		ComPtr<IMFMediaBuffer> yuvBuffer;
		{
			sample->GetBufferByIndex(0, yuvBuffer.GetAddressOf());

			BYTE* bBuffer;
			DWORD maxlen, currentlen;
			yuvBuffer->Lock(&bBuffer, &maxlen, &currentlen);

			MoveUV(bBuffer, maxlen, _size.x, _size.y, &_moveUVPos);

			yuvBuffer->Unlock();

			sample->RemoveAllBuffers();
			sample->AddBuffer(yuvBuffer.Get());
		}

		//IMFTransformはデータをインプットするときはしてアウトプットするときはアウトプットのみする
		//同時にやってしまうとインプットができない
		hr = _transform->ProcessInput(0, sample.Get(), 0);

		DWORD status;
		hr = _transform->GetOutputStatus(&status);
		if (status & MFT_OUTPUT_STATUS_SAMPLE_READY) {
			while (true) {
				//デコーダで処理される出力バッファの大きさを取得する
				ComPtr<IMFMediaBuffer> buffer;
				MFT_OUTPUT_STREAM_INFO streamInfo;
				ZeroMemory(&streamInfo, sizeof(MFT_OUTPUT_STREAM_INFO));
				hr = _transform->GetOutputStreamInfo(0, &streamInfo);
				hr = MFCreateMemoryBuffer(streamInfo.cbSize, buffer.GetAddressOf());

				//バッファを取得
				MFT_OUTPUT_DATA_BUFFER outBuffer;
				ZeroMemory(&outBuffer, sizeof(MFT_OUTPUT_DATA_BUFFER));
				hr = MFCreateSample(&outBuffer.pSample);
				outBuffer.pSample->AddBuffer(buffer.Get());
				hr = _transform->ProcessOutput(0, 1, &outBuffer, &status);

				//出力できるデータがなければループを抜ける
				if (FAILED(hr)) {
					outBuffer.pSample->RemoveAllBuffers();
					outBuffer.pSample->Release();
					break;
				}

				//Texture2Dに書き込み
				BYTE* bBuffer;
				DWORD maxlen, currentlen;

				hr = buffer->Lock(&bBuffer, &maxlen, &currentlen);

				D3D11_MAPPED_SUBRESOURCE subresource;
				deviceContext->Map(_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				memcpy(subresource.pData, bBuffer, maxlen);
				deviceContext->Unmap(_texture.Get(), 0);

				hr = buffer->Unlock();

				//終了処理
				outBuffer.pSample->RemoveAllBuffers();
				outBuffer.pSample->Release();
				_transform->ProcessMessage(MFT_MESSAGE_NOTIFY_END_OF_STREAM, 0);
				_transform->ProcessMessage(MFT_MESSAGE_COMMAND_DRAIN, 0);
			}
		}
	}
	else {
		//再生場所を0に設定
		SeekPosition(0);

		if (_isLoop) _pauseTime = _timer;
		else _isPlay            = false;
	}
}

bool Video::Play(bool loop) {
	if (!_isPlay) {
		_isPlay    = true;
		_isLoop    = loop;
		//_pauseTime = Timer::Instance().UnscaledDeltaTime()* 1000 + _timeStamp * -0.0000001f;
		return true;
	}
	return false;
}

void Video::Pause() { _isPlay = false; }

void Video::Stop() {
	_isPlay = false;
	SeekPosition(0);
}

void Video::SeekPosition(LONGLONG seekTime) {
	if (_sourceReader.Get() == nullptr) return;
	_timeStamp = seekTime;
	PROPVARIANT time;
	PropVariantInit(&time);
	time.vt   = VT_I8;
	time.lVal = seekTime;
	_sourceReader->SetCurrentPosition(GUID_NULL, time);
	PropVariantClear(&time);
	_timer = _timeStamp * -0.0000001f;
}

void Video::Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {
	using namespace DirectX;
	if (_sourceReader.Get() == nullptr) return;
	D3D11_VIEWPORT viewport {};
	UINT numViewports { 1 };
	deviceContext->RSGetViewports(&numViewports, &viewport);

	// left top
	XMFLOAT2 lt = dPos;
	// right top
	XMFLOAT2 rt = { dPos.x + dSize.x, dPos.y };
	// left bottom
	XMFLOAT2 lb = { dPos.x, dPos.y + dSize.y };
	// right bottom
	XMFLOAT2 rb = { dPos.x + dSize.x, dPos.y + dSize.y };

	//--- < 頂点回転処理 > ---

	// 回転の中心を矩形の中心点にした場合
	XMFLOAT2 cPos = { rb.x * 0.5f,rb.y * 0.5f };

	// float cos { cosf(DirectX::XMConvertToRadians(angle)) };
	// float sin { sinf(DirectX::XMConvertToRadians(angle)) };
	//
	// Rotate(lt, cPos, cos, sin);
	// Rotate(rt, cPos, cos, sin);
	// Rotate(lb, cPos, cos, sin);
	// Rotate(rb, cPos, cos, sin);
	//
	//--- < スクリーン座標系からNDCへ座標変換 > ---
	lt.x = 2.0f * lt.x / viewport.Width - 1.0f;
	lt.y = 1.0f - 2.0f * lt.y / viewport.Height;
	rt.x = 2.0f * rt.x / viewport.Width - 1.0f;
	rt.y = 1.0f - 2.0f * rt.y / viewport.Height;
	lb.x = 2.0f * lb.x / viewport.Width - 1.0f;
	lb.y = 1.0f - 2.0f * lb.y / viewport.Height;
	rb.x = 2.0f * rb.x / viewport.Width - 1.0f;
	rb.y = 1.0f - 2.0f * rb.y / viewport.Height;


	//--- < テクセル座標系からUV座標系への変換 > ---
	float u0 { sPos.x / _size.x };
	float v0 { sPos.y / _size.y };
	float u1 { (sPos.x + sSize.x) / _size.x };
	float v1 { (sPos.y + sSize.y) / _size.y };

	//--- < 計算結果で頂点バッファオブジェクトを更新する > ---
	HRESULT hr { S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource {};
	hr = deviceContext->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	Vertex* vertices { reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr) {
		vertices[0].position = { lt.x ,lt.y, 0 };
		vertices[1].position = { rt.x ,rt.y, 0 };
		vertices[2].position = { lb.x ,lb.y, 0 };
		vertices[3].position = { rb.x ,rb.y, 0 };

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	deviceContext->Unmap(_vertexBuffer.Get(), 0);

	//--- < 頂点バッファーのバインド > ---
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	deviceContext->IASetInputLayout(_inputLayout.Get());

	//--- < シェーダーのバインド > ---
	deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);

	//--- < シェーダーリソースのバインド > ---
	deviceContext->PSSetShaderResources(0, 1, _textureView.GetAddressOf());

	//--- < プリミティブの描画 > ---
	deviceContext->Draw(4, 0);
}

// void Video::Render(ID3D11DeviceContext* deviceContext) {
// 	deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
// 	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
// 	deviceContext->IASetInputLayout(nullptr);
//
// 	//--- < シェーダーのバインド > ---
// 	deviceContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
// 	deviceContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
//
// 	//--- < シェーダーリソースのバインド > ---
// 	deviceContext->PSSetShaderResources(0, 1, _textureView.GetAddressOf());
//
// 	//--- < プリミティブの描画 > ---
// 	deviceContext->Draw(4, 0);
// }

//void Video::Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 dPos, DirectX::XMFLOAT2 dSize, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 sPos, DirectX::XMFLOAT2 sSize) {}
