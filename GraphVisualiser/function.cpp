#include "function.h"
#include <exprtk.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef exprtk::symbol_table<float> symbol_table_t;
typedef exprtk::expression<float> expression_t;
typedef exprtk::parser<float> parser_t;

extern float deltaOffsetX;
extern float deltaScale;

void function::calculateCoordinates(const unsigned int& index)
{
	functionCoord.clear();
	float x;

	symbol_table_t symbol_table;
	symbol_table.add_variable("x", x);

	expression_t expression;
	expression.register_symbol_table(symbol_table);

	parser_t parser;
	if(!parser.compile(expression_string.at(index), expression))
		throw "Expression Compilation Error";
	for (x = (negativeX - offset_x * positiveX) / scaleGraph; x <= (positiveX - offset_x * positiveX) / scaleGraph; x = x + 0.001f)
	{
		functionCoord.push_back(coordinates(x, expression.value()));
	}
}

//void function::setDomain(float negativeX, float positiveX)
//{
//	if (first)
//	{
//		this->negativeX = negativeX;
//		this->positiveX = positiveX;
//	}
//	else
//	{
//		this->negativeX = (negativeX - offset_x * positiveX) / scaleGraph;
//		this->positiveX = (positiveX - offset_x * positiveX) / scaleGraph;
//	}	
//	calculateCoordinates(this->negativeX, this->positiveX);
//}

void function::prepareBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void function::drawFunction(shader& functionShader, const unsigned int& index)
{
	if (negativeX > positiveX)
		throw "Invalid Domain";

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (deltaOffsetX != 0.0f || deltaScale != 0.0f || this->first && !expression_string.empty())
	{
		this->first = false;
		calculateCoordinates(index);
		glBufferData(GL_ARRAY_BUFFER, functionCoord.size() * sizeof(coordinates), functionCoord.data(), GL_STREAM_DRAW);
		std::cout << "data bind again" << std::endl;
	}
	
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaleGraph, scaleGraph, 1.0f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(offset_x, offset_y, 0.0f));

	functionShader.useShader();
	glUniformMatrix4fv((glGetUniformLocation(functionShader.m_id, "scale")), 1, GL_FALSE, glm::value_ptr(scale));
	glUniformMatrix4fv((glGetUniformLocation(functionShader.m_id, "translate")), 1, GL_FALSE, glm::value_ptr(translate));
	glUniform4f((glGetUniformLocation(functionShader.m_id, "color")), fColor.getR(), fColor.getG(), fColor.getB(), 1.0f) ;

	glDrawArrays(GL_LINE_STRIP, 0, functionCoord.size());
}

void function::setFunctionColor(float r, float g, float b) {
	fColor.setR(r);
	fColor.setG(g);
	fColor.setB(b);
}

