#pragma once
#include <string>
#include "../../MyCereal.h"
#include "../../Audio/AudioManager.h"

class AnimBindResource
{
public:
	// �R���W����
	struct AnimSphereCollision
	{
		float				radius;
		int					startFrame;
		int					endFrame;
		std::string			bindBoneName;	// �{�[���ɘA�������ē������ꍇ�͂����ɖ��O�������

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(radius, startFrame, endFrame, bindBoneName);
		}
	};

	// ���ʉ�
	struct AnimSe
	{
		MUSIC_LABEL		musicType = MUSIC_LABEL::BGM_TEST;
		std::string		name;
		int				startFrame;
		int				endFrame;
		bool			isPlay = false;	// �Đ����ꂽ���ǂ���

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(musicType, name, startFrame, endFrame);
		}
	};


	// �A�j���[�V�������Ƃɂ��������
	struct AnimBindData
	{
		std::string name = "";	// �o�C���h����A�j���[�V�����̖��O

		std::vector<AnimSphereCollision> animSphereCollisions;
		std::vector<AnimSe> animSes;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, animSphereCollisions, animSes);
		}
	};

	// ���̃f�[�^�� BaseModel ������
	struct AnimationBindResource
	{
		std::vector<AnimBindData> animBindData;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(animBindData);
		}
	};


};

