#pragma once
#include <memory>
// --- External ---
#include "../../External/ImGui/ImSequencer.h"
#include "../../External/ImGui/imgui_internal.h"
#include "../../External/ImGui/ImCurveEdit.h"
#include "../../External/ImGui/ImGuizmo.h"
// --- libarary ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Resource//Model/ModelResource.h"
// --- Scene ---
#include "Scene.h"
// --- Game ---
#include "../Game/Object/ModelEditorObject.h"

#pragma region SequenceSupporter

static const char* SequencerItemTypeNames[] = { "Empty", "SE", "Sphere", "Effect" };
enum class SequencerItemType
{
	EMPTY,
	SE,
	Sphere,
	Effect
};

struct RampEdit : public ImCurveEdit::Delegate
{
	RampEdit()
	{
		mPts[0][0] = ImVec2(-10.f, 0);
		mPts[0][1] = ImVec2(20.f, 0.6f);
		mPts[0][2] = ImVec2(25.f, 0.2f);
		mPts[0][3] = ImVec2(70.f, 0.4f);
		mPts[0][4] = ImVec2(120.f, 1.f);
		mPointCount[0] = 5;

		mPts[1][0] = ImVec2(-50.f, 0.2f);
		mPts[1][1] = ImVec2(33.f, 0.7f);
		mPts[1][2] = ImVec2(80.f, 0.2f);
		mPts[1][3] = ImVec2(82.f, 0.8f);
		mPointCount[1] = 4;


		mPts[2][0] = ImVec2(40.f, 0);
		mPts[2][1] = ImVec2(60.f, 0.1f);
		mPts[2][2] = ImVec2(90.f, 0.82f);
		mPts[2][3] = ImVec2(150.f, 0.24f);
		mPts[2][4] = ImVec2(200.f, 0.34f);
		mPts[2][5] = ImVec2(250.f, 0.12f);
		mPointCount[2] = 6;
		mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
		mMax = ImVec2(1.f, 1.f);
		mMin = ImVec2(0.f, 0.f);
	}
	size_t GetCurveCount()
	{
		return 3;
	}

	bool IsVisible(size_t curveIndex)
	{
		return mbVisible[curveIndex];
	}
	size_t GetPointCount(size_t curveIndex)
	{
		return mPointCount[curveIndex];
	}

	uint32_t GetCurveColor(size_t curveIndex)
	{
		uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
		return cols[curveIndex];
	}
	ImVec2* GetPoints(size_t curveIndex)
	{
		return mPts[curveIndex];
	}
	virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
	virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
	{
		mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
		SortValues(curveIndex);
		for (size_t i = 0; i < GetPointCount(curveIndex); i++)
		{
			if (mPts[curveIndex][i].x == value.x)
				return (int)i;
		}
		return pointIndex;
	}
	virtual void AddPoint(size_t curveIndex, ImVec2 value)
	{
		if (mPointCount[curveIndex] >= 8)
			return;
		mPts[curveIndex][mPointCount[curveIndex]++] = value;
		SortValues(curveIndex);
	}
	virtual ImVec2& GetMax() { return mMax; }
	virtual ImVec2& GetMin() { return mMin; }
	virtual unsigned int GetBackgroundColor() { return 0; }
	ImVec2 mPts[3][8];
	size_t mPointCount[3];
	bool mbVisible[3];
	ImVec2 mMin;
	ImVec2 mMax;
private:
	void SortValues(size_t curveIndex)
	{
		auto b = std::begin(mPts[curveIndex]);
		auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
		std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

	}
};

struct MySequence : public ImSequencer::SequenceInterface
{
	// interface with sequencer

	virtual int GetFrameMin() const {
		return mFrameMin;
	}
	virtual int GetFrameMax() const {
		return mFrameMax;
	}
	virtual int GetItemCount() const { return (int)myItems.size(); }

	virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
	virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
	virtual const char* GetItemLabel(std::string name, int index) const
	{
		static char tmps[512];
		snprintf(tmps, 512, "[%s] %s", SequencerItemTypeNames[myItems[index].mType], name.c_str());
		return tmps;
	}

	virtual void Get(int index, int** start, int** end, std::string& name, int* type, unsigned int* color)
	{
		MySequenceItem& item = myItems[index];
		if (color)
			*color = 0xFFAA8080; // same color for everyone, return color based on type
		if (start)
			*start = &item.mFrameStart;
		if (end)
			*end = &item.mFrameEnd;
		if (type)
			*type = item.mType;

		name = item.name.c_str();
	}
	virtual void Add(int type) {
		int indexNum = 0;
		for (auto myItem : myItems)
		{
			if (static_cast<int>(myItem.mType) == type)
			{
				indexNum++;
			}
		}

		myItems.push_back(MySequenceItem{ indexNum, "test", type, 0, 10 });
	};
	void Add(std::string name, int type, int startFrame, int endFrame) {
		int indexNum = 0;
		for (auto myItem : myItems)
		{
			if (static_cast<int>(myItem.mType) == type)
			{
				indexNum++;
			}
		}

		myItems.push_back(MySequenceItem{ indexNum, name, type, startFrame, endFrame });
	};
	virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
	virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }


	// my datas
	MySequence() : mFrameMin(0), mFrameMax(0) {}
	int mFrameMin, mFrameMax;
	struct MySequenceItem
	{
		int mTypeIndex;				// そのタイプでのインデックス
		std::string name;			// 名前
		int mType;					// タイプ（上の SequencerItemTypeNames で定義）
		int mFrameStart, mFrameEnd;	// 開始・終了フレーム
	};
	std::vector<MySequenceItem> myItems;
	RampEdit rampEdit;

	int selectItemNum;
	virtual void DoubleClick(int index)
	{
		selectItemNum = index;
	}
};


#pragma endregion



// モデルエディターシーン
class SceneModelEditor : public Scene
{
public:
	SceneModelEditor() {}
	~SceneModelEditor() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

private:
	// デバッグ描画
	void DrawDebugGUI();
	// モデルエディター用メニューバー描画
	void DrawModelEditorMenuBar();
	// グリッド描画
	void DrawGrid(int subdivisions, float scale);

	// ボーン情報描画(ラムダ式とかつかってコード内にかきたい)
	void DrawBoneDebug(const std::vector<ModelResource::Bone>& bones, int boneIndex);

	// タイムライン処理
	void DebugTimeLine();


private:
	// 軸の修正行列
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[4] =
	{
		{ -1,0,0,0, 0,1, 0,0, 0,0,1,0, 0,0,0,1 },	// 0:RHS Y-UP
		{  1,0,0,0, 0,1, 0,0, 0,0,1,0, 0,0,0,1 },	// 1:LHS Y-UP
		{ -1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,0,0,1 },	// 2:RHS Z-UP
		{  1,0,0,0, 0,0, 1,0, 0,1,0,0, 0,0,0,1 },	// 3:LHS Z-UP
	};

private:
	bool isMouseHoverSceneView = false;				// マウスがシーンビュー上にあるか
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui描画用フレームバッファ

	std::unique_ptr<ModelEditorObject> modelObject;

	// シークエンス
	MySequence mySequence;

	// let's create the sequencer
	int selectedEntry = -1;
	int firstFrame = 0;
	bool expanded = true;
};