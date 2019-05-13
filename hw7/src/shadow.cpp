#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "stb_image.h"

using namespace std;

//顶点着色器代码
const char *vertexShaderSource1 =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 lightSpaceMatrix;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);\n"
"}\0";

//顶点着色器代码
const char *vertexShaderSource2 =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"out VS_OUT{\n"
"	vec3 FragPos;\n"
"	vec3 Normal;\n"
"	vec2 TexCoords;\n"
"	vec4 FragPosLightSpace;\n"
"} vs_out;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"uniform mat4 lightSpaceMatrix;\n"
"void main()\n"
"{\n"
"	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));\n"
"	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;\n"
"	vs_out.TexCoords = aTexCoords;\n"
"	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

// 顶点着色器代码
const char *vertexShaderSource3 =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"void main()\n"
"{\n"
"	TexCoords = aTexCoords;\n"
"	gl_Position = vec4(aPos, 1.0);\n"
"}\0";

//片段着色器代码
const char *fragmentShaderSource1 =
"#version 330 core\n"
"void main()\n"
"{\n"
"	// gl_FragDepth = gl_FragCoord.z;\n"
"}\n\0";

//片段着色器代码
const char *fragmentShaderSource2 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in VS_OUT{\n"
"	vec3 FragPos;\n"
"	vec3 Normal;\n"
"	vec2 TexCoords;\n"
"	vec4 FragPosLightSpace;\n"
"} fs_in;\n"
"uniform sampler2D diffuseTexture;\n"
"uniform sampler2D shadowMap;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform float ambientFactor;\n"
"uniform float diffuseFactor;\n"
"uniform float specularFactor;\n"
"uniform int shininessFactor;\n"
"float ShadowCalculation(vec4 fragPosLightSpace)\n"
"{\n"
"	// perform perspective divide\n"
"	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
"	// transform to [0,1] range\n"
"	projCoords = projCoords * 0.5 + 0.5;\n"
"	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n"
"	float closestDepth = texture(shadowMap, projCoords.xy).r;\n"
"	// get depth of current fragment from light's perspective\n"
"	float currentDepth = projCoords.z;\n"
"	// calculate bias (based on depth map resolution and slope)\n"
"	vec3 normal = normalize(fs_in.Normal);\n"
"	vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n"
"	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);\n"
"	// check whether current frag pos is in shadow\n"
"	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;\n"
"	// PCF\n"
"	float shadow = 0.0;\n"
"	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);\n"
"	for (int x = -1; x <= 1; ++x)\n"
"	{\n"
"		for (int y = -1; y <= 1; ++y)\n"
"		{\n"
"			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;\n"
"			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;\n"
"		}\n"
"	}\n"
"	shadow /= 9.0;\n"
"	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.\n"
"	if (projCoords.z > 1.0)\n"
"		shadow = 0.0;\n"
"	return shadow;\n"
"}\n"
"void main()\n"
"{\n"
"	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;\n"
"	vec3 normal = normalize(fs_in.Normal);\n"
"	vec3 lightColor = vec3(0.3);\n"
"	// ambient\n"
"	vec3 ambient = ambientFactor * 0.3 * color;\n"
"	// diffuse\n"
"	vec3 lightDir = normalize(lightPos - fs_in.FragPos);\n"
"	float diff = max(dot(lightDir, normal), 0.0);\n"
"	vec3 diffuse = diffuseFactor * diff * lightColor;\n"
"	// specular\n"
"	vec3 viewDir = normalize(viewPos - fs_in.FragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, normal);\n"
"	float spec = 0.0;\n"
"	vec3 halfwayDir = normalize(lightDir + viewDir);\n"
"	spec = pow(max(dot(normal, halfwayDir), 0.0), shininessFactor);\n"
"	vec3 specular = specularFactor * spec * lightColor;\n"
"	// calculate shadow\n"
"	float shadow = ShadowCalculation(fs_in.FragPosLightSpace);\n"
"	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;\n"
"	FragColor = vec4(lighting, 1.0);\n"
"}\n\0";

//片段着色器代码
const char *fragmentShaderSource3 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D depthMap;\n"
"uniform float near_plane;\n"
"uniform float far_plane;\n"
"// required when using a perspective projection matrix\n"
"float LinearizeDepth(float depth)\n"
"{\n"
"	float z = depth * 2.0 - 1.0; // Back to NDC \n"
"	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));\n"
"}\n"
"void main()\n"
"{\n"
"	float depthValue = texture(depthMap, TexCoords).r;\n"
"	// FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective\n"
"	FragColor = vec4(vec3(depthValue), 1.0); // orthographic\n"
"}\n\0";

int windowWidth = 800;
int windowHeight = 800;

ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
ImVec4 color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

// camera
glm::vec3 cameraPos = glm::vec3(3.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// mouse
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
float fov = 45.0f;

// light
glm::vec3 lightPos = glm::vec3(3.0f, 2.0f, -1.0f);
float ambientFactor = 1.0f;
float diffuseFactor = 1.0f;
float specularFactor = 1.0f;
int shininessFactor = 64;

// euler angle
float yaw = 0.0f;
float pitch = 0.0f;
float roll = 0.0f;

// tool window
bool showSettings = true;
bool showPhong = true;
bool showBonus = false;

unsigned int shaderProgram1, shaderProgram2, shaderProgram3;
unsigned int planeVAO, cubeVAO, quadVAO;
unsigned int planeVBO, cubeVBO, quadVBO;
unsigned int depthMapFBO;
unsigned int woodTexture;

//窗口大小改变的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//处理键盘输入
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); //关闭glfw，对应while循环那个判断条件
}

void RenderQuad() {
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void RenderCube() {
	float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
        // Left face
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
        // Right face
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // Top face
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
    };

	if (cubeVAO == 0)
	{
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// Fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// Link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderScene(unsigned int shaderProgramID) {
	// Floor
	glm::mat4 model = glm::mat4(1.0f);;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	// Cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.5));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	RenderCube();
}

unsigned int loadTexture(const char* path)
{
	// Generate texture ID and load texture data 
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char *image = stbi_load(path, &width, &height, 0, 0);// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// SOIL_free_image_data(image);
	return textureID;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// MAC用户

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//获取glfw的所有函数指针地址
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);	//视口大小

													//注册回调事件
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//编译顶点着色器1
	unsigned int vertexShader1;
	vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader1, 1, &vertexShaderSource1, NULL);
	glCompileShader(vertexShader1);
	//检查编译错误
	int success1;
	char infoLog1[512];
	glGetShaderiv(vertexShader1, GL_COMPILE_STATUS, &success1);
	if (!success1) {
		glGetShaderInfoLog(vertexShader1, 512, NULL, infoLog1);
		std::cout << "Shader compile error\n" << infoLog1 << std::endl;
	}

	//编译顶点着色器2
	unsigned int vertexShader2;
	vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vertexShaderSource2, NULL);
	glCompileShader(vertexShader2);
	//检查编译错误
	int success2;
	char infoLog2[512];
	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success2);
	if (!success2) {
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog2);
		std::cout << "Shader compile error\n" << infoLog2 << std::endl;
	}

	//编译顶点着色器3
	unsigned int vertexShader3;
	vertexShader3 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader3, 1, &vertexShaderSource3, NULL);
	glCompileShader(vertexShader3);
	//检查编译错误
	int success3;
	char infoLog3[512];
	glGetShaderiv(vertexShader3, GL_COMPILE_STATUS, &success3);
	if (!success3) {
		glGetShaderInfoLog(vertexShader3, 512, NULL, infoLog3);
		std::cout << "Shader compile error\n" << infoLog3 << std::endl;
	}

	//编译片段着色器1
	unsigned int fragmentShader1;
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader1);
	//检查编译错误
	int success4;
	char infoLog4[512];
	glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success4);
	if (!success4) {
		glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog4);
		std::cout << "Shader compile error\n" << infoLog4 << std::endl;
	}

	//编译片段着色器2
	unsigned int fragmentShader2;
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);
	//检查编译错误
	int success5;
	char infoLog5[512];
	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success5);
	if (!success5) {
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog5);
		std::cout << "Shader compile error\n" << infoLog5 << std::endl;
	}

	//编译片段着色器3
	unsigned int fragmentShader3;
	fragmentShader3 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader3, 1, &fragmentShaderSource3, NULL);
	glCompileShader(fragmentShader3);
	//检查编译错误
	int success6;
	char infoLog6[512];
	glGetShaderiv(fragmentShader3, GL_COMPILE_STATUS, &success6);
	if (!success6) {
		glGetShaderInfoLog(fragmentShader3, 512, NULL, infoLog6);
		std::cout << "Shader compile error\n" << infoLog6 << std::endl;
	}
	
	//创建和链接着色器程序
	shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader1);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);
	//检查链接错误
	int success7;
	char infoLog7[512];
	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success7);
	if (!success7) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog7);
		std::cout << "Program link error\n" << infoLog7 << std::endl;
	}

	//创建和链接着色器程序
	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);
	//检查链接错误
	int success8;
	char infoLog8[512];
	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success8);
	if (!success8) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog8);
		std::cout << "Program link error\n" << infoLog8 << std::endl;
	}

	//创建和链接着色器程序
	shaderProgram3 = glCreateProgram();
	glAttachShader(shaderProgram3, vertexShader3);
	glAttachShader(shaderProgram3, fragmentShader3);
	glLinkProgram(shaderProgram3);
	//检查链接错误
	int success9;
	char infoLog9[512];
	glGetProgramiv(shaderProgram3, GL_LINK_STATUS, &success9);
	if (!success9) {
		glGetProgramInfoLog(shaderProgram3, 512, NULL, infoLog9);
		std::cout << "Program link error\n" << infoLog9 << std::endl;
	}

	//删除着色器对象
	glDeleteShader(vertexShader1);
	glDeleteShader(vertexShader2);
	glDeleteShader(vertexShader3);
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);
	glDeleteShader(fragmentShader3);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330 core";
	ImGui_ImplOpenGL3_Init(glsl_version);

	float planeVertices[] = {
		// Positions          // Normals         // Texture Coords
		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f,
		-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
		25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f, 25.0f
	};

	// plane 

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	// vertice attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load texture
	woodTexture = loadTexture("./wood.png");

	// Configure depth map FBO
	const GLuint shadowWidth = 1024, shadowHeight = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window); //持续监听键盘输入		

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("Tools")) {
			ImGui::MenuItem("Settings", NULL, &showSettings);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Color"))
		{
			ImGui::ColorEdit3("Clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
			ImGui::EndMenu();
		}
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::EndMainMenuBar();

		float light_pos[3] = { 3.0f, 2.0f, -1.0f };
		if (showSettings) {
			ImGui::Begin("Settings", &showSettings);
			ImGui::SliderFloat3("Light Position", light_pos, -5, 5);
			ImGui::SliderFloat("Ambient", &ambientFactor, 0, 5);
			ImGui::SliderFloat("Diffuse", &diffuseFactor, 0, 5);
			ImGui::SliderFloat("Specular", &specularFactor, 0, 5);
			ImGui::SliderInt("Shininess", &shininessFactor, 0, 256);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		lightPos.x = light_pos[0];
		lightPos.y = light_pos[1];
		lightPos.z = light_pos[2];

		// 渲染
		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //设置清屏颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清屏		
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);

		// 1. Render depth of scene to texture (from ligth's perspective)
		// - Get light projection/view matrix.
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.0f, far_plane = 7.5f;
		// 正交投影
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		// 将物体变换到光源视角可见的位置
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// 渲染场景
		glUseProgram(shaderProgram1); // simpleDepthShader
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram1, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(shaderProgram1);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Render scene as normal
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgram2); // shader
		glUniform1i(glGetUniformLocation(shaderProgram2, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram2, "shadowMap"), 1);
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "view"), 1, GL_FALSE, glm::value_ptr(view));
		// Set light uniforms
		glUniform3fv(glGetUniformLocation(shaderProgram2, "lightPos"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(shaderProgram2, "viewPos"), 1, &cameraPos[0]);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glUniform1fv(glGetUniformLocation(shaderProgram2, "ambientFactor"), 1, &ambientFactor);
		glUniform1fv(glGetUniformLocation(shaderProgram2, "diffuseFactor"), 1, &diffuseFactor);
		glUniform1fv(glGetUniformLocation(shaderProgram2, "specularFactor"), 1, &specularFactor);
		glUniform1iv(glGetUniformLocation(shaderProgram2, "shininessFactor"), 1, &shininessFactor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		RenderScene(shaderProgram2);

		// 3. DEBUG: visualize depth map by rendering it to plane
		glUseProgram(shaderProgram3); // debugDepthQuad
		glUniform1f(glGetUniformLocation(shaderProgram3, "near_plane"), near_plane);
		glUniform1f(glGetUniformLocation(shaderProgram3, "far_plane"), far_plane);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//RenderQuad();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window); //双缓冲交换避免闪烁
		glfwPollEvents(); //处理IO事件		
	}

	//释放资源
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}