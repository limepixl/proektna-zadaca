#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Изворен код на шејдер програмите. 
// Зачуван е во овој вид како низа карактери бидејќи е многу
// поедноставен начин за приказ на сите функционалности во една 
// датотека, што е потребно за оваа проектна задача.
const char* vertexS =
"#version 460 core\n"
"layout(location = 0) in vec3 pozicii;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(pozicii, 1.0);\n"
"}\n";

const char* fragmentS =
"#version 460 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

int main()
{
	// Иницијализација на GLFW и специфицање верзија на OpenGL контекст
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	// Создавање на прозорец со димензии 800x600 на главниот монитор
	GLFWwindow* window = glfwCreateWindow(800, 600, "Прозорец", nullptr, nullptr);
	glfwMakeContextCurrent(window);	// Активирање на сегашниот OpenGL контекст

	// Иницијализација на GLAD и проверка дали беше успешна
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
	    std::cout << "ERROR: GLAD init" << std::endl;
	}
	glViewport(0, 0, 800, 600);	// Специфирање на кој дел од прозорецот ќе 
								// биде користен од OpenGL

	float pozicii[]
	{
		-0.5f, -0.5f, 0.0f,	// Долу-лево
		 0.5f, -0.5f, 0.0f, // Долу-десно
		 0.0f,  0.5f, 0.0f  // Врв
	};

	// Создавање низа на атрибути
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Внес на низата на атрибути во scope
	glBindVertexArray(VAO);

	// Создавање атрибут за податоците на точките на триаголникот
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), pozicii, GL_STATIC_DRAW);	// 9 вредности во низата, кои се статични

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);	// Атрибутот ќе се користи во vertex шејдерот
	glEnableVertexAttribArray(0);											// и има 3 вредности за секоја точка (x, y, z)
                                                                            // кои не се нормализирани

	// Деиницијализација на OpenGL променливата GL_ARRAY_BUFFER и променливата за сегашната низа атрибути
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Создавање шејдер на точки
	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexS, 0);
	glCompileShader(vertex);

	// Создавање шејдер на фрагменти
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentS, 0);
	glCompileShader(fragment);

	// Спојување на двете шејдер програми во една главна
	unsigned int glavnaShaderPrograma = glCreateProgram();
	glAttachShader(glavnaShaderPrograma, vertex);
	glAttachShader(glavnaShaderPrograma, fragment);
	glLinkProgram(glavnaShaderPrograma);

	// Бришење на шејдер програмите бидејќи се содржат во главната програма
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	// Главен циклус кој се извршува се додека не се затвори прозорецот
	while(!glfwWindowShouldClose(window))
	{
		// Исполнување на прозорецот со боја после секоја итерација
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);	// Внесување на низата атрибути во scope за OpenGL да знае што да црта
		glUseProgram(glavnaShaderPrograma);	// Користење на главната шејдер програма да знае OpenGL како да црта
		glDrawArrays(GL_TRIANGLES, 0, 3);	// Функцијата за цртање, почнувајќи од првата точка, вкупно 3 точки
											// кои заедно формираат примитив од тип триаголник

		// Замена на стариот исполнет и исцртан прозорец со нов
		// празен прозорец кој следна итерација се исполнува и црта
		// додека претходниот прозорец се прикажува на мониторот
		glfwSwapBuffers(window);
		glfwPollEvents();	// Процесирање на внесни информации
	}

	// Бришење на сите прозорци и сите променливи поврзани со GLFW
	glfwTerminate();

	return 0;
}