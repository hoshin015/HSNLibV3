#include <filesystem>
#include "Text.h"
#include "../Graphics/Graphics.h"

Text::Text()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(dc);

	//---------------�t�H���g�f�[�^---------------

	// meirio
	spriteFont[static_cast<int>(FONTNO::meirio)][static_cast<int>(FONTSIZE::SIZE22)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/���C���I Size22.spritefont");

	// tsunagi
	spriteFont[static_cast<int>(FONTNO::tsunagi)][static_cast<int>(FONTSIZE::SIZE15)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/Tsunagi Gothic Black Size15.spritefont");
	spriteFont[static_cast<int>(FONTNO::tsunagi)][static_cast<int>(FONTSIZE::SIZE22)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/Tsunagi Gothic Black Size22.spritefont");
	spriteFont[static_cast<int>(FONTNO::tsunagi)][static_cast<int>(FONTSIZE::SIZE36)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/Tsunagi Gothic Black Size36.spritefont");

	//x12y16pxMaruMonica
	spriteFont[static_cast<int>(FONTNO::x12y16pxMaruMonica)][static_cast<int>(FONTSIZE::SIZE12)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x12y16pxMaruMonica-12p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x12y16pxMaruMonica)][static_cast<int>(FONTSIZE::SIZE15)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x12y16pxMaruMonica-15p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x12y16pxMaruMonica)][static_cast<int>(FONTSIZE::SIZE22)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x12y16pxMaruMonica-22p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x12y16pxMaruMonica)][static_cast<int>(FONTSIZE::SIZE36)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x12y16pxMaruMonica-36p.spritefont");

	//x8y12pxTheStrongGamer
	spriteFont[static_cast<int>(FONTNO::x8y12pxTheStrongGamer)][static_cast<int>(FONTSIZE::SIZE12)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x8y12pxTheStrongGamer-12p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x8y12pxTheStrongGamer)][static_cast<int>(FONTSIZE::SIZE15)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x8y12pxTheStrongGamer-15p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x8y12pxTheStrongGamer)][static_cast<int>(FONTSIZE::SIZE22)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x8y12pxTheStrongGamer-22p.spritefont");
	spriteFont[static_cast<int>(FONTNO::x8y12pxTheStrongGamer)][static_cast<int>(FONTSIZE::SIZE36)] = std::make_unique<DirectX::SpriteFont>(device, L"./Data/Font/x8y12pxTheStrongGamer-36p.spritefont");
}

Text::~Text()
{
}

void Text::drawText(FONTNO fontNo, FONTSIZE fontSize, std::string str, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 color, TEXT_ALIGN align)
{
	//-------- utf16�֕ϊ� --------
	auto text = std::filesystem::path(str).u16string();

	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//std::wstring wide_string = converter.from_bytes(str);
	//
	//const wchar_t* text = wide_string.c_str();

	//-------- Begin�`End�̊Ԃł���spriteFont�͎g���Ȃ�? --------
	spriteBatch->Begin();

	//�����̕��ƍ����̎擾�E����
	DirectX::XMVECTOR result = spriteFont[static_cast<int>(fontNo)][static_cast<int>(fontSize)]->MeasureString((const wchar_t*)text.c_str());
	float width = DirectX::XMVectorGetX(result);
	float height = DirectX::XMVectorGetY(result);
	float bufferW = getBufferW(align, width);		//�A���C�������g�ɂ�鉡�������l
	float bufferH = getBufferH(align, height);		//�A���C�������g�ɂ��c�������l

	//�����̕`��
	spriteFont[static_cast<int>(fontNo)][static_cast<int>(fontSize)]->DrawString(
		spriteBatch.get(),
		(const wchar_t*)text.c_str(),
		DirectX::XMFLOAT2(position.x + bufferW, position.y + bufferH),
		DirectX::XMVECTORF32{ color.x, color.y, color.z, color.w },
		0.0f,
		DirectX::XMFLOAT2(0, 0),
		DirectX::XMFLOAT2(scale.x, scale.y)
	);

	spriteBatch->End();
	//-------------------------------------------------------

	//���ꂪ�Ȃ��Ɠ��ߏ������ςɂȂ�
	Graphics* gfx = &Graphics::Instance();
	gfx->SetBlend(BLEND_STATE::ALPHA);
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
}


//���������p
float Text::getBufferW(TEXT_ALIGN align, float width)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::LOWER_LEFT:
		return 0.0f;
		break;
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::LOWER_MIDDLE:
		return -width * 0.5f;
		break;
	case TEXT_ALIGN::UPPER_RIGHT:
	case TEXT_ALIGN::MIDDLE_RIGHT:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -width;
	}
	return 0.0f;
}

//�c�������p
float Text::getBufferH(TEXT_ALIGN align, float height)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::UPPER_RIGHT:
		return 0.0f;
		break;
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::MIDDLE_RIGHT:
		return -height * 0.5f;
		break;
	case TEXT_ALIGN::LOWER_LEFT:
	case TEXT_ALIGN::LOWER_MIDDLE:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -height;
	}
	return 0.0f;
}
