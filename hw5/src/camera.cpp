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

//顶点着色器代码
const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 project;\n"
"void main()\n"
"{\n"
"   gl_Position = project * view * model * vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

//片段着色器代码
const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int windowWidth = 800;
int windowHeight = 800;

ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
ImVec4 color1 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
ImVec4 color2 = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
ImVec4 color3 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
ImVec4 color4 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 color5 = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
ImVec4 color6 = ImVec4(1.0f, 0.0f, 1.0f, 1.0f);

// tool window
bool showProjection = true;
bool showView = false;
bool showBonus = false;

// project checkbox
bool orthProj = true;
bool persProj = false;

// project settings
float orthParams[6] = { -10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f };
float persParams[4] = { 45.0f, (float)windowWidth / (float)windowHeight, 0.1f, 100.0f };

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// mouse
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
float fov = 45.0f;

// euler angle
float yaw = 0.0f;
float pitch = 0.0f;
float roll = 0.0f;

unsigned int shaderProgram;
unsigned int VBO, VAO, EBO;

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
	float cameraSpeed = 0.2f; // 移动速度
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// 解决鼠标跳动
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// 计算偏移量
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.2f; // 灵敏度
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// 边界限制
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// 更新摄像机朝向
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
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

	//编译顶点着色器
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//检查编译错误
	int success1;
	char infoLog1[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success1);
	if (!success1) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog1);
		std::cout << "Shader compile error\n" << infoLog1 << std::endl;
	}

	//编译片段着色器
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//检查编译错误
	int success2;
	char infoLog2[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success2);
	if (!success2) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog2);
		std::cout << "Shader compile error\n" << infoLog2 << std::endl;
	}

	//创建和链接着色器程序	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//检查链接错误
	int success3;
	char infoLog3[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success3);
	if (!success3) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog3);
		std::cout << "Program link error\n" << infoLog3 << std::endl;
	}

	//删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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
			-2, -2, -2,  color1.x, color1.y, color1.z,
			2, -2, -2,  color1.x, color1.y, color1.z,
			2,  2, -2,  color1.x, color1.y, color1.z,
			2,  2, -2,  color1.x, color1.y, color1.z,
			-2,  2, -2,  color1.x, color1.y, color1.z,
			-2, -2, -2,  color1.x, color1.y, color1.z,

			-2, -2,  2,  color2.x, color2.y, color2.z,
			2, -2,  2,  color2.x, color2.y, color2.z,
			2,  2,  2,  color2.x, color2.y, color2.z,
			2,  2,  2,  color2.x, color2.y, color2.z,
			-2,  2,  2,  color2.x, color2.y, color2.z,
			-2, -2,  2,  color2.x, color2.y, color2.z,

			-2,  2,  2,  color3.x, color3.y, color3.z,
			-2,  2, -2,  color3.x, color3.y, color3.z,
			-2, -2, -2,  color3.x, color3.y, color3.z,
			-2, -2, -2,  color3.x, color3.y, color3.z,
			-2, -2,  2,  color3.x, color3.y, color3.z,
			-2,  2,  2,  color3.x, color3.y, color3.z,

			2,  2,  2,  color4.x, color4.y, color4.z,
			2,  2, -2,  color4.x, color4.y, color4.z,
			2, -2, -2,  color4.x, color4.y, color4.z,
			2, -2, -2,  color4.x, color4.y, color4.z,
			2, -2,  2,  color4.x, color4.y, color4.z,
			2,  2,  2,  color4.x, color4.y, color4.z,

			-2, -2, -2,  color5.x, color5.y, color5.z,
			2, -2, -2,  color5.x, color5.y, color5.z,
			2, -2,  2,  color5.x, color5.y, color5.z,
			2, -2,  2,  color5.x, color5.y, color5.z,
			-2, -2,  2,  color5.x, color5.y, color5.z,
			-2, -2, -2,  color5.x, color5.y, color5.z,

			-2,  2, -2,  color6.x, color6.y, color6.z,
			2,  2, -2,  color6.x, color6.y, color6.z,
			2,  2,  2,  color6.x, color6.y, color6.z,
			2,  2,  2,  color6.x, color6.y, color6.z,
			-2,  2,  2,  color6.x, color6.y, color6.z,
			-2,  2, -2,  color6.x, color6.y, color6.z,

			0,0,0, 0,0,0
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

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
			ImGui::MenuItem("Projection", NULL, &showProjection);
			ImGui::MenuItem("View", NULL, &showView);
			ImGui::MenuItem("Bonus", NULL, &showBonus);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Color"))
		{
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color1", (float*)&color1); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color2", (float*)&color2); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color3", (float*)&color2); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color4", (float*)&color2); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color5", (float*)&color2); // Edit 3 floats representing a color
			ImGui::ColorEdit3("color6", (float*)&color2); // Edit 3 floats representing a color
			ImGui::EndMenu();
		}
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::EndMainMenuBar();

		if (showProjection) {
			ImGui::Begin("Projection Setting", &showProjection);
			ImGui::Text("Orthographic Projection");
			ImGui::Checkbox("enable1", &orthProj);
			ImGui::SliderFloat2("x", orthParams, -20, 20);
			ImGui::SliderFloat2("y", orthParams + 2, -20, 20);
			ImGui::SliderFloat2("z1", orthParams + 4, -20, 200);
			ImGui::Text("Perspective Projection");
			ImGui::Checkbox("enable2", &persProj);			
			ImGui::SliderFloat("radius", persParams, 0, 360);
			ImGui::SliderFloat2("z2", persParams + 2, 0, 200);
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
		
		if (showProjection) {
			model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5));
			float op = orthProj ? 1 : 0;
			float pp = persProj ? 1 : 0;
			if (op) {
				view = glm::translate(view, glm::vec3(-3.0f, -5.0f, -20.0f));
				project = glm::ortho(orthParams[0], orthParams[1], orthParams[2], orthParams[3], orthParams[4], orthParams[5]);
			}
			else if (pp) {
				view = glm::translate(view, glm::vec3(-3.0f, -5.0f, -20.0f));
				project = glm::perspective(glm::radians(persParams[0]), persParams[1], persParams[2], persParams[3]);
			}
		}

		if (showView) {		// 与投影同时使用	
			// camera
			model = glm::translate(model, glm::vec3(1.5f, -0.5f, 1.5)); // 移回原点
			float radius = 20.0f;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;
			glm::vec3 cPos = glm::vec3(camX, 0.0f, camZ);
			glm::vec3 cFront = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 cUp = glm::vec3(0.0f, 1.0f, 0.0f);
			view = glm::lookAt(cPos, cFront, cUp);
		}		

		if (showBonus) {
			// 鼠标设置
			//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标
			glfwSetCursorPosCallback(window, mouse_callback); // 鼠标位置回调函数
			glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮回调函数
			
			project = glm::perspective(glm::radians(fov), persParams[1], persParams[2], persParams[3]);
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);		
		}

		// 获取uniform地址并设置
		glUseProgram(shaderProgram);
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectLoc = glGetUniformLocation(shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(project));

		// 渲染正方体
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glPointSize(10);
		glDrawArrays(GL_POINTS, 36, 1);
		
		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window); //双缓冲交换避免闪烁
		glfwPollEvents(); //处理IO事件		

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	//释放资源
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}