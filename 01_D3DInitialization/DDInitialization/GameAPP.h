#ifndef GAMEAPP_H
#define GAMEAPP_H
#include"D3DApp.h"
#include"DXTrace.h"

class GameAPP :public D3DApp
{
public:
	GameAPP(HINSTANCE hInstance);
	~GameAPP();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

};






#endif // !GAMEAPP_H
