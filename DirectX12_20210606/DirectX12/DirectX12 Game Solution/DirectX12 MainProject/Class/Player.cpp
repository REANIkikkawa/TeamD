#include "Player.h"

void Player::Initialize()
{
	_mode_state = _Mode::Right;

}

void Player::LoadAssets()
{
	_Rsprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"test16b_R.png");
	_Lsprite = DX9::Sprite::CreateFromFile(DXTK->Device9, L"test16b_L.png");
}

void Player::Update(const float deltaTime)
{
    
}

void Player::Render()
{
    
}