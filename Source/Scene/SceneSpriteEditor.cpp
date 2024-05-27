#include <tchar.h>
#include <DirectXMath.h>
#include <filesystem>
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/ImGui/ConsoleData.h"
// --- Scene ---
#include "SceneSpriteEditor.h"
#include "SceneManager.h"

void SceneSpriteEditor::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);
	imGuiFrameBuffer2 = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);
}

void SceneSpriteEditor::Finalize()
{
}

void SceneSpriteEditor::Update()
{
	// --- inputManager���� ---
	if (isMouseHoverSceneView)
		InputManager::Instance().Update();

	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
}

void SceneSpriteEditor::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// --- imGuiFrameBuffer �ɏ������� ---
	imGuiFrameBuffer->Clear(gfx->GetBgColor());
	imGuiFrameBuffer->Activate();

	if (spr1)
	{
		Framework* frameWork = &Framework::Instance();
		float aspectX = frameWork->GetScreenWidthF() / spr1->GetSpriteResource()->GetSize().x;
		float aspectY = frameWork->GetScreenHeightF() / spr1->GetSpriteResource()->GetSize().y;

		spr1->SetScaleX(aspectX);
		spr1->SetScaleY(aspectY);

		spr1->SetTexPos({ 0,0 });
		spr1->SetSize({
			static_cast<float>(spr1->GetSpriteResource()->GetSize().x),
			static_cast<float>(spr1->GetSpriteResource()->GetSize().y)
		});

		DirectX::XMFLOAT2 storePivot = spr1->GetPivotPoint();
		spr1->SetPivotPoint({ 0,0 });
		spr1->Render();
		spr1->SetPivotPoint(storePivot);
	}

	imGuiFrameBuffer->DeActivate();



	// --- imGuiFrameBuffer �ɏ������� ---
	imGuiFrameBuffer2->Clear(gfx->GetBgColor());
	imGuiFrameBuffer2->Activate();

	if (spr1 && !spr1->GetSpriteResource()->GetAnimations().empty())
	{
		Framework* frameWork = &Framework::Instance();
		float aspectX = frameWork->GetScreenWidthF() / spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameWidth;
		float aspectY = frameWork->GetScreenHeightF() / spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameHeight;

		spr1->SetScaleX(aspectX);
		spr1->SetScaleY(aspectY);

		spr1->SetSize({
			static_cast<float>(spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameWidth),
			static_cast<float>(spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameHeight)
		});
		spr1->UpdateAnimation();

		DirectX::XMFLOAT2 storePivot = spr1->GetPivotPoint();
		spr1->SetPivotPoint({ 0,0 });
		spr1->Render();
		spr1->SetPivotPoint(storePivot);
		
	}

	imGuiFrameBuffer2->DeActivate();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
}

// �f�o�b�O�`��
void SceneSpriteEditor::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();

	// ���f���G�f�B�^�[�p���j���[�o�[�`��
	DrawSpriteEditorMenuBar();

	ImGui::Begin("Inspector");
	
	if (spr1)
	{
		ImGui::InputText("Path", &spr1->GetSpriteResource()->GetFilePath()[0], spr1->GetSpriteResource()->GetFilePath().size() + 1);
		int textureSizeInt[2] = { static_cast<int>(spr1->GetSpriteResource()->GetSize().x), static_cast<int>(spr1->GetSpriteResource()->GetSize().y) };
		ImGui::InputInt2("TextureSize", &textureSizeInt[0]);

		if (ImGui::Button("Add Animation"))
		{
			SpriteResource::Animation anim;
			spr1->GetSpriteResource()->GetAnimations().emplace_back(anim);
		}
		

		
		for (int i = 0; i < spr1->GetSpriteResource()->GetAnimations().size(); i++) {
			if (ImGui::RadioButton(spr1->GetSpriteResource()->GetAnimations().at(i).name.c_str(), selected == i)) {
				selected = i;
			}
		}

		for (int i = 0; i < spr1->GetSpriteResource()->GetAnimations().size(); i++)
		{
			if (i == selected)
			{
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
						spr1->GetSpriteResource()->GetAnimations().erase(spr1->GetSpriteResource()->GetAnimations().begin() + i);

						selected = 0;
						spr1->SetCurrentAnimationIndex(0);
						spr1->SetAnimationTime(0.0f);

						continue;
					}
				}

				SpriteResource::Animation& anim = spr1->GetSpriteResource()->GetAnimations().at(i);

				ImGuiManager::Instance().InputText("name", anim.name);
				ImGui::InputInt("FrameNum", &anim.frameNum);
				ImGui::InputInt("FramePerRow", &anim.framePerRow);
				ImGui::InputInt("FrameWidth", &anim.frameWidth);
				ImGui::InputInt("FrameHeight", &anim.frameHeight);
				ImGui::InputInt("XCellOffset", &anim.xCellOffset);
				ImGui::InputInt("YCellOffset", &anim.yCellOffset);
				ImGui::InputInt("XPixelOffset", &anim.xPixelOffset);
				ImGui::InputInt("YPixelOffset", &anim.yPixelOffset);
				ImGui::InputInt("XPivotPoint", &anim.xPivotPoint);
				ImGui::InputInt("YPivotPoint", &anim.yPivotPoint);
				ImGui::InputFloat("secondsLength", &anim.secondsLength);
			}
		}
		spr1->SetCurrentAnimationIndex(selected);
	}
	
	ImGui::End();

	bool isActiveTextureView = false;
	isActiveTextureView = ImGui::Begin("TextureView");
	{
		if (spr1)
		{
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();

			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;
			vMax.x += ImGui::GetWindowPos().x;
			vMax.y += ImGui::GetWindowPos().y;

			isMouseHoverSceneView = ImGui::IsMouseHoveringRect(vMin, vMax);

			//ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 0, 0, 255));

			// ImGui�̕`��\�̈�̎擾
			ImVec2 WindowSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
			DirectX::XMFLOAT3 imWindowSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 0 };
			DirectX::XMFLOAT3 windowSize = { static_cast<float>(spr1->GetSpriteResource()->GetSize().x), static_cast<float>(spr1->GetSpriteResource()->GetSize().y), 0 };

			DirectX::XMFLOAT3 aspectWindow = imWindowSize / windowSize;
			DirectX::XMFLOAT3 drawWindowSize = { 0,0,0 };
			DirectX::XMFLOAT3 drawPos = { 0,0,0 };

			if (aspectWindow.x > aspectWindow.y)
			{
				drawWindowSize.x = imWindowSize.y / windowSize.y * windowSize.x;
				drawWindowSize.y = imWindowSize.y;
				drawPos.x = (imWindowSize.x - drawWindowSize.x) / 2;
			}
			else
			{
				drawWindowSize.x = imWindowSize.x;
				drawWindowSize.y = imWindowSize.x / windowSize.x * windowSize.y;
				drawPos.y = (imWindowSize.y - drawWindowSize.y) / 2;
			}

			ImGui::SetCursorPos({ ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowContentRegionMin().y + drawPos.y });
			ImGui::Image(imGuiFrameBuffer->shaderResourceViews[0].Get(), { drawWindowSize.x, drawWindowSize.y }, { 0, 0 }, { 1,1 }, { 1, 1, 1, 1 });


			ImVec2 topLeft = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y + drawPos.y };
			ImVec2 bottomRight = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x + drawPos.x + drawWindowSize.x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y + drawPos.y + drawWindowSize.y };


			//ImGui::GetForegroundDrawList()->AddRect(topLeft, bottomRight, IM_COL32(255, 255, 0, 255));
			if (isActiveTextureView)
			{
				for (int i = 0; i < spr1->GetSpriteResource()->GetAnimations().size(); i++)
				{
					if (i == spr1->GetCurrentAnimationIndex())
					{
						SpriteResource::Animation& anim = spr1->GetSpriteResource()->GetAnimations().at(i);

						for (int frameIndex = 0; frameIndex < anim.frameNum; frameIndex++)
						{
							Framework* frameWork = &Framework::Instance();
							ImVec2 delta(
								bottomRight.x - topLeft.x,
								bottomRight.y - topLeft.y
							);
							ImVec2 aspect(
								delta.x / spr1->GetSpriteResource()->GetSize().x,
								delta.y / spr1->GetSpriteResource()->GetSize().y
							);

							ImVec2 pos1(
								topLeft.x + (((anim.frameWidth * (frameIndex % anim.framePerRow)) + (anim.xCellOffset * anim.frameWidth) + anim.xPixelOffset) * aspect.x),
								topLeft.y + (((anim.frameHeight * (frameIndex / anim.framePerRow)) + (anim.yCellOffset * anim.frameHeight) + anim.yPixelOffset) * aspect.y)
							);
							ImVec2 pos2(
								pos1.x + (anim.frameWidth * aspect.x),
								pos1.y + (anim.frameHeight * aspect.y)
							);

							ImGui::GetForegroundDrawList()->AddRect(pos1, pos2, IM_COL32(255, 255, 255, 255));
						}
					}
				}
			}
		}
	}
	ImGui::End();


	bool isActiveAnimationView = false;
	isActiveAnimationView = ImGui::Begin("AnimationView");
	{
		if (spr1 && !spr1->GetSpriteResource()->GetAnimations().empty())
		{
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();

			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;
			vMax.x += ImGui::GetWindowPos().x;
			vMax.y += ImGui::GetWindowPos().y;

			isMouseHoverSceneView = ImGui::IsMouseHoveringRect(vMin, vMax);

			//ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 0, 0, 255));

			// ImGui�̕`��\�̈�̎擾
			ImVec2 WindowSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
			DirectX::XMFLOAT3 imWindowSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 0 };
			DirectX::XMFLOAT3 windowSize = { 
				static_cast<float>(spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameWidth), 
				static_cast<float>(spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex()).frameHeight), 
				0
			};

			DirectX::XMFLOAT3 aspectWindow = imWindowSize / windowSize;
			DirectX::XMFLOAT3 drawWindowSize = { 0,0,0 };
			DirectX::XMFLOAT3 drawPos = { 0,0,0 };

			if (aspectWindow.x > aspectWindow.y)
			{
				drawWindowSize.x = imWindowSize.y / windowSize.y * windowSize.x;
				drawWindowSize.y = imWindowSize.y;
				drawPos.x = (imWindowSize.x - drawWindowSize.x) / 2;
			}
			else
			{
				drawWindowSize.x = imWindowSize.x;
				drawWindowSize.y = imWindowSize.x / windowSize.x * windowSize.y;
				drawPos.y = (imWindowSize.y - drawWindowSize.y) / 2;
			}

			ImGui::SetCursorPos({ ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowContentRegionMin().y + drawPos.y });
			ImGui::Image(imGuiFrameBuffer2->shaderResourceViews[0].Get(), { drawWindowSize.x, drawWindowSize.y }, { 0, 0 }, { 1,1 }, { 1, 1, 1, 1 });

			ImVec2 topLeft = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y + drawPos.y };
			ImVec2 bottomRight = { ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMin().x + drawPos.x + drawWindowSize.x, ImGui::GetWindowPos().y + ImGui::GetWindowContentRegionMin().y + drawPos.y + drawWindowSize.y };

			if (isActiveAnimationView)
			{
				if (!spr1->GetSpriteResource()->GetAnimations().empty())
				{
					SpriteResource::Animation& anim = spr1->GetSpriteResource()->GetAnimations().at(spr1->GetCurrentAnimationIndex());

					for (int frameIndex = 0; frameIndex < anim.frameNum; frameIndex++)
					{
						Framework* frameWork = &Framework::Instance();
						ImVec2 delta(
							bottomRight.x - topLeft.x,
							bottomRight.y - topLeft.y
						);
						ImVec2 aspect(
							delta.x / anim.frameWidth,
							delta.y / anim.frameHeight
						);

						ImVec2 pos(
							topLeft.x + anim.xPivotPoint * aspect.x,
							topLeft.y + anim.yPivotPoint * aspect.y
						);

						ImGui::GetForegroundDrawList()->AddCircle(pos, 3, IM_COL32(255, 255, 255, 255));
					}
				}
			}
		}
	}
	ImGui::End();
	
	// --- Console�`�� ---
	ImGuiManager::Instance().Console();
}

// ���f���G�f�B�^�[�p���j���[�o�[�`��
void SceneSpriteEditor::DrawSpriteEditorMenuBar()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("SpriteEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneSpriteEditor);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load")) {
			if (ImGui::MenuItem(".imageFormat"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
				TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string spritePath = relativePath.string();

					// �X�v���C�g�Ǎ�
					spr1 = std::make_unique<Sprite>(spritePath.c_str());

					ConsoleData::Instance().logs.push_back(spr1->GetSpriteResource()->GetFilePath() + u8" �Ǎ�����");

					selected = 0;
				}

				Timer::Instance().Start();
			}
			if (ImGui::MenuItem(".sprite"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
				TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("sprite�t�@�C��(*.sprite)\0*.sprite\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string spritePath = relativePath.string();

					// �X�v���C�g�Ǎ�
					spr1 = std::make_unique<Sprite>(spritePath.c_str());

					ConsoleData::Instance().logs.push_back(spr1->GetSpriteResource()->GetFilePath() + u8" �Ǎ�����");

					selected = 0;
				}

				Timer::Instance().Start();
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem(".sprite"))
			{
				// ������ .sprite �����o��
				spr1->GetSpriteResource()->OutputSpriteData();

				ConsoleData::Instance().logs.push_back(spr1->GetSpriteResource()->GetFilePath() + u8" �ۑ�����");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

