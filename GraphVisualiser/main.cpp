#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<GL/glew.h>
#include"GLFW/glfw3.h"
#include<vector>
#include"shader.h"
#include"string.h"

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include"function.h"

int window_width, window_height;
GLFWwindow* window;

std::vector<function> f;
functionColor color1(0.2f, 0.12f, 0.4f);
// functionColor color2(0.34f, 0.29f, 0.1f);
// function f(window, color1);
// function f2(window, color2);

struct color {
	float rgb[3] = { 1.0f,1.0f,1.0f };
};
std::vector<color> Color;

void processInput(GLFWwindow* window);
void change_offsets(GLFWwindow* window, double xpos, double ypos);
void RenderGUI();

bool firstClick = true;

double lastX = 0.0f;
double lastY = 0.0f;

float offset_x = 0.0f, offset_y = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float deltaOffsetX = 0.0f;
float lastOffsetX = 0.0f;

float negativeX = -10.0f;
float positiveX = 10.0f;

float scaleGraph = 1.0f;
float deltaScale = 0.0f;
float lastScale = 1.0f;

struct markings {
	float x, y;
};

std::vector<std::string> expression_string;

int main(void)
{
	//GLFWwindow* window;

	//Initialise the library
	if (!glfwInit())
		return -1;
	//Create a windowed mode window and its openGL context
	window = glfwCreateWindow(1000, 1000, "Graph Visualiser", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Make window's context current
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << " Error " << std::endl;

	shader xy_shader("res/Shaders/shader.vert", "res/Shaders/shader.frag");
	shader xy_axis_shader("res/Shaders/axis_shader.vert", "res/Shaders/axis_shader.frag");
	shader functionShader("res/Shaders/functionShader.vert", "res/Shaders/functionShader.frag");

	glfwGetWindowSize(window, &window_width, &window_height);
	std::cout << window_width << " " << window_height << std::endl;

	float xy_axis[8] = {
		-1.0f,  0.0f,
		 1.0f,  0.0f,
		 0.0f, -1.0f,
		 0.0f,	1.0f,
	};

	markings xy_marking[44];
	float step = 0.0f;
	float scale_length = 0.05f;

	for (int i = 0; i <= 21; i = i + 2)
	{
		xy_marking[i].x = -1;
		xy_marking[i + 1].x = -1 + scale_length;
		xy_marking[i].y = -1 + step;
		xy_marking[i + 1].y = xy_marking[i].y;
		step = step + 0.2f;

		xy_marking[i + 22].x = xy_marking[i].y;
		xy_marking[i + 1 + 22].x = xy_marking[i + 1].y;
		xy_marking[i + 22].y = xy_marking[i].x;
		xy_marking[i + 1 + 22].y = xy_marking[i + 1].x;
	}

	for (int i = 0; i < 44; i++)
	{
		std::cout << "Index: " << i << ":" << xy_marking[i].x << "," << xy_marking[i].y << std::endl;
	}

	unsigned int m_vao;
	unsigned int xy_vbo;
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &xy_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, xy_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xy_marking), xy_marking, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	/******************************************************************/
	unsigned int xy_axis_vbo, m_vao1;
	glGenVertexArrays(1, &m_vao1);
	glBindVertexArray(m_vao1);
	glGenBuffers(1, &xy_axis_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, xy_axis_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xy_axis), xy_axis, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//f.at(0).prepareBuffer();
	//f.at(1).prepareBuffer();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//Loop until the user closes window
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		deltaOffsetX = lastOffsetX - offset_x;
		lastOffsetX = offset_x;

		deltaScale = scaleGraph - lastScale;
		lastScale = scaleGraph;

		glClearColor(20.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		RenderGUI();

		xy_shader.useShader();
		glBindVertexArray(m_vao);
		glLineWidth(2.0f);

		/*******Anti-Aliasing*******/
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindBuffer(GL_ARRAY_BUFFER, xy_vbo);
		glDrawArrays(GL_LINES, 0, 44);

		glBindVertexArray(m_vao1);
		xy_axis_shader.useShader();
		glUniform1f(glGetUniformLocation(xy_axis_shader.m_id, "offset_x"), offset_x);
		glUniform1f(glGetUniformLocation(xy_axis_shader.m_id, "offset_y"), offset_y);
		glDrawArrays(GL_LINES, 0, 8);

		glLineWidth(2.5f);

		for (int i = 0; i < f.size(); i++) {
			f.at(i).drawFunction(functionShader, i);
		}

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		glfwSetCursorPosCallback(window, change_offsets);
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetCursorPosCallback(window, NULL);
		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			offset_y += 2.0f * deltaTime;
		else if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
			offset_y -= 2.0f * deltaTime;
		else if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			offset_x += 2.0f * deltaTime;
		else if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
			offset_x -= 2.0f * deltaTime;
		else if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
			scaleGraph += 2.0f * deltaTime;
		else if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
			(scaleGraph < 0.5f) ? (scaleGraph = 0.5f) : (scaleGraph -= 2.0f * deltaTime);
		else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			offset_x = 0.0f;
			offset_y = 0.0f;
			scaleGraph = 1.0f;
		}
	}
}

void change_offsets(GLFWwindow* window, double xpos, double ypos)
{
	glfwGetWindowSize(window, &window_width, &window_height);
	double x, y;

	// this will give mouse positions in range 0 to 1
	x = xpos / window_width;
	y = ypos / window_height;

	// to map mouse coordinates in range -1 to 1
	x = 2 * (x - 0.5f);
	y = 2 * (0.5f - y);

	offset_x += x * 0.02f;
	offset_y += y * 0.02f;

	//std::cout << x << " , " << y << std::endl;
}

void AddFunction() {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::uniform_real_distribution<float> number(0.0f, 1.0f);

	int i = f.size();
	if (ImGui::Button("Add")) {
		expression_string.push_back("0");
		char* expr = const_cast<char*>(expression_string.at(i).c_str());
		ImGui::InputText("", expr, 20);
		//expression_string.at(i).assign(expr);
		f.push_back(function(window, functionColor(number(e), number(e), number(e))));
		f.at(i).prepareBuffer();
	}
	ImGui::SameLine(50.0f);
	if (ImGui::Button("Delete")) {
		expression_string.pop_back();
		f.pop_back();
	}
}

void RenderGUI() {

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Functions");

	AddFunction();

	for (int i = 0; i < expression_string.size(); i++) {
		char* expr = const_cast<char*>(expression_string.at(i).c_str());
		ImGui::PushID(i);
		ImGui::Text("%d:", i + 1);
		ImGui::SameLine(30.0f);
		ImGui::InputText("", expr, 30);
		expression_string.at(i).assign(expr);

		Color.resize(f.size());
		ImGui::ColorEdit3("", Color.at(i).rgb);
		f.at(i).setFunctionColor(Color[i].rgb[0], Color[i].rgb[1], Color[i].rgb[2]);

		ImGui::PopID();
	}

	ImGui::End();
}

