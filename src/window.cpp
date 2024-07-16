#include "window.h"

Window::Window() {
    previousTime = glfwGetTime();
    currentTime = glfwGetTime();
    createWindow();
}

Window::~Window() {
    if(window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    for(int i = 0; i < NUM_BIRDS; i++) {
        delete birds[i];
    }
    // glfw: terminate, clearing all previously allocated GLFWresources.
    glfwTerminate();
}

bool Window::createWindow() {
	// glfw: initialize and configure
    if(!glfwInit()) {
		std::cout << "Failed to initiate GLFW window" << std::endl;
		return 0;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //uncomment this statement to fix compilation on OS X
#endif
	// glfw: create window
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Government Birds", NULL, NULL);
	if(!window) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, updateFramesize);
	// glad: load all OpenGL function pointers
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}

    // create birds
    createBirds();

	// render loop
	while(!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        float dTime = currentTime - previousTime;
        previousTime = currentTime;

		// input
		processInput(window);

		// render
		// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        updateBirds(dTime);
        renderBirds();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
    return 1;
}

void Window::processInput(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Window::updateFramesize(GLFWwindow *window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(win) {
        win->screenWidth = width;
        win->screenHeight = height;
    }
	glViewport(0, 0, width, height);

        // update birds with new window size
    for(int i = 0; i < win->NUM_BIRDS; i++) {
        win->birds[i]->setWindowSize(width, height);
    }
}

void Window::createBirds() {
    for(int i = 0; i < NUM_BIRDS; i++) {
        float position[2] = {0.0, 0.0};
        generatePosition(position, i);
        Bird* bird = new Bird(position[0], position[1]);
        bird->createBird();
        bird->setWindowSize(screenWidth, screenHeight);
        birds.push_back(bird);
    }
}

void Window::updateBirds(float dTime) {
    for (int i = 0; i < NUM_BIRDS; i++) {
        birds[i]->updateVelocity(birds);
        birds[i]->updatePosition(dTime);
    }
}

void Window::renderBirds() {
    for(int i = 0; i < NUM_BIRDS; i++) {
        birds[i]->renderBird();
    }
}

void Window::generatePosition(float position[2], int idx) {
    float x = generateRandomFloat(-1, 1);
    float y = generateRandomFloat(-1, 1);
    for(int i = 0; i < idx; i++) {
        while(birds[i]->getPosition()[0] == x) {
            x = generateRandomFloat(-1, 1);
        }
        while(birds[i]->getPosition()[1] == y) {
            y = generateRandomFloat(-1, 1);
        }
    }
    position[0] = x;
    position[1] = y;
}

// generate a random float
float Window::generateRandomFloat(float min, float max) {
    // obtain a random number from hardware
    std::random_device rd;   
    // seed the generator
    std::mt19937 gen(rd());  
    // define the range
    std::uniform_real_distribution<> dis(min, max); 
    return dis(gen);
}