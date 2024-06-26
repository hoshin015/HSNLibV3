#include "Animator.h"
using namespace DirectX;

void Animator::AddBlendAnimation(const std::string& name, std::initializer_list<ModelResource::Animation*> animations) {
  BlendState state;
  state.blendRate = 0;
  state.timer     = 0;

  int   count      = 0;
  int   size       = animations.size() - 1;
  float maxSeconds = 0;
  for (ModelResource::Animation* animation: animations) {
    Motion motion = {animation, count / static_cast<float>(size), 1};
    state.motions.push_back(motion);
    count++;
    if (maxSeconds < animation->secondsLength) maxSeconds = animation->samplingRate;
  }

  state.maxSeconds = maxSeconds;
  _state[name]     = state;
}

ModelResource::KeyFrame Animator::PlayAnimation(const std::string& name, float elapsedTime) {
  BlendState& state = _state[name];
  state.blendRate   = 0.52f;

  state.timer += elapsedTime;

  const Motion* first  = nullptr;
  const Motion* second = nullptr;

  for (auto&& motion: state.motions) {
    if (state.blendRate >= motion.threshold) first = &motion;
    if (!second && state.blendRate < motion.threshold) second = &motion;
  }

  const float animationRate = state.timer / state.maxSeconds;
  const float leapRate      = (state.blendRate - first->threshold) / (second->threshold - first->threshold);

  const ModelResource::KeyFrame& firstKeyFrame = first->motion->sequence.at(
    first->motion->sequence.size() * animationRate
  );
  const ModelResource::KeyFrame& secondKeyFrame = second->motion->sequence.at(
    second->motion->sequence.size() * animationRate
  );

  ModelResource::KeyFrame resultKeyFrame;
  resultKeyFrame.nodes.resize(firstKeyFrame.nodes.size());
  for (int count = 0; count < firstKeyFrame.nodes.size(); ++count) {
      ModelResource::KeyFrame::Node node;
    XMVECTOR S = XMVectorLerp(
      XMLoadFloat3(&firstKeyFrame.nodes[count].scaling), XMLoadFloat3(&secondKeyFrame.nodes[count].scaling), leapRate
    );
    XMVECTOR R = XMQuaternionSlerp(
      XMLoadFloat4(&firstKeyFrame.nodes[count].rotation), XMLoadFloat4(&secondKeyFrame.nodes[count].rotation), leapRate
    );
    XMVECTOR T = XMVectorLerp(
      XMLoadFloat3(&firstKeyFrame.nodes[count].translation), XMLoadFloat3(&secondKeyFrame.nodes[count].translation),
      leapRate
    );

    XMFLOAT3 scale, transform;
    XMFLOAT4 rotation;

    XMStoreFloat3(&scale, S);
    XMStoreFloat4(&rotation, R);
    XMStoreFloat3(&transform, T);

    node.scaling = scale;
    node.rotation = rotation;
    node.translation = transform;
    node.globalTransform = firstKeyFrame.nodes[count].globalTransform;
    node.name = firstKeyFrame.nodes[count].name;
    node.uniqueId = firstKeyFrame.nodes[count].uniqueId;
    resultKeyFrame.nodes[count] = node;
  }

  return resultKeyFrame;
}
