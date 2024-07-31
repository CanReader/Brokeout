#include "Game.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "Shader.h"

#include "models/Ball.h"
#include "models/Brick.h"
#include "models/Player.h"
#include "models/GameObject.h"
#include "models/Sprite.h"

bool updateView;

// screen dimensions
int screenWidth = 1270;
int screenHeight = 720;

bool stuckToPaddle;
float offset = 0;

int score;

// bricks
const unsigned numbBricksHigh = 5;
const unsigned numbBricksWide = 10;
const unsigned boundBlocks = 20;
const unsigned topBlocks = 25;
std::unique_ptr<Brick> bricks[numbBricksHigh][numbBricksWide];
std::unique_ptr<Brick> boundLeft[boundBlocks];
std::unique_ptr<Brick> boundRight[boundBlocks];
std::unique_ptr<Brick> boundTop[topBlocks];

std::vector<std::unique_ptr<Texture>> scoreText;
std::vector<std::unique_ptr<Sprite>> scoreObject;

glm::mat4 orthoProgMatrix;
glm::mat4 orthoViewMatrix;
glm::mat4 modelTranslate;
glm::mat4 modelScale;
glm::mat4 modelRotation;

bool finishGame;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Game::Game() : camera(std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 10.0f))) 
{
}

void Game::Run()
{
	if (!std::filesystem::exists("res")) {
		MessageBox(NULL,"The resource folder does not exists in app folder!","ERROR",MB_OK);
		throw new std::runtime_error("The resource folder does not exists in app folder! Please move it from project folder..");
	}

	if (!glfwInit())
		return;

	window = glfwCreateWindow(screenWidth, screenHeight, "Brokeout", NULL, NULL);
	
	if (!window)
	{
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glViewport(0, 0, screenWidth, screenHeight);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Init();

	while (!glfwWindowShouldClose(window))
	{
		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera->UpdateVectors();

		Update(deltaTime);

		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void Game::Init()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	
	state = GameState::Play;
	finishGame = false;

	updateView = false;

	stuckToPaddle = true;

	score = 0;

	shader = std::make_unique<Shader>("res\\projection.vert.glsl", "res\\projection.frag.glsl");
	spriteShader = std::make_unique<Shader>("res\\spriteProjection.vert.glsl", "res\\spriteProjection.frag.glsl");
	
	{
		background = std::make_unique<GameObject>();
		background->loadASSIMP("res\\mesh\\backg.obj");
		background->setBuffers();

		background->texture.Load("res\\content\\skymap.png");
		
		background->position = glm::vec3(0.0f);
		background->scale = glm::vec3(100.0f);
	}

	{
		player = std::make_unique<Player>();
		player->loadASSIMP("res\\mesh\\player.obj");
		player->setBuffers();

		player->position = glm::vec3(0.0f, -9.5f, 0.0f);
		player->scale = glm::vec3(1.5f, 0.125f, 0.5f);
		
		offset = player->scale.x;

		player->texture.Load("res\\content\\player.png");
	}

	{
		ball = std::make_unique<Ball>();
		ball->loadASSIMP("res\\mesh\\sphere.obj");
		ball->setBuffers();

		ball->position = glm::vec3
		(
			player->position.x,
			player->position.y + ball->scale.y,
			player->position.z
		);
		
		ball->scale = glm::vec3(0.1f, 0.1f, 0.1f);

		ball->texture.Load("res\\content\\ball.png");
	}

	{
		BuildLevel();
	}

	{
		lives = std::make_unique<Sprite>();
		lives->SetBuffers();
		
		lives->scale = glm::vec3(30.0f, 30.0f, 1.0f);
		lives->position = glm::vec3
		(
			30.0f,
			(GLfloat)screenHeight - lives->scale.y - 5.0f,
			0.0f
		);

		lives->texture.Load("res\\content\\heart.png");
	}

	{
		win = std::make_unique<Sprite>();
		win->SetBuffers();
		
		win->scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		win->position = glm::vec3
		(
			screenWidth / 4,
			screenHeight / 4,
			0.0f
		);
		
		win->texture.Load("res\\content\\reward.png");
		win->active = false;
	}
	
	{
		gameover = std::make_unique<Sprite>();
		gameover->SetBuffers();
		
		gameover->scale = glm::vec3(screenWidth / 2, screenHeight / 2, 1.0f);
		gameover->position = glm::vec3
		(
			screenWidth / 4,
			screenHeight / 4,
			0.0f
		);
		
		gameover->texture.Load("res\\content\\punish.png");
		gameover->active = false;
	}

	LoadScore();

	glEnable(GL_DEPTH_TEST);
}

void Game::Update(float dt)
{
	orthoViewMatrix = glm::mat4(1.0f);
	
	orthoProgMatrix = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
	
	spriteShader->use();
	spriteShader->setFloatMat4("uView", orthoViewMatrix);
	spriteShader->setFloatMat4("uProjection", orthoProgMatrix);
	spriteShader->unuse();
	
	if (state == GameState::Play)
	{
		finishGame = IsGameFinished();
		
		if (finishGame)
		{
			state = GameState::Win;
		}

		UpdatePlayerPosition();

		UpdateBallPosition();
	}
	else if (state == GameState::Win)
	{
		win->active = true;
	}
	else if (state == GameState::Lose)
	{
		gameover->active = true;
	}
}

void Game::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->use();
	
	glm::mat4 rotationMat(1);
	rotationMat = glm::rotate(rotationMat, _lightRotation, glm::vec3(1.0f, 1.0f, 0.0f));
	_lightPos = glm::vec3(rotationMat * glm::vec4(_lightPos, 1.0));
	
	ResetMatrices();
	modelTranslate = translate(modelTranslate, glm::vec3(background->position.x, background->position.y, background->position.z));
	modelScale = scale(modelScale, glm::vec3(background->scale.x, background->scale.y, background->scale.z));
	modelRotation = rotate(modelRotation, background->rotation += deltaTime / 8, glm::vec3(0.0f, 1.0f, 0.0f));

	RenderObject(shader, modelTranslate, modelScale, modelRotation, background->colour, background->texture);
	background->render();
	
	ResetMatrices();
	modelTranslate = translate(modelTranslate, glm::vec3(player->position.x, player->position.y, player->position.z));
	modelScale = scale(modelScale, glm::vec3(player->scale.x, player->scale.y, player->scale.z));
	modelRotation = glm::rotate(modelRotation, player->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	RenderObject(shader, modelTranslate, modelRotation, modelScale, player->colour, player->texture);
	player->render();
	
	ResetMatrices();
	modelTranslate = translate(modelTranslate, ball->position);
	modelRotation = glm::rotate(modelRotation, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelScale = scale(modelScale, ball->scale);

	RenderObject(shader, modelTranslate, modelRotation, modelScale, ball->colour, ball->texture);
	ball->render();
	
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			ResetMatrices();
			modelTranslate = translate(modelTranslate, bricks[y][x]->position);
			modelScale = scale(modelScale, bricks[y][x]->scale);
			modelRotation = rotate(modelRotation, bricks[y][x]->rotation += deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
			
			RenderObject(shader, modelTranslate, modelRotation, modelScale, bricks[y][x]->colour, bricks[y][x]->texture);
			bricks[y][x]->render();
		}
	}

	for (int i = 0; i < boundBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundLeft[i]->position);
		modelScale = scale(modelScale, boundLeft[i]->scale);
		modelRotation = rotate(modelRotation, boundLeft[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(shader, modelTranslate, modelRotation, modelScale, boundLeft[i]->colour, boundLeft[i]->texture);
		boundLeft[i]->render();
	}

	for (int i = 0; i < boundBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundRight[i]->position);
		modelScale = scale(modelScale, boundRight[i]->scale);
		modelRotation = rotate(modelRotation, boundRight[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(shader, modelTranslate, modelRotation, modelScale, boundRight[i]->colour, boundRight[i]->texture);
		boundRight[i]->render();
	}

	for (int i = 0; i < topBlocks; i++)
	{
		ResetMatrices();
		modelTranslate = translate(modelTranslate, boundTop[i]->position);
		modelScale = scale(modelScale, boundTop[i]->scale);
		modelRotation = rotate(modelRotation, boundTop[i]->rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		RenderObject(shader, modelTranslate, modelRotation, modelScale, boundTop[i]->colour, boundTop[i]->texture);
		boundTop[i]->render();
	}

	shader->unuse();
	
	glDisable(GL_DEPTH_TEST);
	
	spriteShader->use();
	
	{
		for (int i = 0; i < player->lives; i++)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, glm::vec3(lives->position.x + (i * 40.0f), lives->position.y, lives->position.z));
			modelScale = glm::scale(modelScale, lives->scale);

			RenderSprite(spriteShader, modelTranslate, modelScale, lives->colour, lives->texture);
			lives->Render();
		}
	}

	{
		if (win->active)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, win->position);
			modelScale = glm::scale(modelScale, win->scale);

			RenderSprite(spriteShader, modelTranslate, modelScale, win->colour, win->texture);
			win->Render();
		}

		if (gameover->active)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, gameover->position);
			modelScale = glm::scale(modelScale, gameover->scale);
			
			RenderSprite(spriteShader, modelTranslate, modelScale, gameover->colour, gameover->texture);
			gameover->Render();
		}
	}

	{
		for (auto& sprite : scoreObject)
		{
			ResetMatrices();
			
			modelTranslate = glm::translate(modelTranslate, sprite->position);
			modelScale = glm::scale(modelScale, sprite->scale);

			RenderSprite(spriteShader, modelTranslate, modelScale, sprite->colour, sprite->texture);
			sprite->Render();
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, 36);

	spriteShader->unuse();
}

void Game::BuildLevel()
{
	auto brickTexture = std::make_unique<Texture>();
	brickTexture->Load("res\\content\\blocks\\brick_block.png");

	auto grassTexture = std::make_unique<Texture>();
	grassTexture->Load("res\\content\\blocks\\grass_block.png");

	auto cobbleTexture = std::make_unique<Texture>();
	cobbleTexture->Load("res\\content\\blocks\\cobble_block.png");

	auto ironTexture = std::make_unique<Texture>();
	ironTexture->Load("res\\content\\blocks\\iron_block.png");

	auto goldTexture = std::make_unique<Texture>();
	goldTexture->Load("res\\content\\blocks\\gold_block.png");

	auto emeraldTexture = std::make_unique<Texture>();
	emeraldTexture->Load("res\\content\\blocks\\emerald_block.png");

	auto diamondTexture = std::make_unique<Texture>();
	diamondTexture->Load("res\\content\\blocks\\diamond_block.png");

	auto crackedTexture = std::make_unique<Texture>();
	crackedTexture->Load("res\\content\\crack.png");
	
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			_brick = std::make_unique<Brick>();
			_brick->loadASSIMP("res\\mesh\\cube.obj");
			_brick->setBuffers();

			_brick->scale = glm::vec3(0.5f, 0.5f, 0.5f);
			_brick->position = (glm::vec3(-9.0f + (2.0f * x), (2.0f * y), 0.0f));


			_brick->texture = 
				y == 0 ? *std::move(grassTexture) :
				y == 1 ? *std::move(cobbleTexture) :
				y == 2 ? *std::move(ironTexture) :
				y == 3 ? *std::move(goldTexture) :
				y == 4 ? *std::move(diamondTexture) :
				*std::move(emeraldTexture);

			_brick->cracked = *std::move(crackedTexture);

			bricks[y][x] = std::move(_brick);

		}
	}

	for (int i = 0; i < boundBlocks; i++)
	{
		_brickLeft = std::make_unique<Brick>();
		_brickLeft->loadASSIMP("res\\mesh\\cube.obj");
		_brickLeft->setBuffers();

		_brickLeft->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickLeft->position = (glm::vec3(-12.0f, -10.0f + i, 0.0f));

		_brickLeft->texture = *std::move(brickTexture);

		boundLeft[i] = std::move(_brickLeft);
	}

	for (int i = 0; i < topBlocks; i++)
	{
		_brickTop = std::make_unique<Brick>();
		_brickTop->loadASSIMP("res\\mesh\\cube.obj");
		_brickTop->setBuffers();

		_brickTop->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickTop->position = (glm::vec3(-12.0f + i, 10.0f, 0.0f));

		_brickTop->texture = *std::move(brickTexture);

		boundTop[i] = std::move(_brickTop);
	}
	
	for (int i = 0; i < boundBlocks; i++)
	{
		_brickRight = std::make_unique<Brick>();
		_brickRight->loadASSIMP("res\\mesh\\cube.obj");
		_brickRight->setBuffers();

		_brickRight->scale = glm::vec3(0.5f, 0.5f, 0.5f);
		_brickRight->position = (glm::vec3(12.0f, -10.0f + i, 0.0f));

		_brickRight->texture = *std::move(brickTexture);

		boundRight[i] = std::move(_brickRight);
	}
}

void Game::UpdateCameraView()
{
	updateView = false;
}

bool Game::IsGameFinished()
{
	for (int y = 0; y < numbBricksHigh; y++)
	{
		for (int x = 0; x < numbBricksWide; x++)
		{
			if (bricks[y][x]->brickAlive)
			{
				return !bricks[y][x]->brickAlive;
			}
		}
	}

	return true;
}

void Game::UpdatePlayerPosition()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		state = GameState::Exit;
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		stuckToPaddle = false;
	}

	if (player->lives > 0)
	{

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			if (player->position.x > -11.25f + offset)
			{
				player->position.x -= player->velocity.x * deltaTime;
			}
		}
		
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			if (player->position.x < 11.15f - offset)
			{
				player->position.x += player->velocity.x * deltaTime;
			}
		}

		if (stuckToPaddle)
		{
			ball->position = glm::vec3
			(
				player->position.x,
				player->position.y + player->scale.y + (ball->scale.y * 2),
				player->position.z
			);
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else if (player->lives <= 0)
	{
		state = GameState::Lose;
	}
}

void Game::UpdateBallPosition()
{
	if (!stuckToPaddle)
	{
		ball->position.x += ball->velocity.x * deltaTime;
		
		if (ball->position.x <= -11.0f)
		{
			ball->velocity.x = -ball->velocity.x;
			ball->position.x = -11.0f;
		}
		else if (ball->position.x >= 11.0f)
		{
			ball->velocity.x = -ball->velocity.x;
			ball->position.x = 11.0f;
		}

		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				if (bricks[y][x]->brickAlive)
				{
					if (CollisionDetection(ball, bricks[y][x]))
					{
						SetCrackedBrick(x, y);
						
						ball->velocity.x = -ball->velocity.x;
						ball->position.x += ball->velocity.x * deltaTime;
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);
				}

				if (bricks[y][x]->position.y < -15.0f)
				{
					bricks[y][x]->brickDying = false;
				}
			}
		}

		ball->position.y += ball->velocity.y * deltaTime;

		if (ball->position.y >= 9.0f)
		{
			ball->velocity.y = -ball->velocity.y;
			ball->position.y = 9.0f;
		}
		else if (ball->position.y <= -15.0f)
		{
			player->lives--;
			stuckToPaddle = true;
		}
		else if (CollisionDetection(ball, player))
		{
			ball->velocity.y = abs(ball->velocity.y);
		}

		for (int y = 0; y < numbBricksHigh; y++)
		{
			for (int x = 0; x < numbBricksWide; x++)
			{
				if (bricks[y][x]->brickAlive)
				{
					if (CollisionDetection(ball, bricks[y][x]))
					{
						SetCrackedBrick(x, y);
						
						ball->velocity.y = -ball->velocity.y;
						ball->position.y += ball->velocity.y * deltaTime;
					}
					
					if (bricks[y][x]->hits < 0)
					{
						SetDeadBrick(x, y);
					}
				}
				
				if (bricks[y][x]->brickDying)
				{
					SetDyingBrick(x, y);
				}

				if (bricks[y][x]->position.y < -15.0f)
				{
					bricks[y][x]->brickDying = false;
				}
			}
		}

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

bool Game::CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Brick>& brick)
{
	float ballLeft = ball->position.x - ball->scale.x;
	float ballRight = ball->position.x + ball->scale.x;
	float ballTop = ball->position.y - ball->scale.x;
	float ballBottom = ball->position.y + ball->scale.x;

	float brickLeft = brick->position.x - brick->scale.x;
	float brickRight = brick->position.x + brick->scale.x;
	float brickTop = brick->position.y - brick->scale.x;
	float brickBottom = brick->position.y + brick->scale.x;

	if (ballBottom <= brickTop) return false;
	if (ballTop >= brickBottom) return false;
	if (ballRight <= brickLeft) return false;
	if (ballLeft >= brickRight) return false;

	return true;
}

bool Game::CollisionDetection(std::unique_ptr<Ball>& ball, std::unique_ptr<Player>& player)
{
	float ballLeft = ball->position.x - ball->scale.x;
	float ballRight = ball->position.x + ball->scale.x;
	float ballTop = ball->position.y - ball->scale.x;
	float ballBottom = ball->position.y + ball->scale.x;

	float playerLeft = player->position.x - player->scale.x;
	float playerRight = player->position.x + player->scale.x;
	float playerTop = player->position.y - player->scale.y;
	float playerBottom = player->position.y + player->scale.y;

	if (ballBottom <= playerTop) return false;
	if (ballTop >= playerBottom) return false;
	if (ballRight <= playerLeft) return false;
	if (ballLeft >= playerRight) return false;

	return true;
}

void Game::SetCrackedBrick(const int x, const int y)
{
	bricks[y][x]->hits -= 1;
	bricks[y][x]->texture =  bricks[y][x]->cracked;
	
	score += 1;
	SetScore();
}

void Game::SetDeadBrick(const int x, const int y)
{
	bricks[y][x]->brickDying = true;
	bricks[y][x]->brickAlive = false;
	
	score += 3;
	SetScore();
}

void Game::SetDyingBrick(const int x, const int y)
{
	bricks[y][x]->position.y -= 9.5f * deltaTime;
	bricks[y][x]->rotation += 0.075f;
	
	if (bricks[y][x]->scale.x > 0.0f)
	{
		bricks[y][x]->scale -= 0.75f * deltaTime;
	}
}

void Game::RenderSprite(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 scale, glm::vec3 colour, Texture& texture)
{
	shader->setFloatMat4("uModel", glm::mat4(translation * scale));
	shader->setFloatMat4("uView", glm::mat4(orthoViewMatrix));
	shader->setFloatMat4("uProjection", glm::mat4(orthoProgMatrix));
	shader->setFloat3("uColour", glm::vec3(colour.x, colour.y, colour.z));

	glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
}

void Game::RenderObject(std::unique_ptr<Shader>& shader, glm::mat4 translation, glm::mat4 rotation, glm::mat4 scale, glm::vec3 colour, Texture& texture)
{
	shader->setFloat3("uObjectColour", glm::vec3(colour.x, colour.y, colour.z));
	shader->setFloat3("uLightColour", glm::vec3(_lightColour.x, _lightColour.y, _lightColour.z));
	shader->setFloat3("uLightPosition", glm::vec3(_lightPos.x, _lightPos.y, _lightPos.z));
	shader->setFloat3("uViewPosition", glm::vec3(camera->Position.x, camera->Position.y, camera->Position.z));

	shader->setFloatMat4("uModel", glm::mat4(translation * rotation * scale));
	shader->setFloatMat4("uView", glm::mat4(camera->GetViewMatrix()));
	shader->setFloatMat4("uProjection", glm::mat4(glm::perspective(glm::radians(90.0f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f)));

	glBindTexture(GL_TEXTURE_2D, texture.GetTexture());
}

void Game::ResetMatrices()
{
	modelTranslate = glm::mat4(1.0f);
	modelScale = glm::mat4(1.0f);
	modelRotation = glm::mat4(1.0f);
}

void Game::LoadScore()
{
	const glm::vec3 scale = glm::vec3(40.0f, 40.0f, 1.0f);

	for (int i = 0; i < 4; i++)
	{
		auto sprite = std::make_unique<Sprite>();
		sprite->SetBuffers();

		if (i == 0)
		{
			sprite->scale = glm::vec3(160.0f, 50.0f, 1.0f);
			sprite->position = glm::vec3
			(
				(GLfloat)screenWidth - 280.0f,
				(GLfloat)screenHeight - sprite->scale.y - (-15.0f),
				0.0f
			);
			
			sprite->texture.Load("res\\content\\score_text.png");
		}
		else
		{
			sprite->scale = scale;
			sprite->position = glm::vec3
			(
				(GLfloat)screenWidth - 140.0f + (i * 30.0f),
				(GLfloat)screenHeight - sprite->scale.y - (-5.0f),
				0.0f
			);
			
			sprite->texture.Load("res\\content\\0.png");
		}
		
		scoreObject.push_back(std::move(sprite));
	}

	for (int i = 0; i < 10; i++)
	{
		auto texture = std::make_unique<Texture>();
		auto file = "res\\content\\" + std::to_string(i) + ".png";
		
		texture->Load(file);
		
		scoreText.push_back(std::move(texture));
	}
}

void Game::SetScore()
{
	std::string scoreStr = std::to_string(score);

	int pos = static_cast<int>(scoreObject.size()) - 1;

	for (int i = (int)scoreStr.length() - 1; i >= 0; i--)
	{
		int value = scoreStr[i] - '0';
		scoreObject[pos]->texture = std::move(*scoreText[value]);
		pos--;
	}
}