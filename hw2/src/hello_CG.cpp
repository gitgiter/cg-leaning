#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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
const char *vertexShaderSource = "#version 330 core\n"
								 "layout (location = 0) in vec3 aPos;\n"
								 "layout (location = 1) in vec3 aColor;\n"
								 "out vec3 ourColor;\n"
								 "void main()\n"
								 "{\n"
								 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
								 "ourColor = aColor;\n"
								 "}\0";

//片段着色器代码
const char *fragmentShaderSource = "#version 330 core\n"
								   "out vec4 FragColor;\n"
								   "in vec3 ourColor;\n"
								   "void main()\n"
								   "{\n"
								   "   FragColor = vec4(ourColor, 1.0f);\n"
								   "}\n\0";

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// MAC用户

	GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, 1280, 720);	//视口大小

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
	unsigned int shaderProgram;
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

	unsigned int VBO, VAO, EBO;

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330 core";
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
	ImVec4 left_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	ImVec4 right_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 top_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 bottom_color1 = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImVec4 bottom_color2 = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	ImVec4 bottom_color3 = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	
	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window); //持续监听键盘输入

		//三角形顶点，前三个是位置，后三个是颜色
		float vertices[] = {
			//三角形
			-0.2f, 0.0f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.2f, 0.0f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.0f,  0.4f, 0.0f, top_color.x, top_color.y, top_color.z,
			-0.4f,  -0.4f, 0.0f, bottom_color1.x, bottom_color1.y, bottom_color1.z,
			0.0f,  -0.4f, 0.0f, bottom_color2.x, bottom_color2.y, bottom_color2.z,
			0.4f,  -0.4f, 0.0f, bottom_color3.x, bottom_color3.y, bottom_color3.z,
			//线段
			0.8f,  -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.8f,  0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.6f,  0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.6f,  -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			//对角线1			
			0.625f, -0.15f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.65f, -0.1f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.675f, -0.05f, 0.0f, top_color.x, top_color.y, top_color.z,			
			0.725f, 0.05f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.75f, 0.1f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.775f, 0.15f, 0.0f, top_color.x, top_color.y, top_color.z,
			//对角线2
			0.625f, 0.15f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.65f, 0.1f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.675f, 0.05f, 0.0f, right_color.x, right_color.y, right_color.z,			
			0.725f, -0.05f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.75f, -0.1f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.775f, -0.15f, 0.0f, right_color.x, right_color.y, right_color.z,
			//中心点
			0.7f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2, //上面的三角形
			0, 3, 4, //左下的三角形
			1, 4, 5  //右下的三角形
		};

		//创建 VBO, VAO, EBO		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		//先绑定VAO，再绑定其他
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//链接并启用顶点属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //位置属性
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //颜色属性
		glEnableVertexAttribArray(1);

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window); //双缓冲交换避免闪烁
		glfwPollEvents(); //处理IO事件	

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Color Tool Box");                          // Create a window called "Hello, world!" and append into it.

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
		ImGui::ColorEdit3("left color", (float*)&left_color); // Edit 3 floats representing a color
		ImGui::ColorEdit3("right color", (float*)&right_color); // Edit 3 floats representing a color
		ImGui::ColorEdit3("top color", (float*)&top_color); // Edit 3 floats representing a color
		ImGui::ColorEdit3("bottom color1", (float*)&bottom_color1); // Edit 3 floats representing a color
		ImGui::ColorEdit3("bottom color2", (float*)&bottom_color2); // Edit 3 floats representing a color
		ImGui::ColorEdit3("bottom color3", (float*)&bottom_color3); // Edit 3 floats representing a color

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //设置清屏颜色
		glClear(GL_COLOR_BUFFER_BIT); //清屏
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//渲染指令		
		//...
		//启用着色器程序并画三角形
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glLineWidth(2.0f);
		glDrawArrays(GL_LINE_LOOP, 6, 4);
		glPointSize(4.0f);
		glDrawArrays(GL_POINTS, 10, 6);
		glDrawArrays(GL_POINTS, 16, 6);		
		glDrawArrays(GL_POINTS, 22, 1);
		glBindVertexArray(0);
	}

	//按序释放资源	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate(); 
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}

