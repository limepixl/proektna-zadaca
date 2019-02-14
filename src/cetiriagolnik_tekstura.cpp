#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Изворен код на шејдер програмите. 
// Зачуван е во овој вид како низа карактери бидејќи е многу
// поедноставен начин за приказ на сите функционалности во една 
// датотека, што е потребно за оваа проектна задача.
const char* vertexS =
"#version 460 core\n"
"layout(location = 0) in vec3 pozicii;\n"
"layout(location = 1) in vec2 koordinati;\n"
"out vec2 texKoordinati;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(pozicii, 1.0);\n"
"texKoordinati = koordinati;\n"
"}\n";

const char* fragmentS =
"#version 460 core\n"
"in vec2 texKoordinati;\n"
"out vec4 color;\n"
"uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"color = texture(tex, texKoordinati);\n"
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
		-0.5f, -0.5f, 0.0f,	// Долу-лево	// 0
		 0.5f, -0.5f, 0.0f, // Долу-десно	// 1
		 0.5f,  0.5f, 0.0f,  // Горе-десно	// 2
		-0.5f,  0.5f, 0.0f  // Горе-лево	// 3
	};

	unsigned int indeksi[]	// Редослед на цртање на точките (секој триплет за секој триаголник)
	{
		0, 1, 2,
		2, 3, 0
	};

	float koordinatiNaTekstura[]	// Сите пиксели на текстурата ќе се користат
	{								// за квадратот
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(pozicii), pozicii, GL_STATIC_DRAW);	// 12 вредности во низата, кои се статични

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);	// Атрибутот ќе се користи во vertex шејдерот
	glEnableVertexAttribArray(0);											// и има 3 вредности за секоја точка (x, y, z)
                                                                            // кои не се нормализирани

	// Користење на индекс атрибут за намалување на бројот на позиции што треба да се специфираат
	// (мала оптимизација која значи многу со голем број на точки)
	unsigned int IBO;
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indeksi), indeksi, GL_STATIC_DRAW);	// 6 статични индекси во низата

	// Создавање атрибут за координатите на текстурата
	unsigned int tVBO;
	glGenBuffers(1, &tVBO);
	glBindBuffer(GL_ARRAY_BUFFER, tVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(koordinatiNaTekstura), koordinatiNaTekstura, GL_STATIC_DRAW);	// 8 вредности во низата кои се статични

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);	// Втор атрибут кој се користи во vertex шејдерот
	glEnableVertexAttribArray(1);											// кој има 2 вредности за секоја точка (x, y)

	// Деиницијализација на OpenGL променливата GL_ARRAY_BUFFER, 
	// GL_ELEMENT_ARRAY_BUFFER и променливата за сегашната низа атрибути
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	// Користење на stb_image.h за вчитување на слики
	// https://github.com/nothings/stb
	int sirina, dolzina, kanali;	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("img/chess.jpg", &sirina, &dolzina, &kanali, 4);
	if(data == nullptr)
	{
		std::cout << "ERROR: STB_IMAGE" << std::endl;
	}

	// Создавање на текстура
	unsigned int tekstura;
	glGenTextures(1, &tekstura);
	
	// Внес на текстурата во scope
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tekstura);

	// Како да се прикаже текстурата ако е растеризирана помала и поголема од нормални димензии, репсективно
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Создавање на сликата во меѓумеморија, со формат од 3 бојни канали, со дадена ширина и должина
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sirina, dolzina, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Деиницијализација на OpenGL променливите
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(glavnaShaderPrograma);	// Користење на главната шејдер програма да знае OpenGL како да црта

	// Бидејќи се користи текстура, се користи uniform променлива во fragment шејдерот
	// која треба да се дефинира надвор од него. Оваа променлива покажува која текстура
	// од вчитаните (ако има други) треба да се користи. Во овој случај е првата
	int lokacija = glGetUniformLocation(glavnaShaderPrograma, "tex");
	glUniform1i(lokacija, 0);

	// Главен циклус кој се извршува се додека не се затвори прозорецот
	while(!glfwWindowShouldClose(window))
	{
		// Исполнување на прозорецот со боја после секоја итерација
		glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);		

		// За користење на текстурата мора да се внесе во scope
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tekstura);

		glBindVertexArray(VAO);	// Внесување на низата атрибути во scope за OpenGL да знае што да црта
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);	// Користејќи индекс атрибут, мора да се внесе истиот во scope
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	// Бидејќи се користи индекс атрибут, се црта користејќи нова функција
																// која ја гледа точната OpenGL променлива

		// Замена на стариот исполнет и исцртан прозорец со нов
		// празен прозорец кој следна итерација се исполнува и црта
		// додека претходниот прозорец се прикажува на мониторот
		glfwSwapBuffers(window);
		glfwPollEvents();	// Процесирање на внесни информации
	}

	// Ослободување на меморијата која е зафатена од
	// вчитаната слика.
	stbi_image_free(data);

	// Бришење на сите прозорци и сите променливи поврзани со GLFW
	glfwTerminate();

	return 0;
}