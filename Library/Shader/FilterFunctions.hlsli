//--------------------------------------------
//	RGB�F��Ԃ̐��l����HSV�F��Ԃ̐��l�ւ̕ϊ��֐�
//--------------------------------------------
//rgb:RGB�F��Ԃ̐��l
float3 RGB2HSV(float3 rgb)
{
    float3 hsv = 0;
    
    float vMax = max(rgb.r, max(rgb.g, rgb.b)); // RGB�̂R�̒l�ōő�̂���
    float vMin = min(rgb.r, min(rgb.g, rgb.b)); // RGB�̂R�̒l�ōŏ��̂���
    float delta = vMax - vMin;                  // �ő�l�ƍŏ��l�̍�
    
    // V (���x) ��ԋ����F��V�l�ɂ���
    hsv.z = vMax;
    
    // S (�ʓx) �ő�l�ƍŏ��l�̍��𐳋K�����ċ��߂�
    hsv.y = (delta / vMax) * step(0, vMax);
    
    // H (�F��) RGB�̂����ő�l�ƍŏ��l�̍����狁�߂�
    if(hsv.y > 0.0f)
    {
        if(rgb.r == vMax)
        {
            hsv.x = 60 * ((rgb.g - rgb.b) / delta);
        }
        else if(rgb.g == vMax)
        {
            hsv.x = 60 * ((rgb.b - rgb.r) / delta) + 120;
        }
        else
        {
            hsv.x = 60 * ((rgb.r - rgb.g) / delta) + 240;
        }
        
        if(hsv.x < 0)
        {
            hsv.x += 360;
        }
    }
    return hsv;
}

//--------------------------------------------
//	HSV�F��Ԃ̐��l����RGB�F��Ԃ̐��l�ւ̕ϊ��֐�
//--------------------------------------------
//hsv:HSV�F��Ԃ̐��l
float3 HSV2RGB(float3 hsv)
{
    float3 rgb = 0;
    
    if(hsv.y == 0)
    {
        // S (�ʓx) ���O�Ɠ������Ȃ疳�F�������͊D�F
        rgb.r = rgb.g = rgb.b = hsv.z;
    }
    else
    {
        // �F���� H (�F��) �̈ʒu�� S (�ʓx) �A V (���x) ����RGB�l���Z�o����
        float vMax = hsv.z;
        float vMin = vMax - (hsv.y * vMax);
        
        hsv.x %= 360;   // 0 �` 360 �ɕϊ�
        float Huei = (int) (hsv.x / 60);
        float Huef = hsv.x / 60 - Huei;
        float p = vMax * (1.0f - hsv.y);
        float q = vMax * (1.0f - hsv.y * Huef);
        float t = vMax * (1.0f - hsv.y * (1 - Huef));
        if(Huei == 0)
        {
            rgb.r = vMax;
            rgb.g = t;
            rgb.b = p;
        }
        else if(Huei == 1)
        {
            rgb.r = q;
            rgb.g = vMax;
            rgb.b = p;
        }
        else if(Huei == 2)
        {
            rgb.r = p;
            rgb.g = vMax;
            rgb.b = t;
        }
        else if(Huei == 3)
        {
            rgb.r = p;
            rgb.g = q;
            rgb.b = vMax;
        }
        else if(Huei == 4)
        {
            rgb.r = t;
            rgb.g = p;
            rgb.b = vMax;
        }
        else if(Huei == 5)
        {
            rgb.r = vMax;
            rgb.g = p;
            rgb.b = q;
        }
    }
    return rgb;
}

//--------------------------------------------
//	RGB�F��Ԃ̐��l����P�x�l�ւ̕ϊ��֐�
//--------------------------------------------
//rgb:RGB�F��Ԃ̐��l
float RGB2Luminance(float3 rgb)
{
    //static const float3 luminanceValue = float3(0.299f, 0.587f, 0.114f);
    static const float3 luminanceValue = float3(0.2125f, 0.75154f, 0.0721f);
    return dot(luminanceValue, rgb);
}