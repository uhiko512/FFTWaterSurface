#include "stdafx.h"
#include "Renderer.h"


#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>

void FT() {
	const unsigned int N = 16;

	std::cout << "DFT" << std::endl;
	for (unsigned int k = 0; k < N; k++) {
		std::complex<double> sum(0.0, 0.0);
		for (unsigned int j = 0; j < N; j++) {
			int integers = -2 * j * k;
			std::complex<double> my_exponent(0.0, M_PI * (double)integers / N );
			sum += sin((double) j + 1) * std::exp(my_exponent);
		}
		std::cout << abs(sum) << std::endl;
	}

	std::cout << std::endl << "FFT" << std::endl;

	std::vector<std::complex<float>> data;
	for (int i = 0; i < N; i++) {
		data.push_back(std::complex<float>(sin((float) i + 1.0f), 0.0f));
	}

	std::vector<std::complex<float>> revData(N);
	for (int i = 0; i < N; i++) {
		uint32_t revIndex = 0, index = i;
		for (int j = 0; j < log2(N); j++) {
			revIndex <<= 1;
			revIndex |= index & 1;
			index >>= 1;
		}
		revData[revIndex] = data[i];
	}

	for (int i = 2; i <= N; i <<= 1) {
		std::complex<float> dw = std::exp(std::complex<float>(0.0, -2.0f * M_PI / i));
		for (int j = 0; j < N; j += i) {
			std::complex<float> w(1, 0);
			for (int k = 0; k < i >> 1; k++) {
				std::complex<float> a = revData[j + k];
				std::complex<float> b = revData[j + k + (i >> 1)] * w;
				revData[j + k] = a + b;
				revData[j + k + (i >> 1)] = a - b;
				w *= dw;
				std::cout << j + k << " " << j + k + (i >> 1) << std::endl;
			}
		}
	}

	for (auto a : revData) {
		std::cout << abs(a) << std::endl;
	}
}

int debug = 0;
void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (debug < 3) {
		fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			type, severity, message);
	}
	debug++;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void cursorCenter(GLFWwindow* window);

int currentKey = 0;
bool isKeyPressed = false;
glm::ivec2 screenSize;
glm::vec2 screenCenter;
std::shared_ptr<Renderer> renderer;

int main() {

	if (!glfwInit()) {
		throw std::system_error(std::error_code(), "Failed to initialize GLFW");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window;
	window = glfwCreateWindow(1280, 720, "Water Suface", nullptr, nullptr);
	if (window == nullptr) {
		throw std::system_error(std::error_code(), "Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw std::system_error(std::error_code(), "Failed to initialize GLEW");
		return -1;
	}

	glfwGetFramebufferSize(window, &screenSize.x, &screenSize.y);
	screenCenter = glm::vec2(screenSize.x * 0.5f, screenSize.y * 0.5f);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC) MessageCallback, 0);

	renderer = std::make_shared<Renderer>(window);

	//FT();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (isKeyPressed) {
			glm::vec3 move;
			glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
			if (currentKey == GLFW_KEY_D) {
				move += X;
			} else if (currentKey == GLFW_KEY_A) {
				move -= X;
			}

			glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);
			if (currentKey == GLFW_KEY_W) {
				move += Z;
			} else if (currentKey == GLFW_KEY_S) {
				move -= Z;
			}
			renderer->keyEvent(move);
		}

		renderer->exec();
		/*for (auto t : ts) {
			t->output();
		}
		(*a)++;*/

		glfwSwapBuffers(window);
	}

	glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		isKeyPressed = true;
		currentKey = key;
	} else if (action == GLFW_RELEASE) {
		isKeyPressed = false;
	}
	
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	glm::vec2 rotation;
	rotation.x = (float) ((screenSize.x - xpos) - screenCenter.x) / screenCenter.x;
	rotation.y = (float) ((screenSize.y - ypos) - screenCenter.y) / screenCenter.y;
	renderer->mouseEvent(rotation);
	cursorCenter(window);
}

void cursorCenter(GLFWwindow* window) {
	glfwSetCursorPos(window, screenCenter.x, screenCenter.y);
}
