//
// MainScene.cpp
//

#include "Base/pch.h"
#include "Base/dxtk.h"
#include "SceneFactory.h"

// Initialize member variables.
MainScene::MainScene() : dx9GpuDescriptor{}
{

}

// Initialize a variable and audio resources.
void MainScene::Initialize()
{
    for (int i = 0; i <= 100; i++) {
        shot_X[i]    = 0;
        shot_Y[i]    = 0;
        shot_flag[i] = 0;
    }

    charge_shot_flag = 0;
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
    player_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9,L"robot_e.png");
    
    shot_sprite_   = DX9::Sprite::CreateFromFile(DXTK->Device9, L"laser.png");

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
    // If you use 'deltaTime', remove it.
    UNREFERENCED_PARAMETER(deltaTime);

    // TODO: Add your game logic here.

    //プレイヤーの動き
    PlayerController(deltaTime);

    //プレイヤーの通常ショット
    Player_Nomal_Shot(deltaTime);
   

    return NextScene::Continue;
}

// Draws the scene.
void MainScene::Render()
{
    // TODO: Add your rendering code here.
    DXTK->Direct3D9->Clear(DX9::Colors::RGBA(0, 0, 0, 255));

    DXTK->Direct3D9->BeginScene();
    DX9::SpriteBatch->Begin();

    DX9::SpriteBatch->DrawSimple(player_sprite_.Get(),
        SimpleMath::Vector3(player_position.x,player_position.y,0.0f)
    );

    for (int i = 0; i <= 50  ; i++) {
        if (shot_flag[i] == 1) {
            DX9::SpriteBatch->DrawSimple(
                shot_sprite_.Get(),
                SimpleMath::Vector3(shot_X[i], shot_Y[i], 0.0f)
            );
        }
    }


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
    //プレイヤー(ゲームパッド)
    constexpr float SPEED = 500.0f;
    SimpleMath::Vector3 movement = SimpleMath::Vector3(
        DXTK->GamePadState[0].thumbSticks.leftX,
        -DXTK->GamePadState[0].thumbSticks.leftY,
        0.0f
    );



    const float SQUARE_X = DXTK->GamePadState[0].thumbSticks.leftX;
    const float SQUARE_Y = -DXTK->GamePadState[0].thumbSticks.leftY;


    movement.x = SQUARE_X * sqrt(1.0f - 0.5 * SQUARE_Y * SQUARE_Y);
    movement.y = SQUARE_Y * sqrt(1.0f - 0.5 * SQUARE_X * SQUARE_X);


    //プレイヤー（キーボード）

    /*if (DXTK->KeyState->Left) {
        movement += SimpleMath::Vector3(-1, 0, 0);
    }
    if (DXTK->KeyState->Right) {
        movement += SimpleMath::Vector3(1, 0, 0);
    }
    if (DXTK->KeyState->Up) {
        movement += SimpleMath::Vector3(0, -1, 0);
    }
    if (DXTK->KeyState->Down) {
        movement += SimpleMath::Vector3(0, 1, 0);
    }*/

    


    //プレイヤーの移動範囲
    player_position += movement * player_move_speed * deltaTime;
    player_position.Clamp(SimpleMath::Vector3(0.0f, 0.0f, 0),
        SimpleMath::Vector3(1280.0f - 150.0f, 720.0f - 150.0f, 0)
    );
}

void MainScene::Player_Nomal_Shot(const float deltaTime) 
{

    //ショット発射
    if (shot_flag == 0) {
        if (DXTK->KeyState->Space|| DXTK->GamePadState->buttons.b) {
            for (int i = 0; i < 20; ++i) {
                shot_flag[i] = 1;
                shot_X[i] = player_position.x;
                shot_Y[i] = player_position.y;
                break;
            }
        }
    }

    if (DXTK->KeyState->Space || DXTK->GamePadState->buttons.b) {
        for (int i = 0; i < 20; ++i) {
            if (shot_flag[i] == 0) {
                shot_flag[i] = 1;
                shot_X[i] = player_position.x + 90;
                shot_Y[i] = player_position.y + 60;
                break;
            }
        }
    }

    //ショットの撃ち直しができるプログラム
    for (int i = 0; i < 20; ++i) {
        if (shot_flag[i] == 1) {
            ; shot_X[i] += 1000.0f * deltaTime;
            if (shot_X[i] > 1280) {
                shot_flag[i] = 0;
                shot_X[i] = 0;
                shot_Y[i] = 0;
            }
        }
    }


}

void MainScene::Player_Charge_Shot(const float deltaTime)
{
    if (charge_shot_flag == 0) {
        if (DXTK->KeyState->Enter) {
            charge_shot_flag = 1;

        }
    }
}