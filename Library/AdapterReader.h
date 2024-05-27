#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>


class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);	// adapter�̃|�C���^�[���󂯎��
	IDXGIAdapter* pAdapter = nullptr;		// �󂯎����adapter�̃|�C���^�̕ۑ���
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	static std::vector<AdapterData> adapters;
};