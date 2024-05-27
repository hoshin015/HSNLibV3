#include "Player.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Timer.h"
#include "../../Library/Math/Math.h"
#include "../../Library/Math/Collision.h"
#include "../../Source/Game/Object/Stage/StageManager.h"
#include "../../ImGui/imgui.h"

Player::Player(const char* filePath,bool left) : AnimatedObject(filePath)
{
	model->GetModelResource()->SetScale(0.01f);
    this->left = left;
}

void Player::Update()
{
    InputMove();
    UpdateVelocity();
	// �A�j���[�V�����X�V
	//UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void Player::Render(bool isShadow)
{
	model->Render(transform, &keyFrame, isShadow);
}

void Player::DrawDebugImGui(int number)
{
    std::stringstream ss;
    ss << "Player" << number;
    std::string s = ss.str();
    //ImGui::Begin(s.c_str());
    if (ImGui::CollapsingHeader(s.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::string pos = s + "Position";
        ImGui::SliderFloat3(pos.c_str(), &position.x, -100, 100);
        std::string vel = s + "Velocity";
        ImGui::SliderFloat3(vel.c_str(), &velocity.x, -100, 100);
        std::string max = s + "maxSpeed";
        ImGui::SliderFloat(max.c_str(), &maxSpeed, 0, 10);
        std::string z = s + "SpeedZ";
        ImGui::SliderFloat(z.c_str(), &speedZ, -10, 0);
    }
}

void Player::ChangePlayerAcceleration(float value, float factor)
{
    accelerationZ = accelerationZ + factor * (value - accelerationZ);
}

void Player::ChangePlayerPosition(DirectX::XMFLOAT3 value, float factor)
{
    DirectX::XMVECTOR Value = DirectX::XMLoadFloat3(&value);
    DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);

    Position = DirectX::XMVectorLerp(Position, Value, factor);
    DirectX::XMStoreFloat3(&position, Position);
}

void Player::InputMove()
{
    GetMoveVec();
    Turn(moveVecX, moveVecZ, turnSpeed);
}

void Player::Turn(float vx, float vz, float speed)
{
    speed *= Timer::Instance().DeltaTime();

    //�i�s�׃N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    DirectX::XMFLOAT3 vec = { vx,0.0f,vz };
    //DirectX::XMFLOAT3 vec = GetMoveVec();
    if (vec.x == 0.0f && vec.z == 0.0f) return;

    DirectX::XMVECTOR Vec = DirectX::XMLoadFloat3(&vec);

    //�i�s�x�N�g���̒P�ʉ�
    DirectX::XMVector3Normalize(Vec);
    DirectX::XMStoreFloat3(&vec, Vec);

    //���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    //��]�p�����߂邽�߂ɁA2�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (vec.x * frontX) + (vec.z * frontZ);

    //���ϒl��-1.0�`1.0�ŕ\������Ă���A2�̒P�ʃx�N�g���̊p�x��
    //�������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0 - dot;

    //���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (vec.z * frontX) - (vec.x * frontZ);

    //2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    //���E������s�����Ƃɂ���č��E��]��I������
    if (cross < 0.0f)
    {
        angle.y += speed * rot;
    }
    else
    {
        angle.y -= speed * rot;
    }
}

void Player::UpdateVelocity()
{
    //�o�߃t���[��
    float elapsedFrame = 60.0f * Timer::Instance().DeltaTime();

    //�������͍X�V����
    UpdateVerticalVelocity(elapsedFrame);

    //�������͍X�V����
    UpdateHorizontalVelocity(elapsedFrame);

    //�����ړ��X�V����
    UpdateVerticalMove();

    //�����ړ��X�V����
    UpdateHorizontalMove();
}

void Player::GetMoveVec()
{
    //���͏����擾
    InputManager& gamePad = InputManager::Instance();
    float ax = 0.0f;
    float ay = 0.0f;

    //����̃L�[����ʂ���(WASD�Ə\���L�[)
    if (left)
    {
        if (gamePad.GetKeyPress(Keyboard::D))
        {
            ax = 1.0f;
        }
        else if (gamePad.GetKeyPress(Keyboard::A))
        {
            ax = -1.0f;
        }
        if (gamePad.GetKeyPress(Keyboard::W))
        {
            ay = 1.0f;
        }
        else if (gamePad.GetKeyPress(Keyboard::S))
        {
            ay = -1.0f;
        }
    }
    else
    {
        if (gamePad.GetKeyPress(Keyboard::Right))
        {
            ax = 1.0f;
        }
        else if (gamePad.GetKeyPress(Keyboard::Left))
        {
            ax = -1.0f;
        }
        if (gamePad.GetKeyPress(Keyboard::Up))
        {
            ay = 1.0f;
        }
        else if (gamePad.GetKeyPress(Keyboard::Down))
        {
            ay = -1.0f;
        }
    }

    //�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    //�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

    //�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�(
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf((cameraRight.x * cameraRight.x) + (cameraRight.y * cameraRight.y) + (cameraRight.z * cameraRight.z));
    if (cameraRightLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLenght = sqrtf((cameraFront.x * cameraFront.x) + (cameraFront.y * cameraFront.y) + (cameraFront.z * cameraFront.z));
    if (cameraFrontLenght > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraFrontX /= cameraFrontLenght;
        cameraFrontZ /= cameraFrontLenght;
    }

    //�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    //�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    //�i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = (cameraFrontX * ay + cameraRightX * ax);
    vec.z = (cameraFrontZ * ay + cameraRightZ * ax);
    //Y�������ɂ͈ړ����Ȃ�

    moveVecX = vec.x;
    moveVecZ = vec.z;
}

void Player::UpdateVerticalVelocity(float elapsedFrame)
{
    //�d�͏���
    //velocity.y = gravity + velocity.y * elapsedFrame;
}

void Player::UpdateVerticalMove()
{
    //���������̈ړ���
    float my = velocity.y * Timer::Instance().DeltaTime();
    //slopeRate = 0.0f;

#if 0
    DirectX::XMFLOAT3 normal = { 0,1,0 };
    //������
    if (my < 0.0f)
    {
        //���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
        //���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };

        //���C�L���X�g�ɂ��n�ʂ̔���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit))
        {
            //�n�ʂɐݒu���Ă���
            position = hit.position;

            //�@���x�N�g�����擾
            normal = hit.normal;

            //�X�Η��̌v�Z
            //�����F�@���x�N�g���̂�����
            //��ӁF�@���x�N�g���̂����������������̂̑傫��
            //slopeRate = 1.0f - (hit.normal.y / (hit.normal.y + sqrtf(hit.normal.x * hit.normal.x + hit.normal.z + hit.normal.z))

            //isGround = true;
            velocity.y = 0.0f;
        }
        else
        {
            //�󒆂ɕ����Ă���
            position.y += my;
            //isGround = false;
        }
    }
    //�㏸��
    else if (my > 0.0f)
    {
        position.y += my;
        //isGround = false;
    }

    //�n�ʂ̌����ɉ����悤��XZ����]
    {
        //Y�����@���x�N�g�������Ɍ����I�C���[�p��]���Z�o����
        float angleX = atan2f(normal.z, normal.y);
        float angleZ = -atan2f(normal.x, normal.y);
    }
#endif
}

void Player::UpdateHorizontalVelocity(float elapsedFrame)
{
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

#if 1
    //XZ���ʂ̑��͂���������
    if (length > 0.0f)
    {
        //���C��
        float friction = this->friction * elapsedFrame;

        //���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;
            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        //�������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
        else
        {
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }
#endif

#if 1
    //XZ���ʂ̑��͂���������
    if (length <= maxSpeed)
    {
        //�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            //������
            float acceleration = this->acceleration * elapsedFrame;

            //�ړ��x�N�g���ɂ���������
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;
        }

        //�ő呬�x����
        length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        if (length > maxSpeed)
        {
            velocity.x = velocity.x / length * maxSpeed;
            velocity.z = velocity.z / length * maxSpeed;
        }
    }
    //�ړ��x�N�g�������Z�b�g
    moveVecX = 0;
    moveVecZ = 0;
#endif
}

void Player::UpdateHorizontalMove()
{
    //�������͗ʌv�Z
    //float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    //if (velocityLengthXZ > 0.0f)
    
    //�����ړ��l
    float mx = velocity.x * Timer::Instance().DeltaTime();
    //��ɉ��ɍs��������
    float mz = (velocity.z + speedZ * accelerationZ) * Timer::Instance().DeltaTime();

#if 0
        //���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x   ,position.y + stepOffset,position.z };
        DirectX::XMFLOAT3 end = { position.x + mx,position.y + stepOffset,position.z + mz };

        //���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit))
        {
            //�ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR P_End = DirectX::XMVectorSubtract(End, P);

            //�ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

            //���˃x�N�g����@���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Normal, DirectX::XMVectorNegate(P_End));
            //DirectX::XMVECTOR Length = DirectX::XMVector3Length(Dot);
            float dot;
            DirectX::XMStoreFloat(&dot, Dot);
            //�߂荞�܂Ȃ��悤�ɕ␳
            dot *= 1.1f;

            ////�␳�ʒu�̌v�Z
            DirectX::XMVECTOR ScalingN = DirectX::XMVectorScale(Normal, dot);

            ////�ڕW�̍��W���v�Z
            DirectX::XMVECTOR O = DirectX::XMVectorAdd(End, ScalingN);
            DirectX::XMFLOAT3 o;
            DirectX::XMStoreFloat3(&o, O);

            //���C�̒��n�_P����ڕW�܂ł̃x�N�g�����v�Z
            /*DirectX::XMVECTOR R = DirectX::XMVectorAdd(P_End, ScalingN);
            DirectX::XMFLOAT3 r;
            DirectX::XMStoreFloat3(&r, R);*/
            DirectX::XMVECTOR R = DirectX::XMVectorSubtract(O, Start);
            DirectX::XMFLOAT3 r;
            DirectX::XMStoreFloat3(&r, R);

            //�ړ���̈ʒu���ǂɖ�����Ă��Ȃ����`�F�b�N
            HitResult secondHit;
            //if (Stage::Instance().RayCast(hit.position,o, secondHit))
            if (StageManager::Instance().RayCast(start, o, secondHit))
            {
                //���C���~�܂����ʒu�Œ�~
                position.x = secondHit.position.x;
                position.z = secondHit.position.z;
            }
            else
            {
                //�ڕW�֍��W���ړ�
                position.x = o.x;
                position.z = o.z;
                //velocity.z = r.z;
            }
        }
        else
        {
            //�ړ�����
            position.x += mx;
            position.z += mz;
        }
    }
#else
        //�ړ�����
        position.x += mx;
        position.z += mz;
#endif
}
