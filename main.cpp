#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include "GL/glew.h"
#include "GLFW/glfw3.h"



//width and height
int windowWidth = 720;
int windowHeight = 600;

// VAO = Vertex Array Object
// VBO = Vertex Buffer Object
GLuint VAO, VBO, shader, uniformXMove;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

// Vertex Shader
static const char* vShader =
"#version 430 \n"
"layout (location = 0) in vec3 pos; \n"
"uniform float xMove; \n"
"void main() {gl_Position = vec4 (0.4 * pos.x + xMove, 0.4 * pos.y, pos.z, 1.0);} \n";

static const char* fShader =
"#version 430 \n"
"out vec4 color; \n"
"void main(){ color = vec4(0.0, 0.0, 1.0, 1.0); }"
;

void CreateTriangle()
{
	GLfloat vertices[] = {
		0.0f,  1.0f, 0.0f,
	   -1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);	//unbind buffer array
	glBindVertexArray(0);						//unbind vertex array

}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
		printf("error compiling the %d  program: '%s'\n",shaderType, eLog);
		return;
	}
	glAttachShader(theProgram, theShader);

}

void CompileShader()
{
	shader = glCreateProgram();
	if (!shader)
	{
		printf("Error creating shader program");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		printf("error linking program: '%s'\n", eLog);
		return;
	}
	glValidateProgram(shader);

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
		printf("error linking program: '%s'\n", eLog);
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");

}

int main()
{
	if (!glfwInit())
	{
		printf("Error initiating GLFW");
		glfwTerminate();
		exit(EXIT_FAILURE); //or return 1 perhaps?
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	

	// Core profile = no backward compability
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Test Window", nullptr, nullptr);
	if (!window)
	{
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
	// set context to GLEW to use
	glfwMakeContextCurrent(window);
	// Allow modern extension
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed");
		glfwDestroyWindow(window);  // destroy window because created on top then we can terminate glfw
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//create viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	glfwSwapInterval(1);

	CreateTriangle();
	CompileShader();


	//loop until window close
	while (!glfwWindowShouldClose(window))
	{
		// get + handle user input events
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else
		{
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}


		//Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(window);
	}


	glfwDestroyWindow(window);
	glfwTerminate();
	/*exit(EXIT_SUCCESS);*/
	return 0;

}