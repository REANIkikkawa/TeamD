//
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


    //プレイヤー
    void PlayerController(const float deltaTime);

    DX9::SPRITE player_Rsprite;
    DX9::SPRITE player_Lsprite;
    XAudio::SOUNDEFFECT player_shot_se;


    enum class Player_Mode {
        Left,
        Right
    };
    Player_Mode player_mode_state;


    float player_X = 0.0f;
    float player_Y = 360.0f;

    float player_move_speed = 500.0f;

    SimpleMath::Vector3 player_position;

    //プレイヤーのショット
    void Player_Shot(const float deltaTime);

    enum class Shot_Chang
    {
        Rapid_fire,
        Single_shot
    };
    Shot_Chang shot_chang_state;

    DX9::SPRITE shotsprite;
    SimpleMath::Vector3 shot_posiston;


    //プレイヤーのショット単発
    DX9::SPRITE beam_shot_sprite_;
    XAudio::SOUNDEFFECT beam_shot_se_;

    float shot_angle;


    int shot_flag;

    enum class SHOT_FIRE
    {
        not_fire,
        fire
    };

    SHOT_FIRE shot_fire_state;

    float shot_speed = 7500.0f;
    float beam_shot_speed = 3000.0f;


    float shot_X = 90.0f;
    float shot_Y = 50.0f;


    void Player_Special_Shot(const float deltaTime);

    DX9::SPRITE special_shot_sprite_;

    SimpleMath::Vector3 special_shot_position;
    XAudio::SOUNDEFFECT special_shot_se;

    SimpleMath::Vector3 beam_shot_posiston;

    DX9::SPRITE beam_shotsprite;
    float beam_shot_X = 0.0f;
    float beam_shot_Y = 50.0f;

    int zero = 0;
    int special_shot_meter = 10;

    enum class SPECIAL_SHOT_FIRE
    {
        not_fire,
        fire
    };
    SPECIAL_SHOT_FIRE special_shot_fire_state;

    SimpleMath::Vector3 special_shot_state;
    float special_shot_X = 10.0f;
    float special_shot_Y = -50.0f;

    float special_shot_time = 0.0f;
    float special_shot_off_time = 0.3f;

    void Player_HIT();
    int playerHP = 100;

    //エネミー1

    DX9::SPRITE enemy_Sprite_;
    float enemyX;
    float enemyY;
    float enemy_Speed;
    float enemy_Theta;
    float enemy_BaseX;
    float enemy_BaseY;
    float enemy_Count;

    //エネミー2
    DX9::SPRITE enemy2_Sprite_;
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

    //エネミーB

    DX9::SPRITE enemy_B_sprite_;

    float enemy_X = 1300.0f;
    float enemy_Y = 360.0f - 75.0f;

    float enemy_speed = 100.0f;

    float enemy_B_Theta;
    float enemy_B_BaseY;

    //エネミーの当たり判定
    void Enemy_HIT();

    //ショット
    DX9::SPRITE shot_sprite;
    DX9::SPRITE special_shotsprite;

    SimpleMath::Vector3 enemy_position;
    SimpleMath::Vector3 movement;


    int enemy_HP = 10;

    XAudio::SOUNDEFFECT player_shothit_se;


    //背景の移動
    void BACK_GRUND(const float deltaTime);

    DX9::SPRITE bg_skysprite;
    DX9::SPRITE bg_seasprite;

    SimpleMath::Vector3 back_grund_posiston;
    SimpleMath::Vector3 back_grund_02_posiston;


    float back_grund_speed = 200.0f;
    float back_grund_begin = 0.0f;
    float back_grund_end = -1280.0f;

    float back_grund_X = 2500.0f;
};