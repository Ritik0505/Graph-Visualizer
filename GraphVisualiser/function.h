#include<iostream>
#include<GL/glew.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<shader.h>
#include<string.h>
#include<random>
#include<chrono>

//static unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//static std::default_random_engine e(seed);
//static std::uniform_real_distribution<float> number(0.0f, 1.0f);

extern float offset_x, offset_y;
extern float scaleGraph;
extern float negativeX, positiveX;
extern std::vector<std::string> expression_string;

struct coordinates {
	float x, y;
	coordinates() :x(0.0f), y(0.0f){}
	coordinates(float X, float Y):x(X), y(Y) {  }
};

class functionColor
{
private:
	float r, g, b;
public:
	functionColor() : r(), g(), b() {}
	functionColor(float R, float G, float B) : r(R), g(G), b(B) {}

	void setR(float red) { r = red; }
	void setG(float green) { g = green; }
	void setB(float blue) { b = blue; }

	float getR() { return r; }
	float getG() { return g; }
	float getB() { return b; }
};

class function
{
private:
	unsigned int VAO, VBO;
	
	GLFWwindow* window;
	functionColor fColor;
	std::vector<coordinates> functionCoord;

	//bool first = true;

	void calculateCoordinates(const unsigned int& index);
	//void prepareBuffer();

public:
	bool first = true;
	function(GLFWwindow* Window, functionColor color = functionColor()) : window(Window), fColor(color) { }//prepareBuffer(); }
	
	/*void setExpression(const char* expression_string) { this->expression_string = expression_string; }
	void setDomain(float negativeX, float positiveX);*/
	void prepareBuffer();
	void setFunctionColor(float r, float g, float b);
	void setFunctionColor(functionColor color) { fColor = color; }
	
	const unsigned int& getVBO() const { return VBO; }
	const unsigned int& getVAO() const { return VAO; }
	const std::vector<coordinates>& getFunctionCoordinates() const { return functionCoord; }

	void drawFunction(shader& functionShader, const unsigned int& index);
	void functionDomainChanged(shader&);
};