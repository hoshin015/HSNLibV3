#include "DamageTextManager.h"

#include "UiClearAfter.h"
#include "../../Library/Timer.h"
#include "../../Library/3D/CameraManager.h"
#include "../../Library/Math/Math.h"

void DamageTextManager::Initialize()
{
	textSprite = std::make_unique<Sprite>("Data/Texture/Font/damageFont.sprite");
	textSprite->UpdateAnimation();
}

void DamageTextManager::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();
	int index = 0;
	for (auto& dmgText : damageTexts)
	{
		dmgText.timer += deltaTime;
		dmgText.screenPosition = Math::WorldToScreenPos(dmgText.worldPosition, CameraManager::Instance().GetCamera().get());
		dmgText.screenPosition.x -= dmgText.screenPositionBuffer.x;
		dmgText.screenPosition.y -= dmgText.screenPositionBuffer.y;

		if(dmgText.timer >= fadeOutTime)
		{
			dmgText.screenPositionBuffer.y += 30 * deltaTime;
		}

		if(dmgText.timer >= lifeTime)
		{
			damageTexts.erase(damageTexts.begin() + index);
			continue;
		}
		index++;
	}
}

void DamageTextManager::Render()
{
	if(!UiClearAfter::Instance().clearFlag)
	{
		for (auto& dmgText : damageTexts)
		{
			textSprite->SprTextOut(dmgText.text, dmgText.screenPosition);
		}
	}
}

void DamageTextManager::Clear()
{
	damageTexts.clear();
}

void DamageTextManager::Register(DamageTextInfo info)
{
	DamageTextInfo dmgText;
	dmgText = info;

	dmgText.screenPositionBuffer.x = textSprite->GetSizeX() * dmgText.text.size() * 0.5f;
	dmgText.screenPositionBuffer.y = textSprite->GetSizeY() * 0.5f;

	damageTexts.emplace_back(dmgText);
}
