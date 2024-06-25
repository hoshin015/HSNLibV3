#include "Collision.h"

// ���Ƌ��̔���
bool Collision::CollisionSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB)
{
    // A �� B�̃x�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    // �擾�����x�N�g���̒����̂Q��擾
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //  ��������
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }
    return true;
}

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA, const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionB)
{
    // A �� B�̃x�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    // �擾�����x�N�g���̒����̂Q��擾
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //  ��������
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }

    // A��B�������o��
    Vec = DirectX::XMVector3Normalize(Vec);     // ���K������
    Vec = DirectX::XMVectorScale(Vec, range);   // �X�P�[�����O
    PositionB = DirectX::XMVectorAdd(PositionA, Vec);
    DirectX::XMStoreFloat3(&outPositionB, PositionB);

    return true;
}

// ���Ƌ��̔�������
bool Collision::RepulsionSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA, float weightA, const DirectX::XMFLOAT3& positionB, float radiusB, float weightB, DirectX::XMFLOAT3& outPositionA, DirectX::XMFLOAT3& outPositionB)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR AtoB = DirectX::XMVectorSubtract(posB, posA);
    DirectX::XMVECTOR BtoA = DirectX::XMVectorSubtract(posA, posB);
    DirectX::XMVECTOR Length = DirectX::XMVector3Length(AtoB);
    float length;
    DirectX::XMStoreFloat(&length, Length);

    float totalRadius = radiusA + radiusB;
    if (totalRadius * totalRadius > length * length)
    {
        // �߂荞�ݗʎZ�o
        float defLength = totalRadius - length;

        // �䗦���Z�o
        float totalWeight = weightA + weightB;
        float scaleA = weightA / totalWeight;
        float scaleB = weightB / totalWeight;

        // a �̈ړ�
        BtoA = DirectX::XMVector3Normalize(BtoA);				    // b����a�Ɍ������P�ʃx�N�g��
        BtoA = DirectX::XMVectorScale(BtoA, defLength * scaleB);    // �߂荞�񂾗� * b�̏d�� = a�̈ړ��x�N�g��
        BtoA = DirectX::XMVectorScale(BtoA, 60.0);                   // �X�P�[�����O
        //posA = DirectX::XMVectorAdd(posA, BtoA);				    // �ړ��x�N�g�����|�W�V�����x�N�g���ɉ��Z
        DirectX::XMStoreFloat3(&outPositionA, BtoA);			    // �K��

        // b �̈ړ�
        AtoB = DirectX::XMVector3Normalize(AtoB);
        AtoB = DirectX::XMVectorScale(AtoB, defLength * scaleA);
        AtoB = DirectX::XMVectorScale(AtoB, 60.0);
        //posB = DirectX::XMVectorAdd(posB, AtoB);
        DirectX::XMStoreFloat3(&outPositionB, AtoB);

        return true;
    }

    return false;
}

bool Collision::StaticRepulsionSphereVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA, const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionA, int power)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR AtoB = DirectX::XMVectorSubtract(posB, posA);
    DirectX::XMVECTOR BtoA = DirectX::XMVectorSubtract(posA, posB);
    DirectX::XMVECTOR Length = DirectX::XMVector3Length(AtoB);
    float length;
    DirectX::XMStoreFloat(&length, Length);

    float totalRadius = radiusA + radiusB;
    if (totalRadius * totalRadius > length * length)
    {
        // �߂荞�ݗʎZ�o
        float defLength = totalRadius - length;

        // a �̈ړ�
        BtoA = DirectX::XMVector3Normalize(BtoA);				    // b����a�Ɍ������P�ʃx�N�g��
        BtoA = DirectX::XMVectorScale(BtoA, defLength);             // �߂荞�񂾗�
        BtoA = DirectX::XMVectorScale(BtoA, 2.0 * power);           // �X�P�[�����O
        DirectX::XMStoreFloat3(&outPositionA, BtoA);			    // �K��

        return true;
    }

    return false;
}

bool Collision::SphereVsSphereCollisionPoint(const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& collisionPoint)
{
    // A �� B�̃x�N�g�����Z�o
    DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
    DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
    // �擾�����x�N�g���̒����̂Q��擾
    DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
    float lengthSq;
    DirectX::XMStoreFloat(&lengthSq, LengthSq);

    //  ��������
    float range = radiusA + radiusB;
    if (range * range < lengthSq)
    {
        return false;
    }

    // �Փ˃|�C���g�Z�o
    Vec = DirectX::XMVector3Normalize(Vec);     // ���K������
    Vec = DirectX::XMVectorScale(Vec, radiusA);   // �X�P�[�����O
    DirectX::XMVECTOR CollisiontPoint = DirectX::XMVectorAdd(PositionA, Vec);
    DirectX::XMStoreFloat3(&collisionPoint, CollisiontPoint);

    return true;
}

// �~���Ɖ~���̌�������
bool Collision::IntersectCylinderVsCylinder(const DirectX::XMFLOAT3& positionA, float radiusA, float heightA, const DirectX::XMFLOAT3& positionB, float radiusB, float heightB, DirectX::XMFLOAT3& outPositionB)
{
    // A�̑�����B�̓�����Ȃ瓖�����Ă��Ȃ�
    if (positionA.y > positionB.y + heightB) return false;

    // A�̓���B�̑�����艺�Ȃ瓖�����Ă��Ȃ�
    if (positionA.y + heightA < positionB.y) return false;

    // XZ���ʂł͈̔̓`�F�b�N
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    vx /= distXZ;
    vz /= distXZ;
    if (distXZ > range)
    {
        return false;
    }
    // A��B�������o��
    outPositionB.x = positionA.x + (vx * range);
    outPositionB.y = positionB.y;
    outPositionB.z = positionA.z + (vz * range);

    return true;
}

// ���Ɖ~���̌�������
bool Collision::IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
    // �����`�F�b�N
    if (spherePosition.y + sphereRadius < cylinderPosition.y) return false;
    if (spherePosition.y - sphereRadius > cylinderPosition.y + cylinderHeight) return false;

    // XZ���ʂł͈̔͂��`�F�b�N
    float vx = cylinderPosition.x - spherePosition.x;
    float vz = cylinderPosition.z - spherePosition.z;
    float range = sphereRadius + cylinderRadius;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range) return false;

    // �����~���������o��
    vx /= distXZ;
    vz /= distXZ;
    outCylinderPosition.x = spherePosition.x + (vx * range);
    outCylinderPosition.y = cylinderPosition.y;
    outCylinderPosition.z = spherePosition.z + (vz * range);

    return true;
}

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const ModelResource* model, HitResult& result)
{
    // ���[���h��Ԃł̃��C�x�N�g�������߂�
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    // ���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    for (const ModelResource::Mesh& mesh : model->GetMeshes())
    {
        // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
        
        // �T�C�Y�̏C��
        const float scaleFactor = model->GetScale();
        DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->GetCoordinateSystemTransform()) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);
        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform);

        // ���b�V���̃��[���h�s��Ƌt�s������߂�
        DirectX::XMMATRIX WorldTransform = M * C;
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        // ���C�x�N�g�������b�V����Ԃɕϊ�
        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // ���b�V����Ԃł̃��C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // �O�p�`�i�ʁj�Ƃ̌�������
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        std::string materialName = "";
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                // �O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index+1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index+2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // �O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                // ���ς̌��ʂ��v���X�Ȃ�Η�����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0) continue;

                // ���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S); // ���C�̊J�n�_����_�`�ւ̃x�N�g��
                DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                float x;
                DirectX::XMStoreFloat(&x, X);
                if (x < 0.0f || x > neart) continue;

                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                // ��_���O�p�`�̓����ɂ��邩����
                // �P��
                DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0.0f) continue;

                // �Q��
                DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0.0f) continue;

                // �R��
                DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0.0f) continue;

                // �ŒZ�������X�V
                neart = x;

                // ��_�Ɩ@�����X�V
                HitPosition = P;
                HitNormal = N;
                materialName = subset.materialName;
            }
        }
        if (materialName != "")
        {
            // ���b�V����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // �q�b�g���ۑ��i���C�ɓ����������ň�ԋ߂��ʂŏ������s���j
            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

                result.distance = distance;
                result.materialName = materialName;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }
        }
    }

    return hit;
}

// ���C�ƃ��f���̌�������
bool Collision::IntersectRayVsModel(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const ModelResource* model, const DirectX::XMFLOAT4X4& transform, HitResult& result)
{
    // ���[���h��Ԃł̃��C�x�N�g�������߂�
    DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
    DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

    // ���[���h��Ԃ̃��C�̒���
    DirectX::XMStoreFloat(&result.distance, WorldRayLength);

    bool hit = false;
    for (const ModelResource::Mesh& mesh : model->GetMeshes())
    {
        // ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�

        // �T�C�Y�̏C��
        DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform);

        // ���b�V���̃��[���h�s��Ƌt�s������߂�
        DirectX::XMMATRIX WorldTransform = M * T;
        DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

        // ���C�x�N�g�������b�V����Ԃɕϊ�
        DirectX::XMVECTOR S = DirectX::XMVector3TransformCoord(WorldStart, InverseWorldTransform);
        DirectX::XMVECTOR E = DirectX::XMVector3TransformCoord(WorldEnd, InverseWorldTransform);
        DirectX::XMVECTOR SE = DirectX::XMVectorSubtract(E, S);
        DirectX::XMVECTOR V = DirectX::XMVector3Normalize(SE);
        DirectX::XMVECTOR Length = DirectX::XMVector3Length(SE);

        // ���b�V����Ԃł̃��C�̒���
        float neart;
        DirectX::XMStoreFloat(&neart, Length);

        // �O�p�`�i�ʁj�Ƃ̌�������
        const std::vector<ModelResource::Vertex>& vertices = mesh.vertices;
        const std::vector<UINT> indices = mesh.indices;

        std::string materialName = "";
        DirectX::XMVECTOR HitPosition;
        DirectX::XMVECTOR HitNormal;
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.indexCount; i += 3)
            {
                UINT index = subset.startIndex + i;

                // �O�p�`�̒��_�𒊏o
                const ModelResource::Vertex& a = vertices.at(indices.at(index));
                const ModelResource::Vertex& b = vertices.at(indices.at(index + 1));
                const ModelResource::Vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

                // �O�p�`�̎O�Ӄx�N�g�����Z�o
                DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
                DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
                DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);

                // �O�p�`�̖@���x�N�g�����Z�o
                DirectX::XMVECTOR N = DirectX::XMVector3Cross(AB, BC);

                // ���ς̌��ʂ��v���X�Ȃ�Η�����
                DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(V, N);
                float dot;
                DirectX::XMStoreFloat(&dot, Dot);
                if (dot >= 0) continue;

                // ���C�ƕ��ʂ̌�_���Z�o
                DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, S); // ���C�̊J�n�_����_�`�ւ̃x�N�g��
                DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(SA, N), Dot);
                float x;
                DirectX::XMStoreFloat(&x, X);
                if (x < 0.0f || x > neart) continue;

                DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));

                // ��_���O�p�`�̓����ɂ��邩����
                // �P��
                DirectX::XMVECTOR PA = DirectX::XMVectorSubtract(A, P);
                DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(PA, AB);
                DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, N);
                float dot1;
                DirectX::XMStoreFloat(&dot1, Dot1);
                if (dot1 < 0.0f) continue;

                // �Q��
                DirectX::XMVECTOR PB = DirectX::XMVectorSubtract(B, P);
                DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(PB, BC);
                DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, N);
                float dot2;
                DirectX::XMStoreFloat(&dot2, Dot2);
                if (dot2 < 0.0f) continue;

                // �R��
                DirectX::XMVECTOR PC = DirectX::XMVectorSubtract(C, P);
                DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(PC, CA);
                DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, N);
                float dot3;
                DirectX::XMStoreFloat(&dot3, Dot3);
                if (dot3 < 0.0f) continue;

                // �ŒZ�������X�V
                neart = x;

                // ��_�Ɩ@�����X�V
                HitPosition = P;
                HitNormal = N;
                materialName = subset.materialName;
            }
        }
        if (materialName != "")
        {
            // ���b�V����Ԃ��烏�[���h��Ԃ֕ϊ�
            DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
            DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
            DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
            float distance;
            DirectX::XMStoreFloat(&distance, WorldCrossLength);

            // �q�b�g���ۑ��i���C�ɓ����������ň�ԋ߂��ʂŏ������s���j
            if (result.distance > distance)
            {
                DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);

                result.distance = distance;
                result.materialName = materialName;
                DirectX::XMStoreFloat3(&result.position, WorldPosition);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
                hit = true;
            }
        }
    }

    return hit;
}
