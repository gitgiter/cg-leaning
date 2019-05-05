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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// MAC�û�

	GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
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

	glViewport(0, 0, 1280, 720);	//�ӿڴ�С

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
	unsigned int shaderProgram;
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
	
	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window); //����������������

		//�����ζ��㣬ǰ������λ�ã�����������ɫ
		float vertices[] = {
			//������
			-0.2f, 0.0f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.2f, 0.0f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.0f,  0.4f, 0.0f, top_color.x, top_color.y, top_color.z,
			-0.4f,  -0.4f, 0.0f, bottom_color1.x, bottom_color1.y, bottom_color1.z,
			0.0f,  -0.4f, 0.0f, bottom_color2.x, bottom_color2.y, bottom_color2.z,
			0.4f,  -0.4f, 0.0f, bottom_color3.x, bottom_color3.y, bottom_color3.z,
			//�߶�
			0.8f,  -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.8f,  0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.6f,  0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			0.6f,  -0.2f, 0.0f, left_color.x, left_color.y, left_color.z,
			//�Խ���1			
			0.625f, -0.15f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.65f, -0.1f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.675f, -0.05f, 0.0f, top_color.x, top_color.y, top_color.z,			
			0.725f, 0.05f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.75f, 0.1f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.775f, 0.15f, 0.0f, top_color.x, top_color.y, top_color.z,
			//�Խ���2
			0.625f, 0.15f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.65f, 0.1f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.675f, 0.05f, 0.0f, right_color.x, right_color.y, right_color.z,			
			0.725f, -0.05f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.75f, -0.1f, 0.0f, right_color.x, right_color.y, right_color.z,
			0.775f, -0.15f, 0.0f, right_color.x, right_color.y, right_color.z,
			//���ĵ�
			0.7f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
		};

		unsigned int indices[] = {
			0, 1, 2, //�����������
			0, 3, 4, //���µ�������
			1, 4, 5  //���µ�������
		};

		//���� VBO, VAO, EBO		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		//�Ȱ�VAO���ٰ�����
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//���Ӳ����ö�������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //λ������
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //��ɫ����
		glEnableVertexAttribArray(1);

		//��鲢�����¼�����������
		glfwSwapBuffers(window); //˫���彻��������˸
		glfwPollEvents(); //����IO�¼�	

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
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w); //����������ɫ
		glClear(GL_COLOR_BUFFER_BIT); //����
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//��Ⱦָ��		
		//...
		//������ɫ�����򲢻�������
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

	//�����ͷ���Դ	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glfwTerminate(); 
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	return 0;
}

