#ifndef WINDOW_H
#define WINDOW_H

#include "libraries.h"
#include "bird.h"

class Window {
    public:
        Window();
        ~Window();
        bool createWindow();

    private:
        const unsigned int SCR_WIDTH = 1000;
        const unsigned int SCR_HEIGHT = 1000;
        const unsigned int NUM_BIRDS = 128;
        unsigned int screenWidth = SCR_WIDTH;
        unsigned int screenHeight = SCR_HEIGHT;
        float previousTime;
        float currentTime;
        GLFWwindow* window = nullptr;
        std::vector<Bird*> birds;

        static void processInput(GLFWwindow *window);
        static void updateFramesize(GLFWwindow *window, int width, int height);
        void createBirds();
        void updateBirds(float dTime);
        void renderBirds();
        void generatePosition(float position[2], int idx);
        float generateRandomFloat(float min, float max);
};

#endif // WINDOW_H