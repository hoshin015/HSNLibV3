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
#include "../../Library/Resource/Model/Animator.h"

void SceneModelEditor::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);

	// --- カメラ初期設定 ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20), // カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),  // ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)   // 上方向ベクトル
	);
	Camera::Instance().SetAngle(
		{DirectX::XMConvertToRadians(-45), DirectX::XMConvertToRadians(45), 0}
	);
	Camera::Instance().SetTarget({0,0,0});
	Camera::Instance().cameraType = Camera::CAMERA::MODEL_EDITOR;

	// ライト初期設定
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(-1, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({1.0f, 1.0f, 1.0f, 1.0f});


	// --- シークエンス変数初期設定 ---
	mySequence.mFrameMin = 0;
	mySequence.mFrameMax = 60;
}

void SceneModelEditor::Finalize()
{
	LightManager::Instance().Clear();
}

void SceneModelEditor::Update()
{
	// --- inputManager処理 ---
	if (isMouseHoverSceneView)
		InputManager::Instance().Update();

	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
	Camera::Instance().Update();

	if (modelObject) modelObject->Update();
}

void SceneModelEditor::Render()
{
	// 必要なポインタ取得
	Graphics*            gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc  = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// カメラの定数バッファの更新
	Camera::Instance().UpdateCameraConstant();

	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();


	// --- imGuiFrameBuffer に書きこむ ---
	imGuiFrameBuffer->Clear(gfx->GetBgColor());
	imGuiFrameBuffer->Activate();

	// ベースのライン描画(先に書いておく)
	DrawGrid(10, 1.0f);
	LineRenderer::Instance().Render();

	DebugPrimitive::Instance().Render();


	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	if (modelObject)
	{
		modelObject->Render();
	}

	imGuiFrameBuffer->DeActivate();

	// --- デバッグ描画 ---
	DrawDebugGUI();

	DebugTimeLine();

	LightManager::Instance().DrawDebugGui();

	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
}

// デバッグ描画
void SceneModelEditor::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();

	// モデルエディター用メニューバー描画
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
					ImGui::Indent(); // 子ヘッダーを少し右にずらす
					if (ImGui::TreeNode(mesh.meshName.c_str()))
					{
						ImGui::InputText("id", &std::to_string(mesh.uniqueId)[0],
						                 std::to_string(mesh.uniqueId).size() + 1);
						ImGui::TreePop();
					}
					ImGui::Unindent(); // インデントを元に戻す
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
						ImGui::Indent(); // 子ヘッダーを少し右にずらす

						if (ImGui::TreeNode(material.name.c_str()))
						{
							ImGuiManager::Instance().InputText("vertexShader", material.vertexShaderName);
							ImGuiManager::Instance().InputText("pixelShader", material.pixelShaderName);

							const char* textureLabelNames[6] = {"Diffuse", "Normal", "Specular", "Emissive", "Occlusion", "dissolve"};
							for (int textureIndex = 0; textureIndex < 6; textureIndex++)
							{
								ImGuiManager::Instance().InputText(textureLabelNames[textureIndex], material.textureFilenames[textureIndex]);
								ImGui::Image(material.shaderResourceViews[textureIndex].Get(), {64, 64});
								ImGui::SameLine();
								std::string buttonLabel = textureLabelNames[textureIndex] +
									std::to_string(textureIndex);
								if (ImGui::Button(buttonLabel.c_str()))
								{
									OPENFILENAME ofn;               // ファイル選択用の構造体
									TCHAR        szFile[260] = {0}; // ファイルパスを格納するバッファ

									// 構造体の初期化
									ZeroMemory(&ofn, sizeof(ofn));
									ofn.lpstrFilter  = _TEXT("pngファイル(*.png)\0*.png\0") _TEXT("全てのファイル(*.*)\0*.*\0");
									ofn.lStructSize  = sizeof(ofn);
									ofn.lpstrFile    = szFile;
									ofn.lpstrFile[0] = '\0';
									ofn.nMaxFile     = sizeof(szFile);
									ofn.nFilterIndex = 1;
									ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

									if (GetOpenFileName(&ofn))
									{
										// --- filesystem によるpath変換 ---

										std::filesystem::path selectedPath(szFile);
										// 現在のディレクトリのパスを取得
										std::filesystem::path currentPath = std::filesystem::current_path();


										// 選択されたファイルへの相対パスを取得
										std::filesystem::path relativePath = selectedPath.lexically_relative(
											currentPath);


										// 開いてるモデルのパスを取得
										std::filesystem::path modelPath(
											modelObject->GetModel()->GetModelResource()->GetFilePath());
										std::filesystem::path modelParebtPath = modelPath.parent_path();

										relativePath         = relativePath.lexically_relative(modelParebtPath);
										std::string filePath = relativePath.string();

										// テクスチャ差し替え
										material.textureFilenames[textureIndex] = filePath;

										for (auto& [name, material] : modelObject->GetModel()->GetModelResource()->
										     GetMaterials()) // 構造化束縛
										{
											for (int textureIndex = 0; textureIndex < 6; textureIndex++)
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
													// metallicRaughness
													if (textureIndex == 2) color = 0xFF000000;
													// emissive & dissolve
													if (textureIndex == 3 || textureIndex == 5) color = 0x00000000;
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

						ImGui::Unindent(); // インデントを元に戻す
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
						ImGui::Indent(); // 子ヘッダーを少し右にずらす
						std::string meshName = mesh.meshName + " (mesh)";
						if (ImGui::TreeNode(meshName.c_str()))
						{
							if (ImGui::BeginPopupContextItem())
							{
								if (ImGui::MenuItem(("copy##" + meshName).c_str()))
								{
									// meshName をクリップボードにコピー
									ImGui::SetClipboardText(meshName.c_str());
								}
								ImGui::EndPopup();
							}

							// 再帰的にBoneを描画するラムダ式
							DrawBoneDebug(mesh.skeleton.bones, 0);
							ImGui::TreePop();
						}
						ImGui::Unindent(); // インデントを元に戻す
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

							// キーフレームの最大値設定
							mySequence.mFrameMax = modelObject->GetModel()->GetModelResource()->GetAnimationClips().
							                                    at(animIndex).sequence.size() - 1;

							// キーフレームを先頭に
							modelObject->SetCurrentKeyFrame(0);

							selectedEntry = -1;
							// sequence内のアイテムをクリア
							mySequence.myItems.clear();
							// 一番上のアイテムを選択状態に
							mySequence.selectItemNum = 0;

							// アイテムを追加
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

							// 何もアイテムがなければ
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
				// 親ウィンドウのハンドル（NULLの場合、メッセージボックスはオーナーレスとなる）
				HWND hwnd = NULL;
				// メッセージボックスのスタイル
				UINT boxStyle = MB_YESNO | MB_ICONWARNING;

				// メッセージボックスを表示し、ユーザーの応答を取得
				int result = MessageBox(hwnd, L"本当に削除しますか？", L"警告", boxStyle);

				// ユーザーの応答に基づいて処理を行う
				if (result == IDYES)
				{
					// 削除
					std::filesystem::path path(modelObject->GetModel()->GetModelResource()->GetFilePath());
					std::string           parentPath     = path.parent_path().string();
					std::string           deleteFilename = parentPath + "/Anims/" + animationClip.name + ".anim";
					std::filesystem::remove(deleteFilename);

					modelObject->GetModel()->GetModelResource()->GetAnimationClips().erase(
						modelObject->GetModel()->GetModelResource()->GetAnimationClips().begin() + animationClipIndex);

					// 削除後
					modelObject->SetCurrentAnimationIndex(0);
					modelObject->SetCurrentKeyFrame(0);
					modelObject->SetCurrentAnimationSeconds(0.0f);
				}
			}
			ImGui::Text(("secondsLength : " + std::to_string(animationClip.secondsLength)).c_str());

			ImGui::Separator();
			ImGui::Text("new tileLine item");

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
				// emptye を選択中なら実行しない
				if (selectSequencerItemTypeName != 0)
				{
					// emptyItem の削除
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
						collision.name       = "";
						collision.startFrame = 0;
						collision.endFrame   = 10;
						collision.radius     = 1.0f;
						collision.position   = {0, 0, 0};
						collision.color      = {1, 1, 1, 1};
						animationClip.animSphereCollisions.push_back(collision);

						mySequence.Add("", selectSequencerItemTypeName, 0, 10);
					}
					// se
					if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::SE))
					{
						AnimSe animSE;
						animSE.name       = "";
						animSE.startFrame = 0;
						animSE.endFrame   = 10;
						animationClip.animSes.push_back(animSE);

						mySequence.Add("", selectSequencerItemTypeName, 0, 10);
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("select tileLine item");

			if (mySequence.selectItemNum != -1)
			{
				bool isSkip = false; // 削除後の処理スキップ用

				// 複製
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
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::SE))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						AnimSe se = animationClip.animSes.at(index);
						animationClip.animSes.push_back(se);

						mySequence.Add(se.name, static_cast<int>(SequencerItemType::SE),
							se.startFrame, se.endFrame);
					}
				}

				// 削除
				ImGui::SameLine();
				if (ImGui::Button(u8"削除"))
				{
					isSkip = true;
					// モデルが持ってるアイテムを削除
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						animationClip.animSphereCollisions.erase(animationClip.animSphereCollisions.begin() + index);
					}
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(
						SequencerItemType::SE))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						animationClip.animSes.erase(animationClip.animSes.begin() + index);
					}

					// mySequence 内のアイテムを削除
					mySequence.myItems.erase(mySequence.myItems.begin() + mySequence.selectItemNum);

					// 削除した後に選択中のアイテムを０に
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

				// 削除処理をしていなければ表示
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

						// 値のセット
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

						// 値のセット
						animationClip.animSes.at(index).name       = selectName.c_str();
						animationClip.animSes.at(index).startFrame = selectStartFrame;
						animationClip.animSes.at(index).endFrame   = selectEndFrame;

						MUSIC_LABEL& musicType = animationClip.animSes.at(index).musicType;

						// TODO: SE更新箇所
						std::string musicTypeName[] = {
							"TEST_MUISC",
							"WEAPON",
							"SELECT_MOVE",
							"BATTLE1",
							"BATTLE2",
							"Fire1",
							"MonsterRoar1",
							"Breath",
							"NovaNoise1",
							"NovaNoise2",
							"Lightning1",
							"Lightning2",
							"Lightning3",
							"Lightning4",
							"Lightning5",
							"Lightning6",
							"Lightning7",
							"Lightning8",
							"Lightning9",
							"Lightning10",
							"SLASH",
							"DODGE",
							"TITLE",
							"LOOSE",
							"WIN",
							"DAMAGE",
							"SKILL",
						};

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
	// ボーン
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
				if (ImGui::CollapsingHeader((skeletonSphereCollisions.at(boneSphereIndex).name + "##" + std::to_string(boneSphereIndex)).c_str()))
				{
					SkeletonSphereCollision& seletonSphere = skeletonSphereCollisions.at(boneSphereIndex);

					std::string t = "SkeletonType##" + std::to_string(boneSphereIndex);
					if (ImGui::BeginCombo(
						t.c_str(),
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

					ImGuiManager::Instance().InputText(("name##" + std::to_string(boneSphereIndex)).c_str(), seletonSphere.name);
					ImGui::DragFloat(("radius##" + std::to_string(boneSphereIndex)).c_str(), &seletonSphere.radius,
					                 0.01f);
					ImGui::InputFloat3("position##" + boneSphereIndex, &seletonSphere.position.x);

					ImGui::SameLine();
					if (ImGui::Button(("delete##" + std::to_string(boneSphereIndex)).c_str()))
					{
						deleteBonesIndex.push_back(boneSphereIndex);
					}
				}

				float radius = skeletonSphereCollisions.at(boneSphereIndex).radius;
			}
			// 要素の削除
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

		// ImGuiの描画可能領域の取得
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


	// --- Console描画 ---
	ImGuiManager::Instance().Console();
}

// モデルエディター用メニューバー描画
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

				OPENFILENAME ofn;               // ファイル選択用の構造体
				TCHAR        szFile[260] = {0}; // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("fbxファイル(*.fbx)\0*.fbx\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           fbxPath      = relativePath.string();

					// モデル読込
					modelObject = std::make_unique<ModelEditorObject>(fbxPath.c_str());

					if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
					{
						// キーフレームの最大値設定
						mySequence.mFrameMax = modelObject->GetModel()->GetModelResource()->GetAnimationClips().at(0).
						                                    sequence.size() - 1;
					}
				}

				Timer::Instance().Start();
			}
			if (ImGui::MenuItem(".model"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;               // ファイル選択用の構造体
				TCHAR        szFile[260] = {0}; // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("modelファイル(*.model)\0*.model\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           modelPath    = relativePath.string();

					// モデル読込
					modelObject = std::make_unique<ModelEditorObject>(modelPath.c_str());

					selectedEntry = -1;
					mySequence.myItems.clear();
					mySequence.selectItemNum = 0;

					std::vector<ModelResource::Animation>& animationClips = modelObject->GetModel()->GetModelResource()->GetAnimationClips();

					// アイテムを追加
					if (animationClips.size() > 0)
					{
						int animationClipIndex = modelObject->GetCurrentAnimationIndex();

						int sphereCount = animationClips.at(animationClipIndex).animSphereCollisions.size();
						for (auto sphere : animationClips.at(animationClipIndex).animSphereCollisions)
						{
							mySequence.Add(sphere.name.c_str(), static_cast<int>(SequencerItemType::Sphere), sphere.startFrame, sphere.endFrame);
						}
						int seCount = animationClips.at(animationClipIndex).animSes.size();
						for (auto se : animationClips.at(animationClipIndex).animSes)
						{
							mySequence.Add(se.name.c_str(), static_cast<int>(SequencerItemType::SE), se.startFrame, se.endFrame);
						}
					}

					// 何もアイテムがなければ
					if (mySequence.myItems.size() == 0)
					{
						mySequence.Add("", 0, -10, -10);
					}

					if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
					{
						// キーフレームの最大値設定
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

				OPENFILENAME ofn;               // ファイル選択用の構造体
				TCHAR        szFile[260] = {0}; // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter  = _TEXT("fbxファイル(*.fbx)\0*.fbx\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize  = sizeof(ofn);
				ofn.lpstrFile    = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile     = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string           fbxPath      = relativePath.string();

					// アニメーション追加
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
				// ここで .model 書き出し
				modelObject->GetModel()->GetModelResource()->OutputModelData();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

// グリッド描画
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

	// X軸
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

	// Y軸
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

	// Z軸
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
	// boneIndexが有効でない場合は処理を終了する
	if (boneIndex < 0 || boneIndex >= bones.size())
		return;

	// 現在のボーンを描画
	const ModelResource::Bone& bone = bones[boneIndex];


	if (ImGui::TreeNode(bone.name.c_str()))
	{
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem(("copy##" + bone.name).c_str()))
			{
				// meshName をクリップボードにコピー
				ImGui::SetClipboardText(bone.name.c_str());
			}
			ImGui::EndPopup();
		}

		// 子ボーンがある場合、再帰的に描画する
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

// タイムライン処理
void SceneModelEditor::DebugTimeLine()
{
	//----------------------------------------------------------
	// タイムライン
	//----------------------------------------------------------
	{
		// --- windowフラグ設定 ---
		static ImGuiWindowFlags windowFlags =
			//ImGuiWindowFlags_MenuBar |		// これを有効にするとメニューバーを追加できる
			ImGuiDockNodeFlags_PassthruCentralNode |
			//ImGuiWindowFlags_NoDocking |
			//ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		// --- dockNodeフラグ設定 ---
		const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		// --- ImGuiの表示設定 ---
		ImGui::SetNextWindowBgAlpha(0.0f); // 背景アルファの設定

		// --- DockSpaceの周囲スタイルの設定? ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// --- DockSpaceの作成 ---
		ImGui::Begin("TimeLine", NULL, windowFlags);
		ImGui::PopStyleVar(3); // 周囲スタイルの適用?

		ImGuiID dockSpaceID = ImGui::GetID("DockTimeline");
		ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

		ImGui::End();
	}

	//----------------------------------------------------------
	// アニメーション設定
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

			// ループ設定
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
	// アニメーションタイムライン
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
