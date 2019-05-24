#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>
#include <math.h>
using namespace std;

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

struct Point {
	float x;
	float y;
};

int windowWidth = 1280;
int windowHeight = 720;
float T = 1;
vector<Point> points;
float cursor_x;
float cursor_y;
ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
ImVec4 edge_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 curve_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
unsigned int shaderProgram;
unsigned int VBO, VAO;

//画点
void drawPoint(float vertice[], float color[], float pointSize = 2.0f) {
	//创建 VBO, VAO		
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//先绑定VAO，再绑定其他
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
			
	float point[6] = { 0 };
	point[0] = vertice[0];
	point[1] = vertice[1];
	for (int i = 0; i < 3; i++) {
		point[i + 3] = color[i];
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); //重新指定缓存
	//链接并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //颜色属性
	glEnableVertexAttribArray(1);
	//渲染指令
	//...
	//启用着色器程序
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(pointSize);
	glDrawArrays(GL_POINTS, 0, 1);
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//画线
void drawLine(float v1[], float v2[], float lineWidth = 2.0f) {
	//创建 VBO, VAO		
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//先绑定VAO，再绑定其他
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float color[6] = { edge_color.x, edge_color.y, edge_color.z };
	float point[12] = { 0 };
	point[0] = v1[0];
	point[1] = v1[1];
	point[6] = v2[0];
	point[7] = v2[1];
	for (int i = 0; i < 3; i++) {
		point[i + 3] = color[i];
		point[6 + i + 3] = color[i];
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); //重新指定缓存
																		 //链接并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //颜色属性
	glEnableVertexAttribArray(1);
	//渲染指令
	//...
	//启用着色器程序
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);	
	glLineWidth(2.0f);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

int comb(int n, int k) {
	if (n == k || k == 0) return 1;

	// 约分后的分子
	int up = 1;
	int i = n;
	while (i > n - k) {
		up *= i;
		i--;
	}

	// 约分后的分母
	int down = 1;
	i = k;
	while (i > 0) {
		down *= i;
		i--;
	}
	return up / down;
}

Point Q(float t) {
	int n = points.size() - 1;
	float* B = new float[n + 1];	
	for (int i = 0; i <= n; i++) {
		int a = comb(n, i);
		float t1 = pow(t, i);
		float t2 = pow(1 - t, n - i);
		B[i] = a * t1 * t2;
	}

	Point result;
	result.x = 0;
	result.y = 0;
	for (int i = 0; i <= n; i++) {
		result.x += B[i] * points[i].x;
		result.y += B[i] * points[i].y;
	}
	delete B;
	return result;
}

//画bezier曲线
void drawCurve() {	
	float t = 0;
	while (t < T) {
		t += 0.001;
		Point point = Q(t);

		float p[2] = { point.x, point.y };
		float color[3] = { curve_color.x, curve_color.y, curve_color.z };
		drawPoint(p, color);
	}
}

//递归画切线
void drawBonus(vector<Point> v) {
	if (v.size() == 1) {
		float p[2] = { v[0].x, v[0].y };	
		float color[3] = { edge_color.x, edge_color.y, edge_color.z };
		drawPoint(p, color, 8.0);
		return;
	};

	vector<Point> v1;
	for (int i = 0; i < v.size() - 1; i++) {		
		Point p;
		p.x = v[i].x + (v[i + 1].x - v[i].x) * T;
		p.y = v[i].y + (v[i + 1].y - v[i].y) * T;
		v1.push_back(p);
	}

	// 连线
	for (int i = 0; i < v1.size() - 1; i++) {
		float p1[2] = { v1[i].x, v1[i].y };
		float p2[2] = { v1[i + 1].x, v1[i + 1].y };
		drawLine(p1, p2, 1.0);
	}

	drawBonus(v1);
}

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
	float speed = 0.01;
	if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && T - speed >= 0)
		T -= speed;
	if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && T + speed <= 1)
		T += speed;
}

//处理鼠标点击
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		Point point;
		point.x = cursor_x;
		point.y = cursor_y;
		points.push_back(point);
		int n = points.size() - 1;
		for (int i = 0; i <= n; i++) {
			cout << n << ' ' << i << ' ' << comb(n, i) << endl;
		}
	}
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (points.size() > 0)
			points.pop_back();
	}	
}

//监听鼠标位置
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	cursor_x = (x - windowWidth / 2) / windowWidth * 2;
	cursor_y = 0 - (y - windowHeight / 2) / windowHeight * 2;	
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

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
		
	bool showBonus = true;	

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
			ImGui::MenuItem("Bonus", NULL, &showBonus);			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Color"))
		{			
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("edge color", (float*)&edge_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("curve color", (float*)&curve_color); // Edit 3 floats representing a color
			ImGui::EndMenu();
		}						
		ImGui::EndMainMenuBar();				

		if (showBonus) {
			ImGui::Begin("Bonus", &showBonus);
			ImGui::SliderFloat("t", &T, 0, 1);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 渲染
		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //设置清屏颜色
		glClear(GL_COLOR_BUFFER_BIT); //清屏
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());									

		if (points.size() > 1) {
			// draw curve		
			drawCurve();
			if (showBonus) {
				drawBonus(points);
			}
		}

		// 画控制点及其之间的连线
		for (int i = 0; i < points.size(); i++) {
			Point point1 = points[i];
			float p1[2] = { point1.x, point1.y };
			float color[3] = { edge_color.x, edge_color.y, edge_color.z };
			drawPoint(p1, color, 8.0f);
			if (i + 1 < points.size()) {
				Point point2 = points[i + 1];
				float p2[2] = { point2.x, point2.y };
				drawLine(p1, p2);
			}
		}
		
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