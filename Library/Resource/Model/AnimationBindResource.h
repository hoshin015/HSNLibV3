#pragma once
#include <string>
#include "../../MyCereal.h"
#include "../../Audio/AudioManager.h"


struct AnimSphereCollision
{
	float				radius;
	int					startFrame;
	int					endFrame;
	std::string			bindBoneName;	// ボーンに連動させて動かす場合はここに名前をいれる

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(radius, startFrame, endFrame, bindBoneName);
	}
};

struct AnimSe
{
	MUSIC_LABEL		musicType = MUSIC_LABEL::BGM_TEST;
	std::string		name;
	int				startFrame;
	int				endFrame;
	bool			isPlay = false;	// 再生されたかどうか

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(musicType, name, startFrame, endFrame);
	}
};


struct AnimBindData
{
	std::string name = "";	// バインドするアニメーションの名前

	std::vector<AnimSphereCollision> animSphereCollisions;
	std::vector<AnimSe> animSes;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name, animSphereCollisions, animSes);
	}
};




// このデータを BaseModel がもつ
struct AnimationBindResource
{
	std::vector<AnimBindData> animBindData;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(animBindData);
	}
};
