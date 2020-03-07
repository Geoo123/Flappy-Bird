#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

class Tema1 : public SimpleScene
{
	public:
		Tema1();
		~Tema1();

		void Init() override;

	private:
		Mesh* CreateMesh(const char *name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		glm::mat3 modelMatrix;
		float scaleX, scaleY;
		float angularStep;
		float BpositionY;
		float BpositionX;
		int aux;
		int cadere;
		int wingpozX;
		int wingpozY; 
		int scor;
		int height_ground;
		int start_game;
		float fGravity;
		float BirdVelocity;
		float BirdAcceleration;
		int sign;
		int dimension;
		float BirdRotation;
		int poz;
		float pozAvion;
		int zi;
		int BirdType;
		float wingsFlip;
		int heartAppear;
		int vieti_extra;
		int collision;
		float initialPozY;
		float heart_collision;
		int heart_collected;
		int game_over;
		int pipeSpeed;
		int modeAvion;
		int height_plane;
		int pipePassed;
		int minim_distance;
		float r, g, b;
};
