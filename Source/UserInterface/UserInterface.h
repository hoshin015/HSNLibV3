#pragma once


class UserInterface
{
private:
	UserInterface(){}
	~UserInterface(){}

public:
	static UserInterface& Instance()
	{
		static  UserInterface instance;
		return instance;
	}

	// ������
	void Initialize();

	// �X�V
	void Update();

	// �`��
	void Render();
};