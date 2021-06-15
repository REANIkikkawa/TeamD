//
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

    // ïœêîÇÃêÈåæ
    void PlayerController(const float deltaTime);

    DX9::SPRITE player_sprite_;

    float player_X = 0.0f;
    float player_Y = 360.0f;

    float player_move_speed = 500.0f;

    SimpleMath::Vector3 player_position;

    void Player_Nomal_Shot(const float deltaTime);

    DX9::SPRITE shot_sprite_;

    int shot_X[100];
    int shot_Y[100];
    int shot_flag[100];

    float shot_speed = 1000.0f;

    void Player_Charge_Shot(const float deltaTime);

    DX9::SPRITE charge_shot_sprite_;

    int charge_shot_X;
    int charge_shot_Y;
    int charge_shot_flag;

    float charge_time = 0.0f;

};