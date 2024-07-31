#pragma once

#include <glm/glm.hpp>


#include "Shader.h"
#include "VertexArray.h"

#include "Camera.h"
#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Model.h"
#include "models/Player.h"
#include "models/Sprite.h"
#include "models/GameObject.h"

enum class GameState { Play, Win, Lose, Exit };

struct GLFWwindow;

class Game
{
public:
	Game();
	void Run();

private:
	void Init();
	void Update(float dt);
	void Render();

	void BuildLevel();
	void UpdateCameraView();
	bool IsGameFinished();
	
	void UpdatePlayerPosition();
	void UpdateBallPosition();
	
	bool CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Brick>& brick);
	bool CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Player>& player);

	void SetCrackedBrick(int x, int y);
	void SetDeadBrick(int x, int y);
	void SetDyingBrick(int x, int y);

	void LoadScore();
	void SetScore();

	void RenderObject(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 rotation, glm::mat4 scale, glm::vec3 colour, Texture& texture);
	void RenderSprite(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 scale, glm::vec3 colour, Texture& texture);
	void ResetMatrices();
	
	std::string resDir;

	bool finishGame;

	GameState state;

	std::unique_ptr<Camera> camera;
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Shader> spriteShader;

	std::unique_ptr<GameObject> background;
	std::unique_ptr<Player> player;
	std::unique_ptr<Sprite> lives;
	std::unique_ptr<Sprite> win;
	std::unique_ptr<Sprite> gameover;
	std::unique_ptr<Ball> ball;
	
	std::unique_ptr<Brick> _brick;
	std::unique_ptr<Brick> _brickLeft;
	std::unique_ptr<Brick> _brickRight;
	std::unique_ptr<Brick> _brickTop;

	GLFWwindow* window;

	glm::vec3 _lightPos = glm::vec3(30.0f, 30.0f, 30.0f);
	const glm::vec3 _lightColour = glm::vec3(0.8f, 0.9f, 0.8f);
	const GLfloat _lightRotation = -0.001f;
};