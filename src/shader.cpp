#include "Shader.h"

unsigned int create_shader(GLenum type, const char* source)
{
	uint32_t shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
	    glGetShaderInfoLog(shader, 512, NULL, infoLog);
	    std::cout << "ERROR::SHADER::" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

void Shader::initialize(const char* vertex_path, const char* fragment_path)
{
	std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        vShaderFile.open(vertex_path);
        fShaderFile.open(fragment_path);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();		
        
        vShaderFile.close();
        fShaderFile.close();
        
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();		
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

	uint32_t vertexShader = create_shader(GL_VERTEX_SHADER, vertexCode.c_str());
	uint32_t fragmentShader = create_shader(GL_FRAGMENT_SHADER, fragmentCode.c_str());

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	glLinkProgram(id);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::set_uniform_3_f(const char* name, const glm::vec3 vector)
{
	int location = glGetUniformLocation(id, name);
	glUniform3fv(location, 1, (float*)&vector);
}

void Shader::set_uniform_4_f(const char* name, const glm::vec4 vector)
{
	int location = glGetUniformLocation(id, name);
	glUniform4fv(location, 1, (float*)&vector);
}

void Shader::set_uniform_mat4_f(const char* name, const glm::mat4 matrix)
{
	int location = glGetUniformLocation(id, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void Shader::set_uniform_texture(const char* name, Texture texture)
{
	int location = glGetUniformLocation(id, name);
	glUniform1i(location, 0);
}