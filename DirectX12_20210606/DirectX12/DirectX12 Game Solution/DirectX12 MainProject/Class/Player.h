#include "Base/pch.h"
#include "Base/dxtk.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;


class Player {

public:
	//ã@î\
	void Update(const float deltaTime);
	void Render();
	void LoadAssets();
	void Initialize();
	void Controller(const float deltaTime);
	
	

private:
	//íËêîÅ@ïœêî

	//Controller
	DX9::SPRITE _Rsprite;
	DX9::SPRITE _Lsprite;
	XAudio::SOUNDEFFECT _shot_se;


	enum class _Mode {
		Left,
		Right
	};
	_Mode _mode_state;


	float _X = 0.0f;
	float _Y = 360.0f;

	float _move_speed = 500.0f;

	SimpleMath::Vector3 _position;

};