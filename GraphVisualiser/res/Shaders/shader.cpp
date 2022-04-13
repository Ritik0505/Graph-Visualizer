#include<iostream>
#include"shader.h"
#include<string>
#include<fstream>
#include<sstream>
#include<GL/glew.h>

shader::shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	
	//Ensuring ifstream can objects throw exceptions 
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open shader file of mentioned paths
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);

		std::stringstream vertexStream, fragmentStream;
		vertexStream << vertexShaderFile.rdbuf();
		fragmentStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ";
	}
	
	//converting shadercode to C-style char array pointer
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//Compiling and checking compilation error
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vShaderCode, NULL);
	glShaderSource(fragmentShader, 1, &fShaderCode, NULL);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	CheckCompilationError(vertexShader, "VERTEX");
	CheckCompilationError(fragmentShader, "FRAGMENT");
	
	// shader program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertexShader);
	glAttachShader(m_id, fragmentShader);
	glLinkProgram(m_id);
	CheckCompilationError(m_id, "PROGRAM");

	glDetachShader(m_id, vertexShader);
	glDetachShader(m_id, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void shader::CheckCompilationError(unsigned int shader, const std::string& type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR_TYPE_" << type << std::endl << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR_TYPE_" << type << std::endl << infoLog << std::endl;
		}
	}
}

void shader::useShader()
{
	glUseProgram(m_id);
}