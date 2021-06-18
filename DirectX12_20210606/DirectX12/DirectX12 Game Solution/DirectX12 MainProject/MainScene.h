//
// MainScene.h
//

#pragma once

#include "Scene.h"
#include <random>

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

	// 変数の宣言
	void PlayerController(const float deltaTime);

	DX9::SPRITE player_sprite_;
	DX9::SPRITE bg_sprite_;
	DX9::SPRITE enemy_Sprite_;
	DX9::SPRITE enemy2_Sprite_;

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

	//当たり判定
	float hitflag;

	//エネミー1
	float enemyX, enemyY;
	float enemy_Speed;
	float enemy_Theta;
	float enemy_BaseX;
	float enemy_BaseY;
	float enemy_Count;

	//エネミー2
	float enemy2X, enemy2Y;
	float enemy2_Speed;
	float enemy2_Theta;
	float enemy2_BaseX;
	float enemy2_BaseY;
	float enemy2_Count;

	//雑魚の行動
	std::mt19937 randomEngine;
	std::uniform_real_distribution<float> randomXIN;
	std::uniform_real_distribution<float> randomYIN;
	std::uniform_real_distribution<float> randomSpeedIN;

};