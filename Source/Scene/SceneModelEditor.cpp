#include <tchar.h>
#include <DirectXMath.h>
#include <filesystem>
#include "SceneModelEditor.h"
#include "SceneManager.h"
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Graphics/Texture.h"

void SceneModelEditor::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);

	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20), // �J�������W
		DirectX::XMFLOAT3(0, 0, 0),  // �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)   // ������x�N�g��
	);
	Camera::Instance().SetAngle(
		{DirectX::XMConvertToRadians(-45), DirectX::XMConvertToRadians(45), 0}
	);

	Camera::Instance().cameraType = Camera::CAMERA::MODEL_EDITOR;

	// ���C�g�����ݒ�
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(-1, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({1.0f, 1.0f, 1.0f, 1.0f});


	// --- �V�[�N�G���X�ϐ������ݒ� ---
	mySequence.mFrameMin = 0;
	mySequence.mFrameMax = 60;
}

void SceneModelEditor::Finalize()
{
	LightManager::Instance().Clear();
}

void SceneModelEditor::Update()
{
	// --- inputManager���� ---
	if (isMouseHoverSceneView)
		InputManager::Instance().Update();

	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();

	// --- effectManager���� ---
	EffectManager::Instance().Update();

	// --- �J�������� ---
	Camera::Instance().Update();

	if (modelObject) modelObject->Update();
}

void SceneModelEditor::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// �J�����̒萔�o�b�t�@�̍X�V
	Camera::Instance().UpdateCameraConstant();

	// ���C�g�̒萔�o�b�t�@�̍X�V
	LightManager::Instance().UpdateConstants();


	// --- imGuiFrameBuffer �ɏ������� ---
	imGuiFrameBuffer->Clear(gfx->GetBgColor());
	imGuiFrameBuffer->Activate();

	// �x�[�X�̃��C���`��(��ɏ����Ă���)
	DrawGrid(10, 1.0f);
	LineRenderer::Instance().Render();

	DebugPrimitive::Instance().Render();


	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	if (modelObject)
	{
		modelObject->Render();
	}

	imGuiFrameBuffer->DeActivate();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	DebugTimeLine();

	LightManager::Instance().DrawDebugGui();

	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
}

// �f�o�b�O�`��
void SceneModelEditor::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();

	// ���f���G�f�B�^�[�p���j���[�o�[�`��
	DrawModelEditorMenuBar();

	ImGui::Begin("Inspector");
	if (modelObject)
	{
		if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_None))
		{
			DirectX::XMFLOAT3 pos   = modelObject->GetPos();
			DirectX::XMFLOAT3 angle = modelObject->GetAngle();
			float             scale = modelObject->GetScaleX();
			ImGui::DragFloat3("POSITION", &pos.x, 0.1f);
			ImGui::DragFloat3("ANGLE", &angle.x);
			ImGui::DragFloat("SCALE", &scale, 0.01f);
			modelObject->SetPos(pos);
			modelObject->SetAngle(angle);
			modelObject->SetScale({scale, scale, scale});
		}

		if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_None))
		{
			ImGui::InputText("Path", &modelObject->GetModel()->GetModelResource()->GetFilePath()[0],
			                 modelObject->GetModel()->GetModelResource()->GetFilePath().size() + 1);

			ImGui::Separator();

			//===== coordinateSystemTransform =====
			const char* coordinateSystemTransformName[] = {"RHS Y - UP", "LHS Y - UP", "RHS Z - UP", "LHS Z - UP"};
			if (ImGui::BeginCombo(
				"Axis",
				coordinateSystemTransformName[modelObject->GetModel()->GetModelResource()->
				                                           GetCoordinateSystemTransformType()]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(coordinateSystemTransformName); i++)
				{
					const bool isSelected = (modelObject->GetModel()->GetModelResource()->
					                                      GetCoordinateSystemTransformType() == i);
					if (ImGui::Selectable(coordinateSystemTransformName[i], isSelected))
					{
						modelObject->GetModel()->GetModelResource()->SetCoordinateSystemTransformType(
							static_cast<CoordinateSystemTransform>(i));
						modelObject->GetModel()->GetModelResource()->SetCoordinateSystemTransform(
							coordinateSystemTransform[i]);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// scale
			{
				float s = modelObject->GetModel()->GetModelResource()->GetScale();
				ImGui::DragFloat("Scale", &s, 0.001f);
				modelObject->GetModel()->GetModelResource()->SetScale(s);
			}

#pragma region Mesh
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None))
			{
				for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
				{
					ImGui::Indent(); // �q�w�b�_�[�������E�ɂ��炷
					if (ImGui::TreeNode(mesh.meshName.c_str()))
					{
						ImGui::InputText("id", &std::to_string(mesh.uniqueId)[0],
						                 std::to_string(mesh.uniqueId).size() + 1);
						ImGui::TreePop();
					}
					ImGui::Unindent(); // �C���f���g�����ɖ߂�
				}
			}
#pragma endregion

#pragma region Material
			if (!modelObject->GetModel()->GetModelResource()->GetMaterials().empty())
			{
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_None))
				{
					for (auto& [name, material] : modelObject->GetModel()->GetModelResource()->GetMaterials())
					{
						ImGui::Indent(); // �q�w�b�_�[�������E�ɂ��炷

						if (ImGui::TreeNode(material.name.c_str()))
						{
							ImGui::InputText("vertexShader", &material.vertexShaderName[0],
							                 material.vertexShaderName.size() + 1);
							ImGui::InputText("pixelShader", &material.pixelShaderName[0],
							                 material.vertexShaderName.size() + 1);

							const char* textureLabelNames[4] = {"Diffuse", "Normal", "Specular", "Emissive"};
							for (int textureIndex = 0; textureIndex < 4; textureIndex++)
							{
								ImGui::InputText(textureLabelNames[textureIndex],
								                 &material.textureFilenames[textureIndex][0],
								                 material.textureFilenames[_deffuseTexture].size() + 1);
								ImGui::Image(material.shaderResourceViews[textureIndex].Get(), {64, 64});
								ImGui::SameLine();
								std::string buttonLabel = textureLabelNames[textureIndex] +
									std::to_string(textureIndex);
								if (ImGui::Button(buttonLabel.c_str()))
								{
									OPENFILENAME ofn;               // �t�@�C���I��p�̍\����
									TCHAR        szFile[260] = {0}; // �t�@�C���p�X���i�[����o�b�t�@

									// �\���̂̏�����
									ZeroMemory(&ofn, sizeof(ofn));
									ofn.lpstrFilter  = _TEXT("png�t�@�C��(*.png)\0*.png\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
									ofn.lStructSize  = sizeof(ofn);
									ofn.lpstrFile    = szFile;
									ofn.lpstrFile[0] = '\0';
									ofn.nMaxFile     = sizeof(szFile);
									ofn.nFilterIndex = 1;
									ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

									if (GetOpenFileName(&ofn))
									{
										// --- filesystem �ɂ��path�ϊ� ---

										std::filesystem::path selectedPath(szFile);
										// ���݂̃f�B���N�g���̃p�X���擾
										std::filesystem::path currentPath = std::filesystem::current_path();


										// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
										std::filesystem::path relativePath = selectedPath.lexically_relative(
											currentPath);


										// �J���Ă郂�f���̃p�X���擾
										std::filesystem::path modelPath(
											modelObject->GetModel()->GetModelResource()->GetFilePath());
										std::filesystem::path modelParebtPath = modelPath.parent_path();

										relativePath         = relativePath.lexically_relative(modelParebtPath);
										std::string filePath = relativePath.string();

										// �e�N�X�`�������ւ�
										material.textureFilenames[textureIndex] = filePath;

										for (auto& [name, material] : modelObject->GetModel()->GetModelResource()->
										     GetMaterials()) // �\��������
										{
											for (int textureIndex = 0; textureIndex < 4; textureIndex++)
											{
												if (material.textureFilenames[textureIndex].size() > 0)
												{
													std::filesystem::path path(
														modelObject->GetModel()->GetModelResource()->GetFilePath());
													path.replace_filename(material.textureFilenames[textureIndex]);
													D3D11_TEXTURE2D_DESC texture2dDesc{};
													LoadTextureFromFile(
														path.c_str(),
														material.shaderResourceViews[textureIndex].GetAddressOf(),
														&texture2dDesc);
												}
												else
												{
													DWORD color = 0xFFFFFFFF;
													// normal
													if (textureIndex == 1) color = 0xFFFF7F7F;
													// emissive
													if (textureIndex == 3) color = 0x00000000;
													LoadFbx::Instance().MakeDummyTexture(
														material.shaderResourceViews[textureIndex].GetAddressOf(),
														color, 16);
												}
											}
										}
									}
								}
							}

							ImGui::TreePop();
						}

						ImGui::Unindent(); // �C���f���g�����ɖ߂�
					}
				}
			}
#pragma endregion

#pragma region Skeleton
			bool isShowSkeletonData = false;
			for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
			{
				if (!mesh.skeleton.bones.empty()) isShowSkeletonData = true;
			}
			if (isShowSkeletonData)
			{
				if (ImGui::CollapsingHeader("Skeleton", ImGuiTreeNodeFlags_None))
				{
					for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
					{
						ImGui::Indent(); // �q�w�b�_�[�������E�ɂ��炷
						std::string meshName = mesh.meshName + " (mesh)";
						if (ImGui::TreeNode(meshName.c_str()))
						{
							if (ImGui::BeginPopupContextItem())
							{
								if (ImGui::MenuItem(("copy##" + meshName).c_str()))
								{
									// meshName ���N���b�v�{�[�h�ɃR�s�[
									ImGui::SetClipboardText(meshName.c_str());
								}
								ImGui::EndPopup();
							}

							// �ċA�I��Bone��`�悷�郉���_��
							DrawBoneDebug(mesh.skeleton.bones, 0);
							ImGui::TreePop();
						}
						ImGui::Unindent(); // �C���f���g�����ɖ߂�
					}
				}
			}
#pragma endregion

#pragma region Animation
			if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
			{
				if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_None))
				{
					int animCount = modelObject->GetModel()->GetModelResource()->GetAnimationClips().size();
					for (int animIndex = 0; animIndex < animCount; animIndex++)
					{
						ModelResource::Animation& animationClips = modelObject->GetModel()->GetModelResource()->
							GetAnimationClips().at(animIndex);
						int animationClipIndex = modelObject->GetCurrentAnimationIndex();
						if (ImGui::RadioButton((animationClips.name).c_str(), &animationClipIndex, animIndex))
						{
							modelObject->SetCurrentAnimationIndex(animationClipIndex);

							// �L�[�t���[���̍ő�l�ݒ�
							mySequence.mFrameMax = modelObject->GetModel()->GetModelResource()->GetAnimationClips().
							                                    at(animIndex).sequence.size() - 1;

							// �L�[�t���[����擪��
							modelObject->SetCurrentKeyFrame(0);

							selectedEntry = -1;
							// sequence���̃A�C�e�����N���A
							mySequence.myItems.clear();
							// ��ԏ�̃A�C�e����I����Ԃ�
							mySequence.selectItemNum = 0;

							// �A�C�e����ǉ�
							int sphereCount = animationClips.animSphereCollisions.size();
							for (auto sphere : animationClips.animSphereCollisions)
							{
								mySequence.Add(sphere.name.c_str(), static_cast<int>(SequencerItemType::Sphere),
								               sphere.startFrame, sphere.endFrame);
							}
							int seCount = animationClips.animSes.size();
							for (auto se : animationClips.animSes)
							{
								mySequence.Add(se.name.c_str(), static_cast<int>(SequencerItemType::SE), se.startFrame,
								               se.endFrame);
							}

							// �����A�C�e�����Ȃ����
							if (mySequence.myItems.size() == 0)
							{
								mySequence.Add("", 0, -10, -10);
							}
						}
					}
				}
			}
#pragma endregion
		}
	}
	ImGui::End();

	ImGui::Begin("AnimationEdit");
	{
		if (modelObject && !modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
		{
			int animationClipIndex = modelObject->GetCurrentAnimationIndex();

			ModelResource::Animation& animationClip = modelObject->GetModel()->GetModelResource()->GetAnimationClips().
			                                                       at(animationClipIndex);

			if (ImGui::Button("DoubleAnimationPlay"))
			{
				modelObject->SetDoubleCurrentAnimationIndex(1);
				modelObject->SetIsDoubleAnimation(true);
			}

			ImGui::PushID(0);
			ImGuiManager::Instance().InputText("name", animationClip.name);
			ImGui::PopID();
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				// �e�E�B���h�E�̃n���h���iNULL�̏ꍇ�A���b�Z�[�W�{�b�N�X�̓I�[�i�[���X�ƂȂ�j
				HWND hwnd = NULL;
				// ���b�Z�[�W�{�b�N�X�̃X�^�C��
				UINT boxStyle = MB_YESNO | MB_ICONWARNING;

				// ���b�Z�[�W�{�b�N�X��\�����A���[�U�[�̉������擾
				int result = MessageBox(hwnd, L"�{���ɍ폜���܂����H", L"�x��", boxStyle);

				// ���[�U�[�̉����Ɋ�Â��ď������s��
				if (result == IDYES)
				{
					// �폜
					std::filesystem::path path(modelObject->GetModel()->GetModelResource()->GetFilePath());
					std::string           parentPath     = path.parent_path().string();
					std::string           deleteFilename = parentPath + "/Anims/" + animationClip.name + ".anim";
					std::filesystem::remove(deleteFilename);

					modelObject->GetModel()->GetModelResource()->GetAnimationClips().erase(
						modelObject->GetModel()->GetModelResource()->GetAnimationClips().begin() + animationClipIndex);

					// �폜��
					modelObject->SetCurrentAnimationIndex(0);
					modelObject->SetCurrentKeyFrame(0);
					modelObject->SetCurrentAnimationSeconds(0.0f);
				}
			}
			ImGui::Text(("secondsLength : " + std::to_string(animationClip.secondsLength)).c_str());

			ImGui::Separator();
			ImGui::Text("new tileLine item");

			static std::string newSequenceName = "";
			ImGui::PushID(1);
			ImGuiManager::Instance().InputText("name", newSequenceName);
			ImGui::PopID();

			ImGui::PushItemWidth(150);
			static int selectSequencerItemTypeName;
			if (ImGui::BeginCombo("type", SequencerItemTypeNames[selectSequencerItemTypeName]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(SequencerItemTypeNames); i++)
				{
					const bool isSelected = (selectSequencerItemTypeName == i);
					if (ImGui::Selectable(SequencerItemTypeNames[i], isSelected))
					{
						selectSequencerItemTypeName = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (ImGui::Button("create"))
			{
				// emptye ��I�𒆂Ȃ���s���Ȃ�
				if (selectSequencerItemTypeName != 0)
				{
					// emptyItem �̍폜
					if (mySequence.myItems.size() == 1)
					{
						if (mySequence.myItems.at(0).mType == static_cast<int>(SequencerItemType::EMPTY))
						{
							mySequence.myItems.clear();
						}
					}

					// sphere
					if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::Sphere))
					{
						AnimSphereCollision collision;
						collision.name       = newSequenceName;
						collision.startFrame = 0;
						collision.endFrame   = 10;
						collision.radius     = 1.0f;
						collision.position   = {0, 0, 0};
						collision.color      = {1, 1, 1, 1};
						animationClip.animSphereCollisions.push_back(collision);

						mySequence.Add(newSequenceName, selectSequencerItemTypeName, 0, 10);
					}
					// se
					if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::SE))
					{
						AnimSe animSE;
						animSE.name       = newSequenceName;
						animSE.startFrame = 0;
						animSE.endFrame   = 10;
						animationClip.animSes.push_back(animSE);

						mySequence.Add(newSequenceName, selectSequencerItemTypeName, 0, 10);
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("select tileLine item");

			if (mySequence.selectItemNum != -1)
			{
				bool isSkip = false; // �폜��̏����X�L�b�v�p

				// ����
				if (ImGui::Button("Deplicate"))
				{
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						AnimSphereCollision collision = animationClip.animSphereCollisions.at(index);
						animationClip.animSphereCollisions.push_back(collision);

						mySequence.Add(collision.name, static_cast<int>(SequencerItemType::Sphere),
						               collision.startFrame, collision.endFrame);
					}
				}

				// �폜
				ImGui::SameLine();
				if (ImGui::Button("Delete"))
				{
					isSkip = true;
					// ���f���������Ă�A�C�e�����폜
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						animationClip.animSphereCollisions.erase(animationClip.animSphereCollisions.begin() + index);
					}

					// mySequence ���̃A�C�e�����폜
					mySequence.myItems.erase(mySequence.myItems.begin() + mySequence.selectItemNum);

					// �폜������ɑI�𒆂̃A�C�e�����O��
					mySequence.selectItemNum = 0;
					if (mySequence.myItems.size() == 0)
					{
						mySequence.Add("", 0, -10, -10);
					}

					selectedEntry = -1;
				}

				std::string& selectName       = mySequence.myItems.at(mySequence.selectItemNum).name;
				int&         selectStartFrame = mySequence.myItems.at(mySequence.selectItemNum).mFrameStart;
				int&         selectEndFrame   = mySequence.myItems.at(mySequence.selectItemNum).mFrameEnd;

				// �폜���������Ă��Ȃ���Ε\��
				if (!isSkip)
				{
					ImGuiManager::Instance().InputText("name", selectName);
					ImGui::DragInt("start frame", &selectStartFrame);
					ImGui::DragInt("end frame", &selectEndFrame);

					// --- sphere ---
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						// �l�̃Z�b�g
						animationClip.animSphereCollisions.at(index).name       = selectName.c_str();
						animationClip.animSphereCollisions.at(index).startFrame = selectStartFrame;
						animationClip.animSphereCollisions.at(index).endFrame   = selectEndFrame;

						float&             radius       = animationClip.animSphereCollisions.at(index).radius;
						DirectX::XMFLOAT3& position     = animationClip.animSphereCollisions.at(index).position;
						DirectX::XMFLOAT4& color        = animationClip.animSphereCollisions.at(index).color;
						std::string        bindBoneName = animationClip.animSphereCollisions.at(index).bindBoneName;

						ImGui::DragFloat("radius", &radius, 0.01f);
						ImGui::DragFloat3("position", &position.x, 0.01f);
						ImGui::ColorEdit4("color", &color.x);
						ImGuiManager::Instance().InputText("bindBoneName", bindBoneName);

						animationClip.animSphereCollisions.at(index).bindBoneName = bindBoneName.c_str();
					}
					// --- se ---
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::SE))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						// �l�̃Z�b�g
						animationClip.animSes.at(index).name       = selectName.c_str();
						animationClip.animSes.at(index).startFrame = selectStartFrame;
						animationClip.animSes.at(index).endFrame   = selectEndFrame;

						MUSIC_LABEL& musicType = animationClip.animSes.at(index).musicType;

						std::string musicTypeName[] = {"TEST_MUISC", "WEAPON"};

						if (ImGui::BeginCombo("SE TYPE", musicTypeName[static_cast<int>(musicType)].c_str()))
						{
							for (int i = 0; i < IM_ARRAYSIZE(musicTypeName); i++)
							{
								const bool isSelected = (static_cast<int>(musicType) == i);
								if (ImGui::Selectable(musicTypeName[i].c_str(), isSelected))
								{
									animationClip.animSes.at(index).musicType = static_cast<MUSIC_LABEL>(i);
								}
								if (isSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
				}
			}
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// �{�[��
	//----------------------------------------------------------
	ImGui::Begin("Skeleton Collision");
	{
		if (modelObject && !modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
		{
			std::vector<SkeletonSphereCollision>& skeletonSphereCollisions = modelObject->GetModel()->GetModelResource()
				->GetSkeletonSphereCollisions();

			static SkeletonSphereCollision::SkeletonType boneType   = SkeletonSphereCollision::SkeletonType::Normal;
			static std::string                           boneName   = "";
			static float                                 boneRadius = 1.0f;
			static DirectX::XMFLOAT3                     bonePos    = {0, 0, 0};

			const char* skeletonTypeName[] = {
				"Normal", "WeakPoint1", "WeakPoint2", "WeakPoint3", "HardenedPoint1", "HardenedPoint2", "HardenedPoint3"
			};

			if (ImGui::BeginCombo(
				"SkeletonType",
				skeletonTypeName[static_cast<int>(boneType)]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(skeletonTypeName); i++)
				{
					const bool isSelected = (static_cast<int>(boneType) == i);
					if (ImGui::Selectable(skeletonTypeName[i], isSelected))
					{
						boneType = static_cast<SkeletonSphereCollision::SkeletonType>(i);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGuiManager::Instance().InputText("name", boneName);
			ImGui::DragFloat("radius", &boneRadius);
			ImGui::InputFloat3("position", &bonePos.x);


			if (ImGui::Button("Add SkeletonCollision"))
			{
				SkeletonSphereCollision skeletonSphere;
				skeletonSphere.skeletonType = boneType;
				skeletonSphere.name         = boneName.c_str();
				skeletonSphere.radius       = boneRadius;
				skeletonSphere.position     = bonePos;
				skeletonSphereCollisions.push_back(skeletonSphere);
			}

			ImGui::Separator();
			ImGui::Separator();


			std::vector<int> deleteBonesIndex;
			int              boneSphereCount = skeletonSphereCollisions.size();
			for (int boneSphereIndex = 0; boneSphereIndex < boneSphereCount; boneSphereIndex++)
			{
				ImGui::Separator();
				//if (ImGui::CollapsingHeader(skeletonSphereCollisions.at(boneSphereIndex).name.c_str()))
				{
					SkeletonSphereCollision& seletonSphere = skeletonSphereCollisions.at(boneSphereIndex);

					if (ImGui::BeginCombo(
						"SkeletonType##" + boneSphereIndex,
						skeletonTypeName[static_cast<int>(seletonSphere.skeletonType)]))
					{
						for (int i = 0; i < IM_ARRAYSIZE(skeletonTypeName); i++)
						{
							const bool isSelected = (static_cast<int>(seletonSphere.skeletonType) == i);
							if (ImGui::Selectable(skeletonTypeName[i], isSelected))
							{
								seletonSphere.skeletonType = static_cast<SkeletonSphereCollision::SkeletonType>(i);
							}
							if (isSelected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGuiManager::Instance().InputText("name##" + boneSphereIndex, seletonSphere.name);
					ImGui::DragFloat(("radius##" + std::to_string(boneSphereIndex)).c_str(), &seletonSphere.radius,
					                 0.01f);
					ImGui::InputFloat3("position", &seletonSphere.position.x);

					ImGui::SameLine();
					if (ImGui::Button(("delete##" + std::to_string(boneSphereIndex)).c_str()))
					{
						deleteBonesIndex.push_back(boneSphereIndex);
					}
				}

				float radius = skeletonSphereCollisions.at(boneSphereIndex).radius;
			}
			// �v�f�̍폜
			for (auto index : deleteBonesIndex)
			{
				skeletonSphereCollisions.erase(skeletonSphereCollisions.begin() + index);
			}
		}
	}
	ImGui::End();


	ImGui::Begin("Scene");
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		isMouseHoverSceneView = ImGui::IsMouseHoveringRect(vMin, vMax);

		// ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 0, 0, 255));

		// ImGui�̕`��\�̈�̎擾
		ImVec2            WindowSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		DirectX::XMFLOAT3 imWindowSize = {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 0};
		DirectX::XMFLOAT3 windowSize   = {
			static_cast<float>(Framework::Instance().GetScreenWidth()),
			static_cast<float>(Framework::Instance().GetScreenHeight()), 0
		};

		DirectX::XMFLOAT3 aspectWindow   = imWindowSize / windowSize;
		DirectX::XMFLOAT3 drawWindowSize = {0, 0, 0};
		DirectX::XMFLOAT3 drawPos        = {0, 0, 0};

		if (aspectWindow.x > aspectWindow.y)
		{
			drawWindowSize.x = imWindowSize.y / windowSize.y * windowSize.x;
			drawWindowSize.y = imWindowSize.y;
			drawPos.x        = (imWindowSize.x - drawWindowSize.x) / 2;
		}
		else
		{
			drawWindowSize.x = imWindowSize.x;
			drawWindowSize.y = imWindowSize.x / windowSize.x * windowSize.y;
			drawPos.y        = (imWindowSize.y - drawWindowSize.y) / 2;
		}

		ImGui::SetCursorPos({
			ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowContentRegionMin().y + drawPos.y
		});
		ImGui::Image(imGuiFrameBuffer->shaderResourceViews[0].Get(), {drawWindowSize.x, drawWindowSize.y}, {0, 0},
		             {1, 1}, {1, 1, 1, 1});
	}
	ImGui::End();


	// --- Console�`�� ---
	ImGuiManager::Instance().Console();
}

// ���f���G�f�B�^�[�p���j���[�o�[�`��
void SceneModelEditor::DrawModelEditorMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("ModelEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneModelEditor);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load"))
		{
			if (ImGui::MenuItem(".fbx"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;               // �t�@�C���I��p�̍\����
				TCHAR        szFile[260] = {0}; // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("fbx�t�@�C��(*.fbx)\0*.fbx\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           fbxPath      = relativePath.string();

					// ���f���Ǎ�
					modelObject = std::make_unique<ModelEditorObject>(fbxPath.c_str());

					if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
					{
						// �L�[�t���[���̍ő�l�ݒ�
						mySequence.mFrameMax = modelObject->GetModel()->GetModelResource()->GetAnimationClips().at(0).
						                                    sequence.size() - 1;
					}
				}

				Timer::Instance().Start();
			}
			if (ImGui::MenuItem(".model"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;               // �t�@�C���I��p�̍\����
				TCHAR        szFile[260] = {0}; // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("model�t�@�C��(*.model)\0*.model\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           modelPath    = relativePath.string();

					// ���f���Ǎ�
					modelObject = std::make_unique<ModelEditorObject>(modelPath.c_str());
					if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
					{
						// �L�[�t���[���̍ő�l�ݒ�
						mySequence.mFrameMax = modelObject->GetModel()->GetModelResource()->GetAnimationClips().at(0).
						                                    sequence.size() - 1;
					}
				}
				Timer::Instance().Start();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem(".fbx"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;               // �t�@�C���I��p�̍\����
				TCHAR        szFile[260] = {0}; // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("fbx�t�@�C��(*.fbx)\0*.fbx\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           fbxPath      = relativePath.string();

					// �A�j���[�V�����ǉ�
					LoadFbx::Instance().AppendAnimation(fbxPath.c_str(), modelObject->GetModel()->GetModelResource(),
					                                    30);
				}
				Timer::Instance().Start();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem(".model"))
			{
				// ������ .model �����o��
				modelObject->GetModel()->GetModelResource()->OutputModelData();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

// �O���b�h�`��
void SceneModelEditor::DrawGrid(int subdivisions, float scale)
{
	int numLines    = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step   = 1.0f / static_cast<float>(subdivisions);

	int   index = 0;
	float s     = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

	LineRenderer& lineRenderer = LineRenderer::Instance();
	// Create vertical lines
	float             scaling = static_cast<float>(subdivisions) * scale;
	DirectX::XMMATRIX M       = DirectX::XMMatrixScaling(scaling, scaling, scaling);
	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 position;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(s, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(s, 0, -corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// Create horizontal lines
	s = -corner;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(-corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// X��
	{
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
		V                           = DirectX::XMVectorSet(0, 0, 0, 0);
		P                           = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);

		V = DirectX::XMVectorSet(corner, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);
	}

	// Y��
	{
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		V                             = DirectX::XMVectorSet(0, 0, 0, 0);
		P                             = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);

		V = DirectX::XMVectorSet(0, corner, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);
	}

	// Z��
	{
		const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
		V                            = DirectX::XMVectorSet(0, 0, 0, 0);
		P                            = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);

		V = DirectX::XMVectorSet(0, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);
	}
}

void SceneModelEditor::DrawBoneDebug(const std::vector<ModelResource::Bone>& bones, int boneIndex)
{
	// boneIndex���L���łȂ��ꍇ�͏������I������
	if (boneIndex < 0 || boneIndex >= bones.size())
		return;

	// ���݂̃{�[����`��
	const ModelResource::Bone& bone = bones[boneIndex];


	if (ImGui::TreeNode(bone.name.c_str()))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem(("copy##" + bone.name).c_str()))
			{
				// meshName ���N���b�v�{�[�h�ɃR�s�[
				ImGui::SetClipboardText(bone.name.c_str());
			}
			ImGui::EndPopup();
		}

		// �q�{�[��������ꍇ�A�ċA�I�ɕ`�悷��
		for (int i = 0; i < bones.size(); ++i)
		{
			if (bones[i].parentIndex == boneIndex)
			{
				DrawBoneDebug(bones, i);
			}
		}
		ImGui::TreePop();
	}
}

// �^�C�����C������
void SceneModelEditor::DebugTimeLine()
{
	//----------------------------------------------------------
	// �^�C�����C��
	//----------------------------------------------------------
	{
		// --- window�t���O�ݒ� ---
		static ImGuiWindowFlags windowFlags =
			//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
			ImGuiDockNodeFlags_PassthruCentralNode |
			//ImGuiWindowFlags_NoDocking |
			//ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		// --- dockNode�t���O�ݒ� ---
		const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		// --- ImGui�̕\���ݒ� ---
		ImGui::SetNextWindowBgAlpha(0.0f); // �w�i�A���t�@�̐ݒ�

		// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// --- DockSpace�̍쐬 ---
		ImGui::Begin("TimeLine", NULL, windowFlags);
		ImGui::PopStyleVar(3); // ���̓X�^�C���̓K�p?

		ImGuiID dockSpaceID = ImGui::GetID("DockTimeline");
		ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

		ImGui::End();
	}

	//----------------------------------------------------------
	// �A�j���[�V�����ݒ�
	//----------------------------------------------------------
	ImGui::Begin("Animation Setting");
	{
		if (modelObject)
		{
			if (ImGui::Button("Play"))
			{
				modelObject->SetIsPlayAnimation(true);
				if (modelObject->GetCurrentKeyFrame() >= mySequence.mFrameMax)
				{
					modelObject->SetAnimationEndFlag(false);
					modelObject->SetCurrentKeyFrame(0);
					modelObject->SetCurrentAnimationSeconds(0);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				modelObject->SetIsPlayAnimation(false);
			}
			ImGui::SameLine();

			// ���[�v�ݒ�
			bool isLoop = modelObject->GetAnimationLoopFlag();
			if (ImGui::Checkbox("Loop", &isLoop))
			{
				modelObject->SetAnimationLoopFlag(isLoop);
			}


			ImGui::PushItemWidth(130);
			//ImGui::DragFloat("Time Scale", &timeScale, 0.01f, 0.0f, 3.0f);
			//
			int currentFrame = modelObject->GetCurrentKeyFrame();
			if (ImGui::InputInt("Frame", &currentFrame))
			{
				modelObject->SetCurrentKeyFrame(currentFrame);
			}

			ImGui::InputInt("StartFrame", &mySequence.mFrameMin);
			ImGui::InputInt("EndFrame", &mySequence.mFrameMax);
			ImGui::PopItemWidth();
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// �A�j���[�V�����^�C�����C��
	//----------------------------------------------------------
	ImGui::Begin("AnimationTimeLine");
	{
		if (modelObject && !modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
		{
			int currentFrame = modelObject->GetCurrentKeyFrame();
			Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame,
			          ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL |
			          ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);

			modelObject->SetCurrentKeyFrame(currentFrame);
			ModelResource::KeyFrame anim = modelObject->GetModel()->GetModelResource()->GetAnimationClips().
			                                            at(modelObject->GetCurrentAnimationIndex()).sequence.at(
				                                            modelObject->GetCurrentKeyFrame());
			modelObject->SetKeyFrame(anim);

			// add a UI to edit that particular item
			if (selectedEntry != -1)
			{
				const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];
				ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
				// switch (type) ....
			}
		}
	}
	ImGui::End();

	if (mySequence.myItems.size() == 0)
	{
		mySequence.Add("", 0, -10, -10);
	}
}
