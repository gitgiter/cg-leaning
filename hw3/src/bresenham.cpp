#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//���ڴ�С�ı�Ļص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//�����������
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); //�ر�glfw����Ӧwhileѭ���Ǹ��ж�����
}

//������ɫ������
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"ourColor = aColor;\n"
"}\0";

//Ƭ����ɫ������
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int windowWidth = 1280;
int windowHeight = 720;
ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.0f);
ImVec4 edge_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 inside_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
unsigned int shaderProgram;
unsigned int VBO, VAO;

//����
void drawPoint(int vertice[], float color[]) {	
	float point[6] = { 0 };
	point[0] = float(vertice[0]) / float(windowWidth);
	point[1] = float(vertice[1]) / float(windowHeight);
	for (int i = 0; i < 3; i++) {
		point[i + 3] = color[i];
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); //����ָ������
	//���Ӳ����ö�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //λ������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //��ɫ����
	glEnableVertexAttribArray(1);
	//��Ⱦָ��
	//...
	//������ɫ������
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(4.0f);
	glDrawArrays(GL_POINTS, 0, 1);
}

//����
void drawLine(int v1[], int v2[]) {
	float color[3] = { edge_color.x, edge_color.y, edge_color.z };
	drawPoint(v1, color);
		
	int dx = abs(v2[0] - v1[0]);	
	int dy = abs(v2[1] - v1[1]);
	int sign_x = (v2[0] - v1[0]) <= 0 ? -1 : 1;
	int sign_y = (v2[1] - v1[1]) <= 0 ? -1 : 1;
	if (dx == 0) {
		int start_point[2] = { v1[0], v1[1] };
		int end_point[2] = { v2[0], v2[1] };
		//б�ʲ�����
		while (start_point[1] != end_point[1]) {
			drawPoint(start_point, color);
			start_point[1] += sign_y;
		}
		return;
	}
	if (dx >= dy) {
		// k <= 1
		int start_point[2] = { v1[0], v1[1] };
		int end_point[2] = { v2[0], v2[1] };
		int dp1 = 2 * dy;
		int dp2 = 2 * (dy - dx);
		int p = 2 * dy - dx;
		while (start_point[0] != end_point[0]) {
			if (p <= 0) {
				start_point[0] += sign_x; //x++ or x--
				drawPoint(start_point, color);
				p += dp1;
			}
			else {
				start_point[0] += sign_x; //x++ or x--
				start_point[1] += sign_y; //y++ or y--
				drawPoint(start_point, color);
				p += dp2;
			}
		}
	}
	else {
		// k > 1������x��y����
		int start_point[2] = { v1[0], v1[1] };
		int end_point[2] = { v2[0], v2[1] };
		int dp1 = 2 * dx;
		int dp2 = 2 * (dx - dy);
		int p = 2 * dx - dy;
		while (start_point[1] != end_point[1]) {
			if (p <= 0) {
				start_point[1] += sign_y; //y++ or y--
				drawPoint(start_point, color);
				p += dp1;
			}
			else {
				start_point[1] += sign_y; //y++ or y--
				start_point[0] += sign_x; //x++ or x--
				drawPoint(start_point, color);
				p += dp2;
			}
		}
	}
	drawPoint(v2, color);
}

//��������
void drawTriangle(int v1[], int v2[], int v3[]) {			
	//���� VBO, VAO		
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//�Ȱ�VAO���ٰ�����
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	

	drawLine(v1, v2);
	drawLine(v2, v3);
	drawLine(v3, v1);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

//��Բ
void drawCircle(int circle[]) {
	//���� VBO, VAO		
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//�Ȱ�VAO���ٰ�����
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//���Ӳ����ö�������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //λ������
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //��ɫ����
	glEnableVertexAttribArray(1);

	float color[3] = { edge_color.x, edge_color.y, edge_color.z };
	int center_x = circle[0];
	int center_y = circle[1];
	int r = circle[2];
	int center[2] = { center_x, center_y };
	drawPoint(center, color);
	int p = 3 - 2 * r;

	int x = 0;
	int y = r;
	while (x <= y) {
		int point[8][2] = {
			x, y,
			y, x,
			y, -x,
			x, -y,
			-x, -y,
			-y, -x,
			-y, x,
			-x, y
		};
		for (int i = 0; i < 8; i++) {
			point[i][0] += center_x;
			point[i][1] += center_y;
			drawPoint(point[i], color);
		}
		if (p < 0) {
			p += 4 * x + 6;
		}
		else {
			p += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

int min3(int v1, int v2, int v3) {	
	int arr[3] = { v1,v2,v3 };
	int min = 10000;
	for (int i = 0; i < 3; i++)
		if (arr[i] < min) 
			min = arr[i];
	return min;
}

int max3(int v1, int v2, int v3) {
	int arr[3] = { v1,v2,v3 };
	int max = -10000;
	for (int i = 0; i < 3; i++)
		if (arr[i] > max)
			max = arr[i];
	return max;
}

void bound(int v1[], int v2[], int v3[], int* box) {
	box[0] = min3(v1[0], v2[0], v3[0]); // xmin
	box[1] = max3(v1[0], v2[0], v3[0]); // xmax
	box[2] = min3(v1[1], v2[1], v3[1]); // ymin
	box[3] = max3(v1[1], v2[1], v3[1]); // ymax
}

bool inside(int lines[3][3], int point[]) {	
	int p_sign = 0;
	int n_sign = 0;
	for (int i = 0; i < 3; i++) {
		//��Ϊͬ���ż��飬����Ӧ�����������ͬС�����ͬ����0��Ҫ��
		if (lines[i][0] * point[0] + lines[i][1] * point[1] + lines[i][2] > 0) {
			p_sign++;
		}
		else if (lines[i][0] * point[0] + lines[i][1] * point[1] + lines[i][2] < 0) {
			n_sign++;
		}
	}
	//ͬ��һ�߷���true
	if (p_sign == 3 || n_sign == 3) return true;
	else return false;
}

void rasterize(int v1[], int v2[], int v3[]) {
	//���� VBO, VAO		
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//�Ȱ�VAO���ٰ�����
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//������Ӿ���
	int box[4] = { 0 };
	bound(v1, v2, v3, box);
	//����ߵı�ʾ
	int v1v2_dx = v2[0] - v1[0];
	int v1v2_dy = v2[1] - v1[1];
	int v2v3_dx = v3[0] - v2[0];
	int v2v3_dy = v3[1] - v2[1];
	int v3v1_dx = v1[0] - v3[0];
	int v3v1_dy = v1[1] - v3[1];
	//������ʽ�����A��B��C�ı�ʾ��
	int lines[3][3] = {
		v1v2_dy, -v1v2_dx, v2[1] * v1v2_dx - v2[0] * v1v2_dy, // line1: v1---v2
		v2v3_dy, -v2v3_dx, v3[1] * v2v3_dx - v3[0] * v2v3_dy, // line2: v2---v3
		v3v1_dy, -v3v1_dx, v3[1] * v3v1_dx - v3[0] * v3v1_dy  // line3: v3---v1
	};
	float color[3] = { inside_color.x, inside_color.y, inside_color.z };
	for (int i = box[0]; i < box[1]; i+=10)
		for (int j = box[2]; j < box[3]; j+=10) {
			int point[2] = { i,j };
			if (inside(lines, point)) 
				drawPoint(point, color);
		}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// MAC�û�

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//��ȡglfw�����к���ָ���ַ
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);	//�ӿڴ�С

	//ע��ص��¼�
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//���붥����ɫ��
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//���������
	int success1;
	char infoLog1[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success1);
	if (!success1) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog1);
		std::cout << "Shader compile error\n" << infoLog1 << std::endl;
	}

	//����Ƭ����ɫ��
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//���������
	int success2;
	char infoLog2[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success2);
	if (!success2) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog2);
		std::cout << "Shader compile error\n" << infoLog2 << std::endl;
	}

	//������������ɫ������	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//������Ӵ���
	int success3;
	char infoLog3[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success3);
	if (!success3) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog3);
		std::cout << "Program link error\n" << infoLog3 << std::endl;
	}

	//ɾ����ɫ������
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
	
	int v1[2] = { 0 };
	int v2[2] = { 0 };
	int v3[2] = { 0 };
	int circle[3] = { 0 };
	bool showLine = false;
	bool showTriangle = false;
	bool showCircle = false;
	bool fillTriangle = false;

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window); //����������������							

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();
		
		if (ImGui::BeginMenu("Tools")) {
			//ImGui::MenuItem("show line", NULL, &showLine);
			ImGui::MenuItem("show triangle", NULL, &showTriangle);
			ImGui::MenuItem("show circle", NULL, &showCircle);			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Color"))
		{			
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("edge color", (float*)&edge_color); // Edit 3 floats representing a color
			ImGui::ColorEdit3("inside color", (float*)&inside_color); // Edit 3 floats representing a color
			ImGui::EndMenu();
		}				
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::EndMainMenuBar();				

		//if (showLine) {
		//	ImGui::Begin("line setting", &showLine);
		//	ImGui::SliderInt2("v1(x,y)", v1, -windowWidth / 2, windowHeight / 2);
		//	ImGui::SliderInt2("v2(x,y)", v2, -windowWidth / 2, windowHeight / 2);			
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//	ImGui::End();
		//}
		if (showTriangle) {
			//ȡ��Բ����ʾ
			showCircle = false;
			ImGui::Begin("triangle setting", &showTriangle);
			ImGui::SliderInt2("v1(x,y)", v1, -windowWidth / 2, windowHeight / 2);
			ImGui::SliderInt2("v2(x,y)", v2, -windowWidth / 2, windowHeight / 2);
			ImGui::SliderInt2("v3(x,y)", v3, -windowWidth / 2, windowHeight / 2);	
			ImGui::Checkbox("fill triangle", &fillTriangle);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		if (showCircle) {
			//ȡ�������ε���ʾ�����
			showTriangle = false;
			fillTriangle = false; 
			ImGui::Begin("circle setting", &showCircle);
			ImGui::SliderInt3("circle(x,y,r)", circle, -windowWidth / 2, windowHeight / 2);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// ��Ⱦ
		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //����������ɫ
		glClear(GL_COLOR_BUFFER_BIT); //����
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());						
		
		//if (showLine) drawLine(v1, v2);
		if (showTriangle) drawTriangle(v1, v2, v3);
		if (showCircle) drawCircle(circle);
		if (fillTriangle) rasterize(v1, v2, v3);

		//��鲢�����¼�����������
		glfwSwapBuffers(window); //˫���彻��������˸
		glfwPollEvents(); //����IO�¼�		
	}

	//�ͷ���Դ	
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}