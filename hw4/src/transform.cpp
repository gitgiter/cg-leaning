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
ImVec4 color1 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 color2 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);

// 平移
//float translation[3];
bool move_right = true;
float translate_x = 0;
float translate_x_max = 4;
float translate_x_min = -4;
// 旋转, 默认按x=z轴选择
bool x_rotate = true;
bool y_rotate;
bool z_rotate = true;
// 放缩
//float scaling_max[3] = { 1,1,1 };
//float scaling_min[3] = { 1,1,1 };
bool bigger = true;
float scaling = 1;
float scaling_max = 2;
float scaling_min = 0.2;

unsigned int shaderProgram;
unsigned int VBO, VAO, EBO;

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

	bool showTranslation = false;
	bool showRotation = true;
	bool showScaling = false;	
	bool showBonus = false;

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window); //持续监听键盘输入							

		float vertices[] = {
			// positions          
			-2, -2, -2, color1.x, color1.y, color1.z,
			2, -2, -2, color2.x, color2.y, color2.z,
			2, 2, -2, color1.x, color1.y, color1.z,
			-2, 2, -2, color2.x, color2.y, color2.z,
			-2, -2, 2, color1.x, color1.y, color1.z,
			2, -2, 2, color2.x, color2.y, color2.z,
			2, 2, 2, color1.x, color1.y, color1.z,
			-2, 2, 2, color2.x, color2.y, color2.z
		};
		unsigned int indices[] = {
			0, 1, 2, 0, 2, 3, // 底面
			0, 1, 5, 0, 4, 5, // 左面
			1, 2, 6, 1, 5, 6, // 前面
			2, 3, 7, 2, 6, 7, // 右面
			0, 3, 7, 0, 4, 7, // 后面
			4, 5, 6, 4, 6, 7  // 顶面
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
			ImGui::MenuItem("Translation", NULL, &showTranslation);
			ImGui::MenuItem("Rotation", NULL, &showRotation);
			ImGui::MenuItem("Scaling", NULL, &showScaling);			
			ImGui::MenuItem("Bonus", NULL, &showBonus);			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Color"))
		{			
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("edge color", (float*)&color1); // Edit 3 floats representing a color
			ImGui::ColorEdit3("inside color", (float*)&color2); // Edit 3 floats representing a color
			ImGui::EndMenu();
		}				
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::EndMainMenuBar();				

		if (showTranslation) {
			ImGui::Begin("Translation setting", &showTranslation);
			//ImGui::SliderFloat3("Translation vector: (x,y,z)", translation, -8, 8);
			ImGui::Text("Translation right bound:");
			ImGui::SliderFloat("right", &translate_x_max, 3, 5);
			ImGui::Text("Translation left bound:");
			ImGui::SliderFloat("left", &translate_x_min, -5, -3);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (showRotation) {
			ImGui::Begin("Rotation setting", &showRotation);
			ImGui::Checkbox("X axis", &x_rotate);
			ImGui::Checkbox("Y axis", &y_rotate);
			ImGui::Checkbox("Z axis", &z_rotate);
			ImGui::End();
		}

		if (showScaling) {
			ImGui::Begin("Scaling setting", &showScaling);
			//ImGui::SliderFloat3("Max scaling factor: (x,y,z)", scaling_max, 1, 3);
			//ImGui::SliderFloat3("Min scaling factor: (x,y,z)", scaling_min, 0.1, 1);
			ImGui::Text("Max scaling factor:");
			ImGui::SliderFloat("max", &scaling_max, 1, 2);
			ImGui::Text("Min scaling factor:");
			ImGui::SliderFloat("min", &scaling_min, 0.2, 1);
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
		if (showTranslation) {
			//model = glm::translate(model, glm::vec3(translation[0], translation[1], translation[2]));
			model = glm::translate(model, glm::vec3(translate_x, 0, 0));
			if (move_right) {
				if (translate_x < translate_x_max) translate_x += 0.1;
				else move_right = false;
			}
			else {
				if (translate_x > translate_x_min) translate_x -= 0.1;
				else move_right = true;
			}
		}
		
		if (showRotation) {
			float x = x_rotate ? 1 : 0;
			float y = y_rotate ? 1 : 0;
			float z = z_rotate ? 1 : 0;
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(x, y, z));
		}

		if (showScaling) {
			//model = glm::scale(model, glm::vec3(scaling_max[0], scaling_max[1], scaling_max[2]));			
			//model = glm::scale(model, glm::vec3(scaling_min[0], scaling_min[1], scaling_min[2]));
			model = glm::scale(model, glm::vec3(scaling, scaling, scaling));
			if (bigger) {
				if (scaling < scaling_max) scaling += 0.02;
				else bigger = false;
			}
			else {
				if (scaling > scaling_min) scaling -= 0.02;
				else bigger = true;
			}
		}

		if (showBonus) {
			model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0)); // 公转
			model = glm::translate(model, glm::vec3(50, 0, 0)); // 半径
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 1, 0)); // 自转
		}

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 project = glm::mat4(1.0f);
		project = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0, 0, -20));

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
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window); //双缓冲交换避免闪烁
		glfwPollEvents(); //处理IO事件		

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	
	//释放资源
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}