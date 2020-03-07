#include "Tema1.h"

#include <vector>
#include <iostream>
#include <time.h>
#include <ctime>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <Core/Engine.h>
#include <Core/Window/InputController.h>
#include "Transform2D.h"
#include "Object2D.h"


using namespace std;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

vector<Mesh*> obstacles;
vector<VertexFormat> vertices_c;
vector<unsigned short> indices_c;
int low[5];
int distances[5];
int hole[5];
int clouds_distances[10];
int clouds_heights[10];
int cloud_types[10];
int aux;

//deseneaza un cerc plin
vector<VertexFormat> drawCircle (float r, float g, float b) {
	vector<VertexFormat> v;
	v.push_back(VertexFormat(glm::vec3(0, 0,  0), glm::vec3(r, g, b)));
	for (int i = 0; i < 360; i++) {
		v.push_back(VertexFormat(glm::vec3(20 * cos(i), 20 * sin(i),  0), glm::vec3(r, g, b)));
	}
	return v;
}


Mesh* Tema1::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
{
	unsigned int VAO = 0;
	// TODO: Create the VAO and bind it
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// TODO: Create the VBO and bind it
	unsigned int VBO = 0;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	// TODO: Send vertices data into the VBO buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// TODO: Crete the IBO and bind it
	unsigned int IBO = 0;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	
	// TODO: Send indices data into the IBO buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// set vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

	// set vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

	// set texture coordinate attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

	// set vertex color attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	// ========================================================================

	// TODO: Unbind the VAO
	glDeleteBuffers(1, &VAO);

	// Check for OpenGL errors
	CheckOpenGLError();

	// Mesh information is saved into a Mesh object
	meshes[name] = new Mesh(name);
	meshes[name]->InitFromBuffer(VAO, static_cast<unsigned short>(indices.size()));
	return meshes[name];
}

//determina cea mai apropiata teava spre care se indreapta pasarea
int nextPipe() {
	for (int i = 0; i < 5; i++) {
		if (distances[i] < 215)
			return i;
	}
	return -1;
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	start_game = 0;

	// compute coordinates of square center
	float cx = corner.x + squareSide / 2;
	float cy = corner.y + squareSide / 2;

	
	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// initializations
	angularStep = 0;
	BirdRotation = 0;
	BpositionY = 300;
	initialPozY = BpositionY;
	BpositionX = -25;
	scor = 0;
	vieti_extra = 0;
	cout << "=====================================================";
	cout << endl << "INSTRUCTIONS:" << endl;
	cout << "Press LEFT and/or RIGHT arrows to change bird's color." << endl;
	cout << "Press ENTER to start/resume game." << endl;
	cout << "Press P to pause game." << endl;
	cout << "Press SPACE to fly." << endl;
	cout << "Press ESC to exit the game." << endl;
	cout << "Dont't touch buildings or ground." << endl;
	cout << "Collect hearts to get extra lives." << endl;
	cout << "In case you die press ENTER to revive." << endl;
	cout << "=====================================================" << endl;
	cout << endl << "New Game!" << endl << "Your score: " << scor << endl;	
	height_ground = 60;
	BirdType = 0;
	pozAvion = resolution.x;
	wingsFlip = 0;
	heartAppear = 0;
	heart_collected = 0;
	collision = 0;
	game_over = 0;
	pipeSpeed = 180;
	modeAvion = 0;
	minim_distance = 400;
	start_game = 0;
	zi = 1;
	sign = 1;
	dimension = 0;
	fGravity = -100.0f;
	BirdVelocity = 0.0f;
	BirdAcceleration = 0.0f;
	wingpozX = 75;
	wingpozY = 5;
	aux = 1;
	cadere = 1;
	r = 0.6;
	g = 0.8;
	b = 1;

	//patrat rosu
	Mesh* redSquare = Object2D::CreateSquare("redSquare", corner, squareSide / 10, glm::vec3(0.93, 0.16, 0.10), true);
	AddMeshToList(redSquare);

	//patrat galben
	Mesh* yellowSquare = Object2D::CreateSquare("yellowSquare", corner, squareSide / 10, glm::vec3(0.95, 0.83, 0), true);
	AddMeshToList(yellowSquare);

	//patrat negru
	Mesh* blackSquare = Object2D::CreateSquare("blackSquare", corner, squareSide / 10, glm::vec3(0, 0, 0), true);
	AddMeshToList(blackSquare);

	//patrat bleu
	Mesh* lightBlueSquare = Object2D::CreateSquare("lightBlueSquare", corner, squareSide / 10, glm::vec3(0.27, 0.59, 0.96), true);
	AddMeshToList(lightBlueSquare);

	//patrat maro
	Mesh* brownSquare = Object2D::CreateSquare("brownSquare", corner, squareSide / 10, glm::vec3(0.15, 0.04, 0.04), true);
	AddMeshToList(brownSquare);

	//patrat alb
	Mesh* whiteSquare = Object2D::CreateSquare("whiteSquare", corner, squareSide / 10, glm::vec3(1, 1, 1), true);
	AddMeshToList(whiteSquare);

	//patrat visiniu
	Mesh* brick = Object2D::CreateSquare("brick", corner, squareSide / 10, glm::vec3(0.68, 0, 0), true);
	AddMeshToList(brick);

	//patrat rozaliu
	Mesh* squareGround = Object2D::CreateSquare("squareGround", corner, squareSide / 10, glm::vec3(1, 0.8, 0.6), true);
	AddMeshToList(squareGround);

	//patrat verde
	Mesh* squareGrass = Object2D::CreateSquare("squareGrass", corner, squareSide / 10, glm::vec3(0.4, 0.6, 0), true);
	AddMeshToList(squareGrass);

	//kind of purple
	Mesh* purpleSquare = Object2D::CreateSquare("purpleSquare", corner, squareSide / 10, glm::vec3(0.4, 0.4, 0.6), true);
	AddMeshToList(purpleSquare);

	//kind of pink
	Mesh* pinkSquare = Object2D::CreateSquare("pinkSquare", corner, squareSide / 10, glm::vec3(1, 0.6, 0.4), true);
	AddMeshToList(pinkSquare);

	//dark blue
	Mesh* darkBlueSquare = Object2D::CreateSquare("darkBlueSquare", corner, squareSide / 10, glm::vec3(0, 0, 0.4), true);
	AddMeshToList(darkBlueSquare);

	//lemon square
	Mesh* lemonSquare = Object2D::CreateSquare("lemonSquare", corner, squareSide / 10, glm::vec3(0.8, 1, 0.2), true);
	AddMeshToList(lemonSquare);

	//light green
	Mesh* lightGreenSquare = Object2D::CreateSquare("lightGreenSquare", corner, squareSide / 10, glm::vec3(0.6, 1, 0.4), true);
	AddMeshToList(lightGreenSquare);

	//culorile pentru tevi
	obstacles.push_back(lightGreenSquare);
	obstacles.push_back(redSquare);
	obstacles.push_back(purpleSquare);
	obstacles.push_back(pinkSquare);
	obstacles.push_back(lemonSquare);

	//stele
	vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0.93f, 0.21)),
		VertexFormat(glm::vec3(2, 0,  0), glm::vec3(1, 0.93f, 0.21)),
		VertexFormat(glm::vec3(1, 1.73,  0), glm::vec3(1, 0.93f, 0.21))

	};

	//triunghi inima
	vector<VertexFormat> vertices_inima
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0, 0)),
		VertexFormat(glm::vec3(2, 0,  0), glm::vec3(1, 0, 0)),
		VertexFormat(glm::vec3(1, -1.73,  0), glm::vec3(1, 0, 0))

	};

	//triunghi avion
	vector<VertexFormat> vertices_plane
	{
		VertexFormat(glm::vec3(0, 0.25,  0), glm::vec3(0.94, 0.94, 0.94)),
		VertexFormat(glm::vec3(0.5, 0,  0), glm::vec3(0.94, 0.94, 0.94)),
		VertexFormat(glm::vec3(1, 1,  0), glm::vec3(0.94, 0.94, 0.94))

	};

	//raze
	vector<VertexFormat> vertices_r
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(2, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(1, 1.73,  0), glm::vec3(1, 0.48f, 0)),

	};

	//raze
	vector<VertexFormat> vertices_r2
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(2, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(1, -1.73,  0), glm::vec3(1, 0.48f, 0)),

	};

	//aripa verde
	vector<VertexFormat> vertices2
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0, 0.37f, 0)),
		VertexFormat(glm::vec3(0, 1,  0), glm::vec3(0, 0.37f, 0)),
		VertexFormat(glm::vec3(1, 0.5,  0), glm::vec3(0, 0.37f, 0)),

	};

	//aripa mov
	vector<VertexFormat> vertices2bis
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0.33, 0, 0.22)),
		VertexFormat(glm::vec3(0, 1,  0), glm::vec3(0.33, 0, 0.22)),
		VertexFormat(glm::vec3(1, 0.5,  0), glm::vec3(0.33, 0, 0.22)),

	};

	//aripa albastra
	vector<VertexFormat> vertices3bis
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0.2, 0.8, 0.8)),
		VertexFormat(glm::vec3(0, 1,  0), glm::vec3(0.2, 0.8, 0.8)),
		VertexFormat(glm::vec3(1, 0.5,  0), glm::vec3(0.2, 0.8, 0.8)),

	};
 
 	//tufis
	vector<VertexFormat> vertices3
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0, 0.4f, 0)),
		VertexFormat(glm::vec3(0.5, 1,  0), glm::vec3(0, 0.4f, 0)),
		VertexFormat(glm::vec3(1, 0,  0), glm::vec3(0, 0.4f, 0)),
		

	};

	//partea superioara a ciocului
	vector<VertexFormat> vertices_c
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(2, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(0, 1,  0), glm::vec3(1, 0.48f, 0)),

	};

	//partea inferioara a ciocului
	vector<VertexFormat> vertices_c2
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0.83, 0.37f, 0)),
		VertexFormat(glm::vec3(1, 0,  0), glm::vec3(0.83, 0.37f, 0)),
		VertexFormat(glm::vec3(0, -0.5,  0), glm::vec3(0.83, 0.37f, 0)),

	};

	vector<unsigned short> indices {
		0, 1, 2
	};

	//cerc
	for (int i = 1; i < 360; i++) {
		indices_c.push_back(0);
		indices_c.push_back(i);
		indices_c.push_back(i + 1);
	}


	Mesh* raze = CreateMesh("raze", vertices_r, indices);
	Mesh* raze2 = CreateMesh("raze2", vertices_r2, indices);
	Mesh* stele = CreateMesh("stele", vertices, indices);
	Mesh* coada_avion = CreateMesh("coada_avion", vertices_plane, indices);
	Mesh* aripa_avion = CreateMesh("aripa_avion", drawCircle(0.72, 0.72, 0.72), indices_c);
	Mesh* aripa = CreateMesh("aripa", vertices2, indices);
	Mesh* aripa2 = CreateMesh("aripa2", vertices2bis, indices);
	Mesh* aripa3 = CreateMesh("aripa3", vertices3bis, indices);
	Mesh* tufis = CreateMesh("tufis", vertices3, indices);
	Mesh* triunghi_inima = CreateMesh("triunghi_inima", vertices_inima, indices);
	Mesh* cioc1 = CreateMesh("cioc1", vertices_c, indices);
	Mesh* cioc2 = CreateMesh("cioc2", vertices_c2, indices);
	Mesh* cerc = CreateMesh("cerc", drawCircle(0.007, 0.87, 0.16), indices_c);
	Mesh* cerc_inima = CreateMesh("cerc_inima", drawCircle(1, 0, 0), indices_c);
	Mesh* cerc1 = CreateMesh("cerc1", drawCircle(0.4, 0.4, 0.6), indices_c);
	Mesh* cerc2 = CreateMesh("cerc2", drawCircle(0.07, 0.3, 0.8), indices_c);
	Mesh* cerc_gri = CreateMesh("cerc_gri", drawCircle(0.94, 0.94, 0.94), indices_c);
	Mesh* luna = CreateMesh("luna", drawCircle(0, 0, 0.4), indices_c);
	Mesh* soare = CreateMesh("soare", drawCircle(1, 0.93, 0.21), indices_c);
	Mesh* ochi = CreateMesh("ochi", drawCircle(1, 1, 1), indices_c);
	Mesh* pupila = CreateMesh("pupila", drawCircle(0, 0, 0), indices_c);
	Mesh* copac = CreateMesh("copac", drawCircle(0, 0.4, 0), indices_c);

	//generarea tevilor
	distances[0] = resolution.x;
	for (int i = 0; i < obstacles.size(); i++) {
			low[i] = (rand() % 40) + 1;
			hole[i] = (rand() % 30) + 1;
			int door_height = 30;
			int window_width = 46;
			while (hole[i] < 27) {
				hole[i] = (rand() % 30) + 1;
			}
			while (low[i] < 10 || (hole[i] + low[i]) * 10 + height_ground > 700 || (i > 0 && low[i] - low[i - 1] > 20)) {
				low[i] = rand() % 40 + 1;
			}
			if (i > 0) {
				distances[i] = distances[i - 1] + minim_distance + rand() % 101;
			}
	}

	//generarea norilor
	for (int k = 0; k < 10; k++) {
			clouds_heights[k] = (rand() % 68) + 1;
			clouds_distances[k] = (rand() % resolution.x) + 1;
			cloud_types[k] = rand() % 3;
			int up_down = rand() % 2 + 1;
			while (clouds_heights[k] < 40) {
				clouds_heights[k] = (rand() % 55) + 1;
			}			
	}

	//generearea inaltimii avionului
	height_plane = rand() % 60;
	while (height_plane < 20)
		height_plane = rand() % 60;
}

void Tema1::FrameStart()
{
	glClearColor(r, g, b, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
	
}

void Tema1::Update(float deltaTimeSeconds)
{	
	glm::ivec2 resolution = window->GetResolution();

	angularStep -= deltaTimeSeconds * 0.1;
	wingsFlip += deltaTimeSeconds;
	if (wingsFlip > 1) {
		wingsFlip = 0;
		cadere *= -1;
	}

	//pamant
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Scale(resolution.x / 10, 4);
	RenderMesh2D(meshes["squareGround"], shaders["VertexColor"], modelMatrix);

	//iarba
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 40);
	modelMatrix *= Transform2D::Scale(resolution.x / 10, 2);
	RenderMesh2D(meshes["squareGrass"], shaders["VertexColor"], modelMatrix);
	
	//palarie pasare
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 75, BpositionY + 85);
	modelMatrix *= Transform2D::Scale(4.8, 0.4);
	RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 50, BpositionY + 75);
	modelMatrix *= Transform2D::Scale(9.4, 0.8);
	RenderMesh2D(meshes["yellowSquare"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 75, BpositionY + 80);
	modelMatrix *= Transform2D::Scale(4.8, 2.3);
	RenderMesh2D(meshes["yellowSquare"], shaders["VertexColor"], modelMatrix);

	//nara
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 150, BpositionY + 35);
	modelMatrix *= Transform2D::Scale(0.04, 0.04);
	RenderMesh2D(meshes["pupila"], shaders["VertexColor"], modelMatrix);

	//cioc
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 143, BpositionY + 30);
	modelMatrix *= Transform2D::Scale(14.8, 19.8);
	RenderMesh2D(meshes["cioc1"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 143, BpositionY + 30);
	modelMatrix *= Transform2D::Scale(17.8, 19.8);
	RenderMesh2D(meshes["cioc2"], shaders["VertexColor"], modelMatrix);

	//aripa
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + wingpozX, BpositionY + wingpozY);
	modelMatrix *= Transform2D::Scale(39.8, 39.8);
	modelMatrix *= Transform2D::Translate(0.333, 0.5);
	modelMatrix *= Transform2D::Rotate(40.2);

	//miscarea aripilor
	if (cadere == 1 && game_over == 0) {
		modelMatrix *= Transform2D::OglindireOY();
		wingpozX = 75;
		wingpozY = 5;
	}
	if (cadere == -1 && game_over == 0) {
		modelMatrix *= Transform2D::OglindireOY();
		modelMatrix *= Transform2D::OglindireOY();
		wingpozX = 55;
		wingpozY = 20;
	}
	modelMatrix *= Transform2D::Translate(-0.333, -0.5);
	if (BirdType == 0)
		RenderMesh2D(meshes["aripa"], shaders["VertexColor"], modelMatrix);
	if (BirdType == 1)
		RenderMesh2D(meshes["aripa2"], shaders["VertexColor"], modelMatrix);
	if (BirdType == 2)
		RenderMesh2D(meshes["aripa3"], shaders["VertexColor"], modelMatrix);

	//ochi
	if (game_over == 0) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
		modelMatrix *= Transform2D::Rotate(BirdRotation);
		modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
		modelMatrix *= Transform2D::Translate(BpositionX + 125, BpositionY + 45);
		modelMatrix *= Transform2D::Scale(0.2, 0.25);
		RenderMesh2D(meshes["pupila"], shaders["VertexColor"], modelMatrix);
	}

	if (game_over == 1) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Translate(BpositionX + 125, BpositionY + 50);
		modelMatrix *= Transform2D::Rotate(0.744);
		modelMatrix *= Transform2D::Translate(-(BpositionX + 125), -(BpositionY + 50));
		modelMatrix *= Transform2D::Translate(BpositionX + 125, BpositionY + 45);
		modelMatrix *= Transform2D::Scale(0.25, 1);
		RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Translate(BpositionX + 125, BpositionY + 50);
		modelMatrix *= Transform2D::Rotate(-0.644);
		modelMatrix *= Transform2D::Translate(-(BpositionX + 125), -(BpositionY + 50));
		modelMatrix *= Transform2D::Translate(BpositionX + 124.5, BpositionY + 46);
		modelMatrix *= Transform2D::Scale(0.25, 1);
		RenderMesh2D(meshes["blackSquare"], shaders["VertexColor"], modelMatrix);
	}


	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(0, 0);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 43);
	modelMatrix *= Transform2D::Rotate(BirdRotation);
	modelMatrix *= Transform2D::Translate(-(BpositionX + 100), -(BpositionY + 43));
	modelMatrix *= Transform2D::Translate(BpositionX + 125, BpositionY + 50);
	modelMatrix *= Transform2D::Scale(0.3, 0.4);
	RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

	//corp pasare	
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(BpositionX + 100, BpositionY + 40);
	modelMatrix *= Transform2D::Scale(2.3, 2.3);
	if (BirdType == 0) {
		RenderMesh2D(meshes["cerc"], shaders["VertexColor"], modelMatrix); 
	}

	if (BirdType == 1) {
		RenderMesh2D(meshes["cerc1"], shaders["VertexColor"], modelMatrix); 
	}

	if (BirdType == 2) {
		RenderMesh2D(meshes["cerc2"], shaders["VertexColor"], modelMatrix); 
	}

	if (start_game == 1) {
		//miscarea pasarii
		BirdAcceleration += fGravity * deltaTimeSeconds * 3;
		BirdVelocity += BirdAcceleration * deltaTimeSeconds * 3;
		BpositionY += BirdVelocity * deltaTimeSeconds * 3 ;
		
		//rotatia pasarii
		if (aux == 1)
			BirdRotation = BirdVelocity / 180;
		if (BirdRotation < -0.441002)
			aux = 0;

		int next_pipe = nextPipe();
		float front_collision = distances[next_pipe]; 
		if (next_pipe == 1 && heartAppear == 1)
			heart_collision = front_collision + 45;

		//pasarea loveste pamantul
		if (BpositionY < height_ground && game_over == 0) {
			if (vieti_extra == 0) {
				if (game_over == 0) {
						cout << "Game over! Your score is: " << scor << endl;
					}
				start_game = 0;
				BirdRotation = 0;
				if (BpositionY > height_ground) {
					BpositionY -= deltaTimeSeconds * 50;
				}
				game_over = 1;
			}
			else {
				vieti_extra--;
				collision = 1;
			}
		}
		if (next_pipe != -1) {
			// coliziune cu ciocul
			if (front_collision > BpositionX + 144 && front_collision < BpositionX + 174 && (BpositionY + 43 < low[next_pipe] * 10 + height_ground || BpositionY + 75 > (low[next_pipe] + hole[next_pipe]) * 10 + height_ground)) {
				if (vieti_extra == 0) {
					if (game_over == 0) {
						cout << "Game over! Your score is: " << scor << endl;
					}
					start_game = 0;
					BirdRotation = 0;
					while (BpositionY > height_ground) {
						BpositionY -= deltaTimeSeconds * 50;
					}
					game_over = 1;
				}
				else {
					vieti_extra--;
					collision = 1;
				}
			}

			//coliziune cu burta
			if (front_collision + 100 > BpositionX + 50 && front_collision < BpositionX + 144 && BpositionY < low[next_pipe] * 10 + height_ground) {
				if (vieti_extra == 0) {
					if (game_over == 0) {
						cout << "Game over! Your score is: " << scor << endl;
					}
					start_game = 0;
					BirdRotation = 0;
					while (BpositionY > height_ground) {
						BpositionY -= deltaTimeSeconds * 50;
					}
					game_over = 1;
				}
				else {
					vieti_extra--;
					collision = 1;
				}
			}

			//coliziune cu palaria
			if (front_collision + 100 > BpositionX + 75 && front_collision < BpositionX + 123 && BpositionY + 100 > (low[next_pipe] + hole[next_pipe]) * 10 + height_ground) {
				if (vieti_extra == 0) {
					if (game_over == 0) {
						cout << "Game over! Your score is: " << scor << endl;
					}
					start_game = 0;
					BirdRotation = 0;
					while (BpositionY > height_ground) {
						BpositionY -= deltaTimeSeconds * 50;
					}
					game_over = 1;
					}
				else {
					vieti_extra--;
					collision = 1;
				}
			}

			//coliziune cu varful capului
			if (front_collision + 100 > BpositionX + 50 && front_collision < BpositionX + 144 && BpositionY + 75 > (low[next_pipe] + hole[next_pipe]) * 10 + height_ground) {
				if (vieti_extra == 0) {
					if (game_over == 0) {
						cout << "Game over! Your score is: " << scor << endl;
					}
					start_game = 0;
					BirdRotation = 0;
					while (BpositionY > height_ground) {
						BpositionY -= deltaTimeSeconds * 50;
					}
					game_over = 1;
				}
				else {
					vieti_extra--;
					collision = 1;
				}
			}

			//coliziunea cu inimioara
			if (next_pipe == 1 && heartAppear == 1)
				if (BpositionX + 144 > heart_collision) {
					heartAppear = 0;
					heart_collected = 1;

				}		
		}
	}

	srand(time(NULL));
	for (int i = 0; i < obstacles.size(); i++) {
			//usa
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[i] + 25, 5 + height_ground);
			modelMatrix *= Transform2D::Scale(5, 2);
			RenderMesh2D(meshes["brownSquare"], shaders["VertexColor"], modelMatrix); 

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[i] + 20, 5 + height_ground);
			modelMatrix *= Transform2D::Scale(6, 2.5);
			RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

			int window_width = 46;
			int door_height = 30;
			int nr_of_windows = (float)(low[i] * 10 - door_height) / window_width;
			int initial_height = 41;
			for (int j = 0; j < nr_of_windows; j++) {
				//casute cu gemulete
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 15, initial_height + 12 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(3, 0.4);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 28, initial_height - 1 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(0.4, 3);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 16, initial_height + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(2.8, 2.8);
				RenderMesh2D(meshes["lightBlueSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 14, initial_height - 2 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(3.2, 3.2);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 55, initial_height + 12 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(3, 0.4);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 68, initial_height - 1 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(0.4, 3);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 56, initial_height + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(2.8, 2.8);
				RenderMesh2D(meshes["lightBlueSquare"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(distances[i] + 54, initial_height - 2 + j * window_width + height_ground);
				modelMatrix *= Transform2D::Scale(3.2, 3.2);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);
			}

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[i] - 10, height_ground);
			modelMatrix *= Transform2D::Scale(12, 0.5);
			RenderMesh2D(meshes["brownSquare"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[i], low[i] * 10 + height_ground);
			modelMatrix *= Transform2D::Scale(10, 0.5);
			RenderMesh2D(meshes["brownSquare"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(distances[i], height_ground);
		modelMatrix *= Transform2D::Scale(10, low[i]);

		
		if (start_game == 1) {
			//cresterea vietilor atunci cand o inimioara este colectata
			if (heart_collected == 1) {
					vieti_extra++;
					heart_collected = 0;
			}

			if (modeAvion == 0)
				distances[i] -= deltaTimeSeconds * pipeSpeed;	

			//redesenarea tevilor atunci cand una iese din ecran
			if (distances[0] + 100 < 0) {
				if (distances[0] + 100 < BpositionX + 50)
					scor++;

				if (scor % 25 == 0 && scor > 0) {
					pipeSpeed += 50;
				}

				if (scor % 50 == 0 && scor > 0) {
					minim_distance -= 25;
				}

				cout << "Your score: " << scor << endl;
				distances[0] = distances[4] + minim_distance + rand() % 101;
				low[0] = (rand() % 35) + 1;
				hole[0] = (rand() % 35) + 1;

				while (hole[0] < 27) {
					hole[0] = (rand() % 35) + 1;
				}	

				while (low[i] < 10 || (hole[i] + low[i]) * 10 + height_ground > 700 || low[0] - low[4] > 20) {
					low[i] = rand() % 35 + 1;
				}
						
			}

			if (i > 0 && distances[i] < -100) {
				if (distances[i] + 100 < BpositionX + 50)
					scor++;

				if (scor % 25 == 0 && scor > 0) {
					pipeSpeed += 50;
				}

				if (scor % 50 == 0 && scor > 0) {
					minim_distance -= 25;
				}

				cout << "Your score: " << scor << endl;
				distances[i] = distances[i - 1] + minim_distance + rand() % 101;
				low[i] = (rand() % 35) + 1;
				hole[i] = (rand() % 35) + 1;

				while (hole[i] < 27) {
					hole[i] = (rand() % 35) + 1;
				}	

				while (low[i] < 10 || (hole[i] + low[i]) * 10 + height_ground > 700 || low[i] - low[i - 1] > 20) {
					low[i] = rand() % 35 + 1;
				}
			}
			
		}
		
		RenderMesh2D(obstacles[i], shaders["VertexColor"], modelMatrix);

		//caramizi pentru pipe-ul de sus
		int nr_of_bricks = (resolution.y - height_ground - low[i] - hole[i] * 10) / 20;
			for (int q = 0; q < nr_of_bricks; q++) {
				modelMatrix = glm::mat3(1);
				if (q % 2 == 0)
					modelMatrix *= Transform2D::Translate(distances[i], (low[i] + hole[i]) * 10 + height_ground + q * 20);
				else 
					modelMatrix *= Transform2D::Translate(distances[i] + 50, (low[i] + hole[i]) * 10 + height_ground + q * 20);
				modelMatrix *= Transform2D::Scale(5, 2);
				RenderMesh2D(meshes["brick"], shaders["VertexColor"], modelMatrix);
			}

		//pipe-ul de sus
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(distances[i], (low[i] + hole[i]) * 10 + height_ground);
		modelMatrix *= Transform2D::Scale(10, (resolution.y - height_ground - low[i] * 10 - hole[i] * 10) / 10);
		RenderMesh2D(obstacles[i], shaders["VertexColor"], modelMatrix);


		//aparitia inimioarei pe ecran
		if (scor % 25 == 0 && scor > 0) {
			heartAppear = 1;
		}						
		if (heartAppear == 1) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[1] + 45, height_ground + low[1] * 10 + hole[1] * 10 / 2);
			modelMatrix *= Transform2D::Scale(0.5, 0.5);
			RenderMesh2D(meshes["cerc_inima"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[1] + 65, height_ground + low[1] * 10 + hole[1] * 10 / 2);
			modelMatrix *= Transform2D::Scale(0.5, 0.5);
			RenderMesh2D(meshes["cerc_inima"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(distances[1] + 33.5, height_ground + low[1] * 10 + hole[1] * 10 / 2);
			modelMatrix *= Transform2D::Scale(21.5, 15);
			RenderMesh2D(meshes["triunghi_inima"], shaders["VertexColor"], modelMatrix);	
		}
				
		
	}

	//inima pentru vieti extra
	for (int j = 0; j < vieti_extra; j++) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(50 * j + 25, 700);
				modelMatrix *= Transform2D::Scale(0.5, 0.5);
				RenderMesh2D(meshes["cerc_inima"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(50 * j + 45, 700);
				modelMatrix *= Transform2D::Scale(0.5, 0.5);
				RenderMesh2D(meshes["cerc_inima"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(50 * j + 13.5, 700);
				modelMatrix *= Transform2D::Scale(21.5, 15);
				RenderMesh2D(meshes["triunghi_inima"], shaders["VertexColor"], modelMatrix);	
	}

	
	//copaci pe fundal
	for (int i = 0; i < resolution.x / 100; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(55 + i * 100, 120);
		RenderMesh2D(meshes["copac"], shaders["VertexColor"], modelMatrix);
	}

	//pamant
	for (int i = 0; i < resolution.x / 100; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(50 + i * 100, 60);
		modelMatrix *= Transform2D::Scale(1, 6);
		RenderMesh2D(meshes["brownSquare"], shaders["VertexColor"], modelMatrix);
	}

	//tufisuri
	for (int i = 0; i < resolution.x / 40 + 1; i++) {
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0 + i * 40, 60);
		modelMatrix *= Transform2D::Scale(40, 40);
		RenderMesh2D(meshes["tufis"], shaders["VertexColor"], modelMatrix);
	}

	//cand pierdem o viata
	if (collision == 1) {
		distances[0] = resolution.x;
		for (int i = 0; i < obstacles.size(); i++) {
				low[i] = (rand() % 40) + 1;
				hole[i] = (rand() % 30) + 1;
				int door_height = 30;
				int window_width = 46;
				while (hole[i] < 27) {
					hole[i] = (rand() % 30) + 1;
				}
				while (low[i] < 10 || (hole[i] + low[i]) * 10 + height_ground > 700 || (i > 0 && low[i] - low[i - 1] > 20)) {
					low[i] = rand() % 40 + 1;
				}
				if (i > 0) {
					distances[i] = distances[i - 1] + minim_distance + rand() % 101;
				}
		}
		BpositionY = initialPozY;
		start_game = 0;
		BirdRotation = 0;
		BirdVelocity = 0.0f;
		BirdAcceleration = 0.0f;
		collision = 0;
		game_over = 0;
		fGravity = -100.0f;
		BirdVelocity = 0.0f;
		BirdAcceleration = 0.0f;
		wingpozX = 75;
		wingpozY = 5;
		aux = 1;
		cadere = 1;
		if (scor == 0) {
			cout << "=====================================================";
			cout << endl << "New Game!" << endl << "Your score: " << scor << endl;	
		}

	}

	if (scor > 0) {
		
		pozAvion -= deltaTimeSeconds * 350;

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(pozAvion + 200, height_plane * 10);
		modelMatrix *= Transform2D::Scale(1.6, 0.1);
		RenderMesh2D(meshes["aripa_avion"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(pozAvion + 104, height_plane * 10 + 10);
		modelMatrix *= Transform2D::Scale(1.2, 0.3);
		RenderMesh2D(meshes["cerc2"], shaders["VertexColor"], modelMatrix);	

		for (int k = 0; k < 7; k++) {
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(pozAvion + 150 + k * 20, height_plane * 10 + 15);
			modelMatrix *= Transform2D::Scale(0.3, 0.3);
			RenderMesh2D(meshes["cerc2"], shaders["VertexColor"], modelMatrix);	
		}
		
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(pozAvion + 200, height_plane * 10 + 10);
		modelMatrix *= Transform2D::Scale(6, 1);
		RenderMesh2D(meshes["cerc_gri"], shaders["VertexColor"], modelMatrix);

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(pozAvion + 300, height_plane * 10 + 10);
		modelMatrix *= Transform2D::Scale(40, 40);
		RenderMesh2D(meshes["coada_avion"], shaders ["VertexColor"], modelMatrix);


	}
	if (pozAvion + 300 < 0) {
		modeAvion = 0;
	}

	if (pozAvion + 4000 < 0) {
		pozAvion = resolution.x;
		height_plane = rand() % 60;
		while (height_plane < 20)
			height_plane = rand() % 60;
	}

	//cer bleu (zi)
	if (zi == 1) {
		r = 0.6;
		g = 0.8;
		b = 1;

		for (int k = 0; k < 10; k++) {

			//tipul 1 de nor
			if (cloud_types[k] == 0) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k], clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 30, clouds_heights[k] * 10 + 5);
				modelMatrix *= Transform2D::Scale(1.5, 1.5);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);
				

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 60, clouds_heights[k] * 10 - 5);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);
			}

			//tipul 2 de nor
			if (cloud_types[k] == 1) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k], clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 35, clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 70, clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 15, clouds_heights[k] * 10 + 25);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 50, clouds_heights[k] * 10 + 25);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k], clouds_heights[k] * 10 - 20);
				modelMatrix *= Transform2D::Scale(7, 1);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);
			}

			//tipul 3 de nor
			if (cloud_types[k] == 2) {
				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k], clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 40, clouds_heights[k] * 10 + 10);
				modelMatrix *= Transform2D::Scale(1.5, 1.5);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 80, clouds_heights[k] * 10 + 10);
				modelMatrix *= Transform2D::Scale(1.3, 1.2);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k] + 115, clouds_heights[k] * 10);
				RenderMesh2D(meshes["ochi"], shaders["VertexColor"], modelMatrix);

				modelMatrix = glm::mat3(1);
				modelMatrix *= Transform2D::Translate(clouds_distances[k], clouds_heights[k] * 10 - 20);
				modelMatrix *= Transform2D::Scale(12, 1);
				RenderMesh2D(meshes["whiteSquare"], shaders["VertexColor"], modelMatrix);
			}
			if (clouds_distances[k] + 100 > 0) {
				clouds_distances[k] -= deltaTimeSeconds * 120;
			}
			else {
				clouds_distances[k] = resolution.x;
			}
		}

		//soare cu raze
		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Rotate(angularStep);
		modelMatrix *= Transform2D::Translate(77, 650);
		modelMatrix *= Transform2D::Scale(2, 2);
		RenderMesh2D(meshes["soare"], shaders["VertexColor"], modelMatrix);	

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Rotate(angularStep);
		modelMatrix*=Transform2D::Translate(12, 610);
		modelMatrix*=Transform2D::Scale(65, 65);
		RenderMesh2D(meshes["raze"], shaders["VertexColor"], modelMatrix);	

		modelMatrix = glm::mat3(1);
		modelMatrix *= Transform2D::Translate(0, 0);
		modelMatrix *= Transform2D::Rotate(angularStep);
		modelMatrix*=Transform2D::Translate(12, 690);
		modelMatrix*=Transform2D::Scale(65, 65);
		RenderMesh2D(meshes["raze2"], shaders["VertexColor"], modelMatrix);	
	}
	
	//trecerea de la noapte la zi cand luna scade sub un anumit nivel
	if (angularStep < -1.7 && zi == 0) {
		angularStep = 0;
		zi = 1;
	}

	//cer portocaliu (amiaza)
	if (angularStep < -0.7 && zi == 1) {
		r = 0.99;
		g = 0.71;
		b = 0;
	}
	//cer rosu (apus)
	if (angularStep < -1 && zi == 1) {
		r = 1;
		g = 0.36;
		b = 0.2;
	}
	
	//afisare luna si stele
	if (zi == 0) {
			r = g = 0;
			b = 0.4;

			//stele
			for (int z = 1; z <= 5; z++) {
				if (z % 2 == 1) {
					poz = -20;
				}
				else 
					poz = 20;
				for (int s = 0; s < 10; s ++) {
				modelMatrix = glm::mat3(1);	
				modelMatrix *= Transform2D::Translate(96 + z * 200, 600 + poz);
				modelMatrix *= Transform2D::Rotate(0.628f * s);
				modelMatrix *= Transform2D::Translate(-96 - z * 200, -600 - poz);
				if (z % 2 == 0)
					modelMatrix *= Transform2D::Translate(96 + z * 200 - 10, 600 + poz + 10);
				if (z % 2 == 1)
					modelMatrix *= Transform2D::Translate(96 + z * 200, 600 + poz);
				modelMatrix *= Transform2D::Scale(5, 5);
				modelMatrix *= Transform2D::Translate(0, 0);
				modelMatrix *= Transform2D::Rotate(angularStep);
				RenderMesh2D(meshes["stele"], shaders["VertexColor"], modelMatrix);
				}
				
			}
			for (int z = 1; z <= 7; z++) {			
				if (z % 2 == 1) {
					poz = 35;
				}
				else 
					poz = -35;
				for (int s = 0; s < 10; s ++) {
				modelMatrix = glm::mat3(1);	
				modelMatrix *= Transform2D::Translate(6 + z * 160, 400 + poz);
				modelMatrix *= Transform2D::Rotate(0.628f * s);
				modelMatrix *= Transform2D::Translate(-6 - z * 160, -400 - poz);
				if (z % 2 == 0)
					modelMatrix *= Transform2D::Translate(6 + z * 160 - 10, 400 + poz + 10);
				if (z % 2 == 1)
					modelMatrix *= Transform2D::Translate(6 + z * 160, 400 + poz);
				modelMatrix *= Transform2D::Scale(7.5, 7.5);
				modelMatrix *= Transform2D::Translate(0, 0);
				modelMatrix *= Transform2D::Rotate(angularStep);
				RenderMesh2D(meshes["stele"], shaders["VertexColor"], modelMatrix);
				}

				
			}

			//luna
			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(0, 0);
			modelMatrix *= Transform2D::Rotate(angularStep);
			modelMatrix *= Transform2D::Translate(50, 650);
			modelMatrix *= Transform2D::Scale(2, 2);
			RenderMesh2D(meshes["luna"], shaders["VertexColor"], modelMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix *= Transform2D::Translate(0, 0);
			modelMatrix *= Transform2D::Rotate(angularStep);
			modelMatrix *= Transform2D::Translate(70, 650);
			modelMatrix *= Transform2D::Scale(2, 2);
			RenderMesh2D(meshes["soare"], shaders["VertexColor"], modelMatrix);	
	}

	//cer roz (rasarit)
	if (angularStep < -1.5 && zi == 0) {
		r = 1;
		g = 0.8;
		b = 1;
	}

	//trecerea de la zi la noapte cand soarele apune
	if (angularStep < -1.47 && zi == 1) {
		angularStep = 0;
		zi = 0;
			
	}		
}

void Tema1::FrameEnd()
{

}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{

}

void Tema1::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_SPACE )
	{
		if (start_game == 1) {
			BirdAcceleration = 0.0f;
			BirdVelocity = -fGravity;
			aux = 1;
			BirdRotation = BirdVelocity / 180.0f;
		}
	}
	if (key == GLFW_KEY_ENTER)
	{
		start_game = 1;
		if (game_over == 1) {
			scor = 0;
			collision = 1;
		}
	}
	if (key == GLFW_KEY_P)
	{
		start_game = 0;
	}

	if (key == GLFW_KEY_ESCAPE)
	{	
		cout << endl << "Game finished! Your score is: " << scor << endl; 
		exit(0);
	}

	if (key == GLFW_KEY_RIGHT) {
		if (BirdType == 0) {
			BirdType = 1;
		}
		else if (BirdType == 1) {
			BirdType = 2;
		}
		else if (BirdType == 2) {
			BirdType = 0;
		}
	}

	if (key == GLFW_KEY_LEFT) {
		if (BirdType == 0) {
			BirdType = 2;
		}
		else if (BirdType == 2) {
			BirdType = 1;
		}
		else if (BirdType == 1) {
			BirdType = 0;
		}
	}
}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}
