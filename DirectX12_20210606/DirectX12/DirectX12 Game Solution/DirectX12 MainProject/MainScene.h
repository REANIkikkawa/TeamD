//
// MainScene.h
//

#pragma once

#include "Scene.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;

class MainScene final : public Scene {
public:
	MainScene();
	virtual ~MainScene() { Terminate(); }

	MainScene(MainScene&&) = default;
	MainScene& operator= (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator= (MainScene const&) = delete;

	// These are the functions you will implement.
	void Initialize() override;
	void LoadAssets() override;

	void Terminate() override;

	void OnDeviceLost() override;
	void OnRestartSound() override;

	NextScene Update(const float deltaTime) override;
	void Render() override;

private:
	DX12::DESCRIPTORHEAP descriptorHeap;
	DX12::SPRITEBATCH    spriteBatch;
	DX12::HGPUDESCRIPTOR dx9GpuDescriptor;

	// �ϐ��̐錾
	void PlayerController(const float deltaTime);

	DX9::SPRITE player_sprite_;
	DX9::SPRITE bg_sprite_;

	SimpleMath::Vector3 player_position_;

	const float PLAYER_WIDTH1 = -180.0f;
	const float PLAYER_WIDTH2 = 1280.0f;

	float player_X = 0.0f;
	float player_Y = 360.0f;

	float player_move_speed = 900.0f;

	float player_minimum_said = 0 - 20;
	float player_full_said = 1280 - 100;

	float player_minimum_vertical = 0 - 20;
	float player_hull_vertical = 720 - 150;

	float xCircle;
	float yCircle;
	float XSquare;
	float YSquare;
	float xSquare;
	float ySquare;

};