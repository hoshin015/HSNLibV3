#pragma once

// ���x����`
enum class FONTNO
{
    meirio,
    tsunagi,
    x12y16pxMaruMonica,
    x8y12pxTheStrongGamer,
    MYFONT4,
    MYFONT5,
    MYFONT6,
};

//Font Size
enum class FONTSIZE
{
    SIZE12,
    SIZE15,
    SIZE22,
    SIZE36,
};

// �����`��̃A���C��
enum class TEXT_ALIGN
{
    UPPER_LEFT = 0, // ����
    UPPER_MIDDLE,   // �㒆��
    UPPER_RIGHT,    // �E��
    MIDDLE_LEFT,    // ������
    MIDDLE,         // ����
    MIDDLE_RIGHT,   // �E����
    LOWER_LEFT,     // ����
    LOWER_MIDDLE,   // ������
    LOWER_RIGHT,    // �E��
};