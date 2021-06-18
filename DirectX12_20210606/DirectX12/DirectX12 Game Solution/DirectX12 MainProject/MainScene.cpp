//
// MainScene.cpp
//

#include "Base/pch.h"
#include "Base/dxtk.h"
#include "SceneFactory.h"

// Initialize member variables.
MainScene::MainScene()
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{

    enemyX = randomXIN(randomEngine);
    enemyY = randomYIN(randomEngine);
    enemy_Speed = randomSpeedIN(randomEngine);
    enemy_Theta = 1280;
    enemy_BaseX = enemyX;
    enemy_BaseY = enemyY;

    enemy2X = randomXIN(randomEngine);
    enemy2Y = randomYIN(randomEngine);
    enemy2_Speed = randomSpeedIN(randomEngine);
    enemy2_Theta = 1280;
    enemy2_BaseX = enemy2X;
    enemy2_BaseY = enemy2Y;

    std::random_device rand_dev;
    randomEngine = std::mt19937(rand_dev());

    randomXIN = std::uniform_real_distribution<float>(-1280.0f, 0.0f);
    randomYIN = std::uniform_real_distribution<float>(-500.0f, -150.0f);
    randomSpeedIN = std::uniform_real_distribution<float>(0.1f, 1.0f);
}

// Allocate all memory the Direct3D and Direct2D resources.
void MainScene::LoadAssets()
{
    descriptorHeap = DX12::CreateDescriptorHeap(DXTK->Device, 1);

    ResourceUploadBatch resourceUploadBatch(DXTK->Device);
    resourceUploadBatch.Begin();

    RenderTargetState rtState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
    SpriteBatchPipelineStateDescription pd(rtState, &CommonStates::NonPremultiplied);
    D3D12_VIEWPORT viewport = {
        0.0f, 0.0f, 1280.0f, 720.0f,
        D3D12_MIN_DEPTH, D3D12_MAX_DEPTH
    };

    spriteBatch = DX12::CreateSpriteBatch(DXTK->Device, resourceUploadBatch, pd, &viewport);

    dx9GpuDescriptor = DXTK->Direct3D9->CreateShaderResourceView(descriptorHeap.get(), 0);

    auto uploadResourcesFinished = resourceUploadBatch.End(DXTK->CommandQueue);
    uploadResourcesFinished.wait();


    // グラフィックリソースの初期化処理

    //プレイヤー
    player_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"robot_e.png");
    bg_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"wallpaperbetter.com_1280x720.png");
    enemy_Sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"s-company_character_black.png");
    enemy2_Sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"s-figure_oufuku_binta.png");
}

// Releasing resources required for termination.
void MainScene::Terminate()
{
    DXTK->ResetAudioEngine();
    DXTK->WaitForGpu();

    // TODO: Add your Termination logic here.

}

// Direct3D resource cleanup.
void MainScene::OnDeviceLost()
{

}

// Restart any looped sounds here
void MainScene::OnRestartSound()
{

}

// Updates the scene.
NextScene MainScene::Update(const float deltaTime)
{
    constexpr float  SPEED = 500.0f;
    const float SQUARE_X = DXTK->GamePadState[0].thumbSticks.leftX;
    const float SQUARE_Y = DXTK->GamePadState[0].thumbSticks.leftY;

    SimpleMath::Vector3 movement;
    movement.x = SQUARE_X * sqrt(1.0f - 0.5f * SQUARE_Y * SQUARE_Y);
    movement.y = SQUARE_Y * sqrt(1.0f - 0.5f * SQUARE_X * SQUARE_X);

    player_position_ += movement * SPEED * deltaTime;

    // If you use 'deltaTime', remove it.
    UNREFERENCED_PARAMETER(deltaTime);

    //
    player_X = std::clamp(player_X, PLAYER_WIDTH1, PLAYER_WIDTH2 - PLAYER_WIDTH1);

    enemy_Theta += 2.0f * deltaTime;
    if (enemy_Theta >= XM_2PI)
        enemy_Theta -= XM_2PI;
    enemyY = enemy_BaseY + sinf(enemy_Theta) * -400.0f;
    enemyX += enemy_Speed * 300.0f * deltaTime;

    enemy2_Theta += 2.0f * deltaTime;
    if (enemy2_Theta >= XM_2PI)
        enemy_Theta -= XM_2PI;
    enemy2Y = enemy2_BaseY + sinf(enemy2_Theta) * 400.0f;
    enemy2X += enemy2_Speed * 300.0f * deltaTime;

    if (enemyX > 1280) {
        enemyX = randomXIN(randomEngine);
        enemyY = randomYIN(randomEngine);
        enemy_Speed = randomSpeedIN(randomEngine);
        ++enemy_Count;
        hitflag = 1;

    }

    if (enemy2X > 1280) {
        enemy2X = randomXIN(randomEngine);
        enemy2Y = randomYIN(randomEngine);
        enemy2_Speed = randomSpeedIN(randomEngine);
        ++enemy2_Count;
        hitflag = 1;

    }

    return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
    // TODO: Add your rendering code here.
    DXTK->Direct3D9->Clear(DX9::Colors::RGBA(0, 0, 0, 255));

    DXTK->Direct3D9->BeginScene();
    DX9::SpriteBatch->Begin();

    DX9::SpriteBatch->DrawSimple(player_sprite_.Get(), SimpleMath::Vector3(player_X, player_Y, 0.0f));
    DX9::SpriteBatch->DrawSimple(bg_sprite_.Get(), SimpleMath::Vector3(0.0, 0.0f, 4.0f));
    DX9::SpriteBatch->DrawSimple(enemy_Sprite_.Get(), SimpleMath::Vector3(enemyX, enemyY, 1.0f));
    DX9::SpriteBatch->DrawSimple(enemy2_Sprite_.Get(), SimpleMath::Vector3(enemy2X, enemy2Y, 1.0f));


    DX9::SpriteBatch->End();
    DXTK->Direct3D9->EndScene();

    DXTK->Direct3D9->UpdateResource();

    DXTK->ResetCommand();
    DXTK->ClearRenderTarget(Colors::CornflowerBlue);

    const auto heapes = descriptorHeap->Heap();
    DXTK->CommandList->SetDescriptorHeaps(1, &heapes);

    spriteBatch->Begin(DXTK->CommandList);
    spriteBatch->Draw(
        dx9GpuDescriptor,
        XMUINT2(1280, 720),
        SimpleMath::Vector2(0.0f, 0.0f)
    );
    spriteBatch->End();

    DXTK->Direct3D9->WaitUpdate();

    DXTK->ExecuteCommandList();
}

void MainScene::PlayerController(const float deltaTime)
{

    //プレイヤー（キーボード）
    if (DXTK->KeyState->D) {
        player_X += player_move_speed * deltaTime;
    }

    if (DXTK->KeyState->A) {
        player_X += -player_move_speed * deltaTime;
    }

    if (DXTK->KeyState->W) {
        player_Y += -player_move_speed * deltaTime;
    }

    if (DXTK->KeyState->S) {
        player_Y += player_move_speed * deltaTime;
    }

    //プレイヤーの移動範囲
    if (player_X < player_minimum_said) {
        player_X = player_minimum_said;
    }

    if (player_X > player_full_said) {
        player_X = player_full_said;
    }

    if (player_Y < player_minimum_vertical) {
        player_Y = player_minimum_vertical;
    }

    if (player_Y > player_hull_vertical) {
        player_Y = player_hull_vertical;
    }

    //エネミーの動き

}
