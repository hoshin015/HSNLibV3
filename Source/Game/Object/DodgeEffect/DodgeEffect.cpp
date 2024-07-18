#include "DodgeEffect.h"
#include "../../../../Library/Graphics/Shader.h"
#include "../../../../Library/Timer.h"

DodgeEffect::DodgeEffect()
{
	model = std::make_unique<AnimatedModel>("Data/Fbx/GaoPlayer/gaoplayer_6.model");

	CreatePsFromCso("Data/Shader/PbrEmissionPS.cso", pixelShader.GetAddressOf());
}

void DodgeEffect::Update()
{
	for (auto it = dodgeData.begin(); it != dodgeData.end();) {
		(*it)->alpha -= 1.0f * Timer::Instance().DeltaTime();

		if ((*it)->alpha <= 0) { // ‹ô”‚È‚çíœ
			it = dodgeData.erase(it);
		}
		else {
			++it;
		}
	}
}

void DodgeEffect::Render()
{
	for(auto data : dodgeData)
	{
		model->data.materialColorKd.w = data->alpha;
		model->Render(data->trans, &data->key, pixelShader.Get());
	}
}

void DodgeEffect::Clear()
{
	dodgeData.clear();
}

void DodgeEffect::Register(DodgeData* dodge)
{
	dodgeData.emplace_back(dodge);
}
