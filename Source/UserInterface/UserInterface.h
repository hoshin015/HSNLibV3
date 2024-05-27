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

	// ‰Šú‰»
	void Initialize();

	// XV
	void Update();

	// •`‰æ
	void Render();
};