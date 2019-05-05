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

//Ƭ����ɫ������
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

// ƽ��
//float translation[3];
bool move_right = true;
float translate_x = 0;
float translate_x_max = 4;
float translate_x_min = -4;
// ��ת, Ĭ�ϰ�x=z��ѡ��
bool x_rotate = true;
bool y_rotate;
bool z_rotate = true;
// ����
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

	bool showTranslation = false;
	bool showRotation = true;
	bool showScaling = false;	
	bool showBonus = false;

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window); //����������������							

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
			0, 1, 2, 0, 2, 3, // ����
			0, 1, 5, 0, 4, 5, // ����
			1, 2, 6, 1, 5, 6, // ǰ��
			2, 3, 7, 2, 6, 7, // ����
			0, 3, 7, 0, 4, 7, // ����
			4, 5, 6, 4, 6, 7  // ����
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

		// ��Ⱦ
		ImGui::Render();
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //����������ɫ
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //����		
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// �任����
		glm::mat4 model = glm::mat4(1.0f); // ��ʼ���ɵ�λ����
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
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 1, 0)); // ��ת
			model = glm::translate(model, glm::vec3(50, 0, 0)); // �뾶
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1, 1, 0)); // ��ת
		}

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 project = glm::mat4(1.0f);
		project = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0, 0, -20));

		// ��ȡuniform��ַ������
		glUseProgram(shaderProgram);
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		unsigned int projectLoc = glGetUniformLocation(shaderProgram, "project");
		glUniformMatrix4fv(projectLoc, 1, GL_FALSE, glm::value_ptr(project));

		// ��Ⱦ������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));

		//��鲢�����¼�����������
		glfwSwapBuffers(window); //˫���彻��������˸
		glfwPollEvents(); //����IO�¼�		

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	
	//�ͷ���Դ
	glfwTerminate();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}