#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
using namespace std;

//顶点着色器代码，控制物体（Phong）
const char *vertexShaderSource1 =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 fragPos;\n"
"out vec3 normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 project;\n"
"void main()\n"
"{\n"
"   gl_Position = project * view * model * vec4(aPos, 1.0);\n"
"	fragPos = vec3(model * vec4(aPos, 1.0));\n"
"	normal = aNormal;\n"
"}\0";

//顶点着色器代码，控制光源
const char *vertexShaderSource2 =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 project;\n"
"void main()\n"
"{\n"
"   gl_Position = project * view * model * vec4(aPos, 1.0);\n"
"}\0";

// 顶点着色器代码，控制物体（Gouraud）
const char *vertexShaderSource3 =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 lightingColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 project;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 lightColor;\n"
"uniform float ambientFactor;\n"
"uniform float diffuseFactor;\n"
"uniform float specularFactor;\n"
"uniform int shininessFactor;\n"
"void main()\n"
"{\n"
"   gl_Position = project * view * model * vec4(aPos, 1.0);\n"
"	vec3 fragPos = vec3(model * vec4(aPos, 1.0));\n"
"	vec3 normal = aNormal;\n"
"	//float ambientFactor = 0.1;\n"
"	vec3 ambient = ambientFactor * lightColor;\n"
"	//float diffuseFactor = 1;\n"
"	vec3 norm = normalize(normal);\n"
"	vec3 lightDir = normalize(lightPos - fragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diffuseFactor * diff * lightColor;\n"
"	//float specularFactor = 0.5;\n"
"	vec3 viewDir = normalize(viewPos - fragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessFactor);\n"
"	vec3 specular = specularFactor * spec * lightColor;\n"
"	lightingColor = ambient + diffuse + specular;\n"
"}\0";

//片段着色器代码，控制物体颜色（Phong）
const char *fragmentShaderSource1 =
"#version 330 core\n"
"out vec4 fragColor;\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"uniform float ambientFactor;\n"
"uniform float diffuseFactor;\n"
"uniform float specularFactor;\n"
"uniform int shininessFactor;\n"
"void main()\n"
"{\n"
"	//float ambientFactor = 0.1;\n"
"	vec3 ambient = ambientFactor * lightColor;\n"
"	//float diffuseFactor = 1;\n"
"	vec3 norm = normalize(normal);\n"
"	vec3 lightDir = normalize(lightPos - fragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diffuseFactor * diff * lightColor;\n"
"	//float specularFactor = 0.5;\n"
"	vec3 viewDir = normalize(viewPos - fragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessFactor);\n"
"	vec3 specular = specularFactor * spec * lightColor;\n"
"	vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"	fragColor = vec4(result, 1.0);\n"
"}\n\0";

//片段着色器代码，控制光源颜色
const char *fragmentShaderSource2 =
"#version 330 core\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"   fragColor = vec4(1.0f);\n"
"}\n\0";

//片段着色器代码，控制物体颜色（Gouraud）
const char *fragmentShaderSource3 =
"#version 330 core\n"
"out vec4 fragColor;\n"
"in vec3 lightingColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
"   fragColor = vec4(lightingColor * objectColor, 1.0f);\n"
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
glm::vec3 lightPos = glm::vec3(2.0f, 2.0f, -2.0f);
float ambientFactor = 0.1f;
float diffuseFactor = 1.0f;
float specularFactor = 0.5f;
int shininessFactor = 32.0f;

// euler angle
float yaw = 0.0f;
float pitch = 0.0f;
float roll = 0.0f;

// tool window
bool showSettings = true;
bool showPhong = true;
bool showGouraud = false;
bool showBonus = false;

unsigned int shaderProgram1, shaderProgram2, shaderProgram3;
unsigned int VBO, lightVAO, cubeVAO, EBO;

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

	//创建和链接着色器程序，控制物体	
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

	//创建和链接着色器程序，控制光源
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

	//创建和链接着色器程序，控制光源
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

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window); //持续监听键盘输入			

		float vertices[] = {
			-2, -2, -2,  0.0f,  0.0f, -1.0f,
			 2, -2, -2,  0.0f,  0.0f, -1.0f,
			 2,  2, -2,  0.0f,  0.0f, -1.0f,
			 2,  2, -2,  0.0f,  0.0f, -1.0f,
			-2,  2, -2,  0.0f,  0.0f, -1.0f,
			-2, -2, -2,  0.0f,  0.0f, -1.0f,

			-2, -2,  2,  0.0f,  0.0f,  1.0f,
			 2, -2,  2,  0.0f,  0.0f,  1.0f,
			 2,  2,  2,  0.0f,  0.0f,  1.0f,
			 2,  2,  2,  0.0f,  0.0f,  1.0f,
			-2,  2,  2,  0.0f,  0.0f,  1.0f,
			-2, -2,  2,  0.0f,  0.0f,  1.0f,

			-2,  2,  2, -1.0f,  0.0f,  0.0f,
			-2,  2, -2, -1.0f,  0.0f,  0.0f,
			-2, -2, -2, -1.0f,  0.0f,  0.0f,
			-2, -2, -2, -1.0f,  0.0f,  0.0f,
			-2, -2,  2, -1.0f,  0.0f,  0.0f,
			-2,  2,  2, -1.0f,  0.0f,  0.0f,

			 2,  2,  2,  1.0f,  0.0f,  0.0f,
			 2,  2, -2,  1.0f,  0.0f,  0.0f,
			 2, -2, -2,  1.0f,  0.0f,  0.0f,
			 2, -2, -2,  1.0f,  0.0f,  0.0f,
			 2, -2,  2,  1.0f,  0.0f,  0.0f,
			 2,  2,  2,  1.0f,  0.0f,  0.0f,

			-2, -2, -2,  0.0f, -1.0f,  0.0f,
			 2, -2, -2,  0.0f, -1.0f,  0.0f,
			 2, -2,  2,  0.0f, -1.0f,  0.0f,
			 2, -2,  2,  0.0f, -1.0f,  0.0f,
			-2, -2,  2,  0.0f, -1.0f,  0.0f,
			-2, -2, -2,  0.0f, -1.0f,  0.0f,

			-2,  2, -2,  0.0f,  1.0f,  0.0f,
			 2,  2, -2,  0.0f,  1.0f,  0.0f,
			 2,  2,  2,  0.0f,  1.0f,  0.0f,
			 2,  2,  2,  0.0f,  1.0f,  0.0f,
			-2,  2,  2,  0.0f,  1.0f,  0.0f,
			-2,  2, -2,  0.0f,  1.0f,  0.0f,
		};

		glGenVertexArrays(1, &lightVAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(lightVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// vertice attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// vertice attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

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

		if (showSettings) {
			ImGui::Begin("Settings", &showSettings);
			ImGui::Checkbox("Phong shading", &showPhong);
			ImGui::Checkbox("Gouraud shading", &showGouraud);
			ImGui::Checkbox("Bonus", &showBonus);
			ImGui::SliderFloat("ambient", &ambientFactor, 0, 5);
			ImGui::SliderFloat("diffuse", &diffuseFactor, 0, 5);
			ImGui::SliderFloat("specular", &specularFactor, 0, 5);
			ImGui::SliderInt("shininess", &shininessFactor, 0, 256);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 渲染
		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //设置清屏颜色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清屏		
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		

		// 变换矩阵
		glm::mat4 model = glm::mat4(1.0f); // 初始化成单位矩阵
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 project = glm::mat4(1.0f);				

		project = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);		
		view = glm::lookAt(cameraPos, glm::vec3(0,0,0), cameraUp);
		model = glm::scale(model, glm::vec3(0.4f));		
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));

		if (showPhong) {
			// 获取uniform地址并设置，控制物体
			glUseProgram(shaderProgram1);
			unsigned int modelLoc1 = glGetUniformLocation(shaderProgram1, "model");
			glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model));
			unsigned int viewLoc1 = glGetUniformLocation(shaderProgram1, "view");
			glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view));
			unsigned int projectLoc1 = glGetUniformLocation(shaderProgram1, "project");
			glUniformMatrix4fv(projectLoc1, 1, GL_FALSE, glm::value_ptr(project));
			unsigned int ocLoc = glGetUniformLocation(shaderProgram1, "objectColor");
			glUniform3fv(ocLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
			unsigned int lcLoc = glGetUniformLocation(shaderProgram1, "lightColor");
			glUniform3fv(lcLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
			unsigned int lpLoc = glGetUniformLocation(shaderProgram1, "lightPos");
			glUniform3fv(lpLoc, 1, glm::value_ptr(lightPos));
			unsigned int vpLoc = glGetUniformLocation(shaderProgram1, "viewPos");
			glUniform3fv(vpLoc, 1, glm::value_ptr(cameraPos));
			unsigned int afLoc = glGetUniformLocation(shaderProgram1, "ambientFactor");
			glUniform1fv(afLoc, 1, &ambientFactor);
			unsigned int dfLoc = glGetUniformLocation(shaderProgram1, "diffuseFactor");
			glUniform1fv(dfLoc, 1, &diffuseFactor);
			unsigned int sfLoc = glGetUniformLocation(shaderProgram1, "specularFactor");
			glUniform1fv(sfLoc, 1, &specularFactor);
			unsigned int shfLoc = glGetUniformLocation(shaderProgram3, "shininessFactor");
			glUniform1iv(shfLoc, 1, &shininessFactor);

			// 渲染物体
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		if (showGouraud) {
			// 获取uniform地址并设置，控制物体
			glUseProgram(shaderProgram3);
			unsigned int modelLoc1 = glGetUniformLocation(shaderProgram3, "model");
			glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model));
			unsigned int viewLoc1 = glGetUniformLocation(shaderProgram3, "view");
			glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view));
			unsigned int projectLoc1 = glGetUniformLocation(shaderProgram3, "project");
			glUniformMatrix4fv(projectLoc1, 1, GL_FALSE, glm::value_ptr(project));
			unsigned int ocLoc = glGetUniformLocation(shaderProgram3, "objectColor");
			glUniform3fv(ocLoc, 1, glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
			unsigned int lcLoc = glGetUniformLocation(shaderProgram3, "lightColor");
			glUniform3fv(lcLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
			unsigned int lpLoc = glGetUniformLocation(shaderProgram3, "lightPos");
			glUniform3fv(lpLoc, 1, glm::value_ptr(lightPos));
			unsigned int vpLoc = glGetUniformLocation(shaderProgram3, "viewPos");
			glUniform3fv(vpLoc, 1, glm::value_ptr(cameraPos));
			unsigned int afLoc = glGetUniformLocation(shaderProgram3, "ambientFactor");
			glUniform1fv(afLoc, 1, &ambientFactor);
			unsigned int dfLoc = glGetUniformLocation(shaderProgram3, "diffuseFactor");
			glUniform1fv(dfLoc, 1, &diffuseFactor);
			unsigned int sfLoc = glGetUniformLocation(shaderProgram3, "specularFactor");
			glUniform1fv(sfLoc, 1, &specularFactor);
			unsigned int shfLoc = glGetUniformLocation(shaderProgram3, "shininessFactor");
			glUniform1iv(shfLoc, 1, &shininessFactor);

			// 渲染物体
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// 调整光源位置、大小
		model = glm::mat4(1.0f); // 重新初始化成单位矩阵
		if (showBonus) {
			lightPos.x = sin(glfwGetTime()) * 2.0f;
			lightPos.y = cos(glfwGetTime()) * 2.0f;
			lightPos.z = cos(glfwGetTime()) * 2.0f;
		}
		model = glm::translate(model, lightPos);	
		model = glm::scale(model, glm::vec3(0.1f));

		// 获取uniform地址并设置，控制光源
		glUseProgram(shaderProgram2);
		unsigned int modelLoc2 = glGetUniformLocation(shaderProgram2, "model");
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int viewLoc2 = glGetUniformLocation(shaderProgram2, "view");
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectLoc2 = glGetUniformLocation(shaderProgram2, "project");
		glUniformMatrix4fv(projectLoc2, 1, GL_FALSE, glm::value_ptr(project));		

		// 渲染光源
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window); //双缓冲交换避免闪烁
		glfwPollEvents(); //处理IO事件		

		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &lightVAO);
		glDeleteBuffers(1, &VBO);
	}

	//释放资源
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}