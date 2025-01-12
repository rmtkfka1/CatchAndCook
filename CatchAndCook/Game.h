#pragma once
class Game
{
public:

	void Init(HWND hwnd);
	void PrevUpdate();
	void Run();
	void Release();

private:
	HWND _hwnd;

	//temp
	shared_ptr<Shader> _shader;
	shared_ptr<GameObject> _gameObjects;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	shared_ptr<Material> _material;
	shared_ptr<MeshRenderer> _meshRenderer;

	bool _fullScreen = false;
	bool _quit = false;
};

