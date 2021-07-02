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
	enemyX = 1280;
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

	// randomXIN = std::uniform_real_distribution<float>(1280.0f, 0.0f);
	randomYIN = std::uniform_real_distribution<float>(-500.0f, -150.0f);
	randomSpeedIN = std::uniform_real_distribution<float>(0.9f, 1.0f);

	shot_chang_state = Shot_Chang::Rapid_fire;

	shot_fire_state = SHOT_FIRE::not_fire;
	special_shot_fire_state = SPECIAL_SHOT_FIRE::not_fire;

	special_shot_meter = zero;

	player_shot_se = XAudio::CreateSoundEffect(DXTK->AudioEngine, L"ショット.wav");
	player_shothit_se = XAudio::CreateSoundEffect(DXTK->AudioEngine, L"ショット命中.wav");
	beam_shot_se_ = XAudio::CreateSoundEffect(DXTK->AudioEngine, L"ビーム砲1.wav");


	player_mode_state = Player_Mode::Right;

	player_position.x = shot_X;
	player_position.y = shot_Y;

	enemy_position.x = enemy_X;
	enemy_position.y = enemy_Y;

	enemy_B_Theta = 0;
	enemy_B_BaseY = enemy_position.y;

	//ショット
	beam_shot_X = 0;
	beam_shot_Y = 50;

	beam_shot_posiston.x = beam_shot_X;
	beam_shot_posiston.y = beam_shot_Y;


	//背景
	back_grund_02_posiston.x = back_grund_X;



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
	player_Rsprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"test16b_R.png");
	player_Lsprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"test16b_L.png");

	DXTK->Direct3D9->SetRenderState(NormalizeNormals_Enable);
	DXTK->Direct3D9->SetRenderState(Specular_Enable);






	//ショット
	shot_sprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"laser.png");
	beam_shot_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"ビーム5.png");

	special_shot_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"Special attack.png");

	//エネミーA
	enemy_Sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"s-company_character_black.png");
	enemy2_Sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"s-figure_oufuku_binta.png");

	//エネミーB
	enemy_B_sprite_ = DX9::Sprite::CreateFromFile(DXTK->Device9, L"robot_h.png");

	//背景
	bg_skysprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"bg_sky_v01.png");
	bg_seasprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"bg_sea_v01.png");


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
	Player_Shot(deltaTime);

	//プレイヤーの特殊ショット(ビーム)
	Player_Special_Shot(deltaTime);
	//背景の移動
	BACK_GRUND(deltaTime);

	return NextScene::Continue;
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

	//if (DXTK->KeyState->Left && !DXTK->KeyState->Right) {
	//    movement += SimpleMath::Vector3(-0.5, 0, 0);
	//}
	//if (DXTK->KeyState->Right && !DXTK->KeyState->Left) {
	//    movement += SimpleMath::Vector3(1, 0, 0);
	//}
	//if (DXTK->KeyState->Up && !DXTK->KeyState->Down) {
	//    movement += SimpleMath::Vector3(0, -1, 0);
	//}
	//if (DXTK->KeyState->Down && !DXTK->KeyState->Up) {
	//    movement += SimpleMath::Vector3(0, 1, 0);
	//}




	if (movement.x > 0.0f) {

		player_mode_state = Player_Mode::Right;

	}
	else if (movement.x < 0.0f) {

		player_mode_state = Player_Mode::Left;
		movement += SimpleMath::Vector3(0.5, 0, 0);
	}



	//プレイヤーの移動範囲
	player_position += movement * player_move_speed * deltaTime;
	player_position.Clamp(SimpleMath::Vector3(0.0f, 0.0f, 0),
		SimpleMath::Vector3(1280.0f - 150.0f, 720.0f - 150.0f, 0)
	);



	shot_angle = atan2(-DXTK->GamePadState[0].thumbSticks.rightY, DXTK->GamePadState[0].thumbSticks.rightX);

	float speed = 0.04f;

	//ショット発射
	if (DXTK->KeyState->Space || DXTK->GamePadState[0].buttons.rightShoulder) {

		if (shot_chang_state == Shot_Chang::Rapid_fire) {
			for (int i = 0; i < 1; ++i) {
				if (shot_fire_state == SHOT_FIRE::not_fire) {
					shot_fire_state = SHOT_FIRE::fire;
					shot_posiston.x = player_position.x + shot_X;
					shot_posiston.y = player_position.y + shot_Y;
					shot_posiston.z = -10.0f;
					player_shot_se->Play();
					shot_angle;
					break;
				}
			}
		}

	}
	//ショットの撃ち直しができるプログラム
	for (int i = 0; i < 1; ++i) {

		if (shot_chang_state == Shot_Chang::Rapid_fire) {
			if (shot_fire_state == SHOT_FIRE::fire) {

				shot_posiston.x += shot_speed * cos(shot_angle) * deltaTime;
				shot_posiston.y += shot_speed * sin(shot_angle) * deltaTime;
				if (shot_posiston.x >= 1800) {
					shot_fire_state = SHOT_FIRE::not_fire;
					shot_posiston.x = 0;
					shot_posiston.y = 0;
				}
				else if (shot_posiston.x <= -100) {
					shot_fire_state = SHOT_FIRE::not_fire;
					shot_posiston.x = 0;
					shot_posiston.y = 0;

				}
				else if (shot_posiston.y <= -100) {
					shot_fire_state = SHOT_FIRE::not_fire;
					shot_posiston.x = 0;
					shot_posiston.y = 0;
				}
				else if (shot_posiston.y >= 820) {
					shot_fire_state = SHOT_FIRE::not_fire;
					shot_posiston.x = 0;
					shot_posiston.y = 0;
				}
			}
		}
		const float SQUARE_X = DXTK->GamePadState[0].thumbSticks.rightX;
		const float SQUARE_Y = -DXTK->GamePadState[0].thumbSticks.rightY;


		movement.x = SQUARE_X * sqrt(1.0f - 0.5 * SQUARE_Y * SQUARE_Y);
		movement.y = SQUARE_Y * sqrt(1.0f - 0.5 * SQUARE_X * SQUARE_X);
	}
}

void MainScene::Player_Special_Shot(const float deltaTime)
{
	SimpleMath::Vector3 movement = SimpleMath::Vector3(
		DXTK->GamePadState[0].thumbSticks.rightX,
		-DXTK->GamePadState[0].thumbSticks.rightY,
		0.0f
	);

	shot_angle = atan2(-DXTK->GamePadState[0].thumbSticks.rightY, DXTK->GamePadState[0].thumbSticks.rightX);


	if (DXTK->GamePadEvent[0].rightTrigger == GamePad::ButtonStateTracker::PRESSED) {
		if (special_shot_meter >= 10) {
			for (int i = 0; i < 1; i++) {
				if (special_shot_fire_state == SPECIAL_SHOT_FIRE::not_fire) {
					special_shot_fire_state = SPECIAL_SHOT_FIRE::fire;
					special_shot_position.x = player_position.x + special_shot_X;
					special_shot_position.y = player_position.y + special_shot_Y;
					special_shot_position.z = -10.0f;
					player_shot_se->Play();
					shot_angle;
					break;

				}
			}
		}
	}

	//ショットの撃ち直しができるプログラム
	for (int i = 0; i < 1; ++i) {
		if (special_shot_time > special_shot_off_time) {
			if (special_shot_fire_state == SPECIAL_SHOT_FIRE::fire) {
				special_shot_fire_state = SPECIAL_SHOT_FIRE::not_fire;

			}
		}

	}
	if (special_shot_time > special_shot_off_time) {

		special_shot_time -= deltaTime;
	}

}

void MainScene::Player_HIT()
{
	//当たり判定
	if (!(enemy_X > player_position.x + 150.0f || enemy_X + 150.0f < player_position.y ||
		enemy_Y > player_position.x + 150.0f || enemy_Y + 150.0f < player_position.y)) {
		--playerHP;//当たっている
	}
}

void MainScene::Enemy_HIT()
{
	//エネミーBの当たり判定
	for (int i = 0; i < 1; ++i) {

		if (!(shot_posiston.x > enemy_position.x + 150.0f || shot_posiston.x + 200.0f < enemy_position.x ||
			shot_posiston.y > enemy_position.y + 150.0f || shot_posiston.y + 100.0f < enemy_position.y)) {
			--enemy_HP;
			if (enemy_HP <= 0) {
				special_shot_meter += 10;
			}
			player_shothit_se->Play();

		}
	}
}

//void MainScene::ENEMY_MOVE(const float deltaTime)
//{
//    enemy_Theta += 20.0f * deltaTime;
//    if (enemy_Theta >= XM_2PI)
//        enemy_Theta -= XM_2PI;
//    enemyY = enemy_BaseY + sinf(enemy_Theta) * -400.0f;
//    enemyX += enemy_Speed * -3000.0f * deltaTime;
//
//    enemy2_Theta += 2.0f * deltaTime;
//    if (enemy2_Theta >= XM_2PI)
//        enemy_Theta -= XM_2PI;
//    enemy2Y = enemy2_BaseY + sinf(enemy2_Theta) * 200.0f;
//    enemy2X += enemy2_Speed * 300.0f * deltaTime;
//
//    if (enemyX > 1280) {
//        enemyX = randomXIN(randomEngine);
//        enemyY = randomYIN(randomEngine);
//        enemy_Speed = randomSpeedIN(randomEngine);
//        ++enemy_Count;
//        hitflag = 1;
//
//    }
//
//    if (enemy2X > 1280) {
//        enemy2X = randomXIN(randomEngine);
//        enemy2Y = randomYIN(randomEngine);
//        enemy2_Speed = randomSpeedIN(randomEngine);
//        ++enemy2_Count;
//        hitflag = 1;
//
//    }
//}





void MainScene::BACK_GRUND(const float deltaTime)
{
	back_grund_posiston.x -= back_grund_speed * deltaTime;
	if (back_grund_posiston.x <= back_grund_end) {
		back_grund_posiston.x = back_grund_begin;

	}

	back_grund_02_posiston.x -= back_grund_speed * deltaTime;
	if (back_grund_02_posiston.x <= back_grund_end) {
		back_grund_02_posiston.x = back_grund_begin;

	}

}

void MainScene::Render()
{
	DXTK->Direct3D9->Clear(DX9::Colors::RGBA(0, 0, 0, 255));

	DXTK->Direct3D9->BeginScene();
	DX9::SpriteBatch->Begin();

	///プレイヤーの描画
	if (player_mode_state == Player_Mode::Right) {
		DX9::SpriteBatch->DrawSimple(player_Rsprite.Get(),
			SimpleMath::Vector3(player_position.x, player_position.y, 0.0f)
		);
	}

	if (player_mode_state == Player_Mode::Left) {
		DX9::SpriteBatch->DrawSimple(player_Lsprite.Get(),
			SimpleMath::Vector3(player_position.x, player_position.y, 0.0f)
		);
	}




	//背景
	DX9::SpriteBatch->DrawSimple(bg_seasprite.Get(),
		SimpleMath::Vector3(back_grund_posiston.x, back_grund_posiston.y, 5.0f)
	);

	DX9::SpriteBatch->DrawSimple(bg_skysprite.Get(),
		SimpleMath::Vector3(back_grund_posiston.x, back_grund_posiston.y, 10.0f)
	);

	//背景2枚目
	DX9::SpriteBatch->DrawSimple(bg_seasprite.Get(),
		SimpleMath::Vector3(back_grund_02_posiston.x, back_grund_02_posiston.y, 5.0f)
	);

	DX9::SpriteBatch->DrawSimple(bg_skysprite.Get(),
		SimpleMath::Vector3(back_grund_02_posiston.x, back_grund_02_posiston.y, 10.0f)
	);
	//特殊攻撃
	if (special_shot_fire_state == SPECIAL_SHOT_FIRE::fire) {
		DX9::SpriteBatch->DrawSimple(
			special_shotsprite.Get(),
			SimpleMath::Vector3(special_shot_position.x, special_shot_position.y, 0.0f)
		);
	}

	//プレイヤーのショット
	if (shot_chang_state == Shot_Chang::Rapid_fire) {
		if (shot_fire_state == SHOT_FIRE::fire) {
			auto origin = SimpleMath::Vector3(-10.0f, 25.0f, 0.0f);
			DX9::SpriteBatch->Draw(
				shotsprite.Get(),
				shot_posiston, nullptr,
				DX9::Colors::White,
				shot_angle, &origin
			);
		}
	}



	if (shot_chang_state == Shot_Chang::Single_shot) {
		if (shot_fire_state == SHOT_FIRE::fire) {
			auto origin = SimpleMath::Vector3(0.0f, 25.0f, 0.0f);
			DX9::SpriteBatch->Draw(
				beam_shotsprite.Get(),
				beam_shot_posiston, nullptr,
				DX9::Colors::White,
				shot_angle, &origin
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

//ショット発射
void MainScene::Player_Shot(const float dentaTime)
{
	if (DXTK->KeyState->Space || DXTK->GamePadState[0].buttons.rightShoulder) {

		if (shot_chang_state == Shot_Chang::Rapid_fire) {
			for (int i = 0; i < 1; ++i) {
				if (shot_fire_state == SHOT_FIRE::not_fire) {
					shot_fire_state = SHOT_FIRE::fire;
					shot_posiston.x = player_position.x + shot_X;
					shot_posiston.y = player_position.y + shot_Y;
					shot_posiston.z = -10.0f;
					player_shot_se->Play();
					shot_angle;
					break;
				}
			}
		}

	}
}