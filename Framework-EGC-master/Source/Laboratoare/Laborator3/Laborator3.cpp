#include "Laborator3.h"

#include <vector>
#include <iostream>
//#include<GL/glut.h>

#include <Core/Engine.h>
#include <Core/Window/InputController.h>
#include "Transform2D.h"
#include "Object2D.h"

//vector<Mesh*> obstacles;

//#include <olcConsoleGameEngine.h>

using namespace std;

Laborator3::Laborator3()
{
}

Laborator3::~Laborator3()
{
}

void Laborator3::Init()
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

	// compute coordinates of square center
	float cx = corner.x + squareSide / 2;
	float cy = corner.y + squareSide / 2;

	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;

	// initialize angularStep
	angularStep = 0;

	Mesh* square4 = Object2D::CreateSquare("square4", corner, squareSide, glm::vec3(0, 0, 1), true);
	AddMeshToList(square4);

	Mesh* square5 = Object2D::CreateSquare("square5", corner, squareSide, glm::vec3(0, 1, 0), true);
	AddMeshToList(square5);

	
	//obstacles.push_back(square5);

	vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(0, 0,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(0, 1,  0), glm::vec3(1, 0.48f, 0)),
		VertexFormat(glm::vec3(1, 0.5,  0), glm::vec3(1, 0.48f, 0)),

	};

	vector<unsigned short> indices =
	{
		0, 1, 2	// indices for first triangle
	};

	Mesh* triunghi = CreateMesh("triunghi", vertices, indices);
}

Mesh* Laborator3::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
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

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttribPointer(VERTEX_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 12, vertexBuffer.data());
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

void Laborator3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0.6, 0.8, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator3::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();
	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(350, 285);
	modelMatrix *= Transform2D::Scale(30, 30);
	modelMatrix *= Transform2D::Translate(0.333, 0.5);
	angularStep += deltaTimeSeconds;
	//modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Translate(-0.333, -0.5);
	//modelMatrix *= Transform2D::Rotate(20);
	RenderMesh2D(meshes["triunghi"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(200, 250);
	modelMatrix *= Transform2D::Scale(1.5, 1);
	if (Bposition > 0) {
		Bposition--;
	}
	modelMatrix *= Transform2D::Translate(50, 50);
	//modelMatrix *= Transform2D::Rotate(angularStep);
	modelMatrix *= Transform2D::Translate(-50, -50);
	//modelMatrix *= Transform2D::Translate(0, Bposition);
	RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Scale(1, 2);
	if (poz + 100 > 0) {
		poz--;
	}
	else {
		poz = resolution.x;
	}
	//modelMatrix *= Transform2D::Translate(poz, 0);
	//modelMatrix *= Transform2D::Translate(0, Bposition);
	RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);

}

void Laborator3::FrameEnd()
{

}

void Laborator3::OnInputUpdate(float deltaTime, int mods)
{
	/*if (window->KeyHold(GLFW_KEY_SPACE)) {
		modelMatrix *= Transform2D::Rotate(-40);
		Bposition += 2;
	}*/

}

void Laborator3::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_SPACE)
	{
		Bposition += 10;
	}
}

void Laborator3::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator3::OnWindowResize(int width, int height)
{
}
