#pragma once
// ���������[�N�̏ꏊ�\��
#if _DEBUG
#define new_ new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define new_ new
#endif