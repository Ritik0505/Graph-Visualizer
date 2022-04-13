#pragma once
#include<string>
class shader 
{
public:
	unsigned int m_id;
	shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void useShader();
private:
	void CheckCompilationError(unsigned int shader,const std::string& type);
};
