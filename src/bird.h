#ifndef BIRD_H
#define BIRD_H

#include "libraries.h"

class Bird {
    public:
        Bird(float x, float y);
        ~Bird();
        void createBird();
        void renderBird();
        void setWindowSize(int width, int height);
        float* getPosition();
        void updatePosition(float dTime);
        float* getVelocity();
        void updateVelocity(std::vector<Bird*>& birds);
        bool isNeighbor(Bird* bird);

    private:
        unsigned int vertexArray = 0;
        unsigned int vertexBuffer = 0;
        unsigned int shader = 0;
        float vertices[9];
        float originalVertices[9] = {
            0.0f, 5.0f, 0.0f,  // top
            -5.0f, -5.0f, 0.0f,  // bottom left
            5.0f, -5.0f, 0.0f   // bottom right
        };

        float position[2] = {0.0, 0.0};
        float velocity[2] = {0.4, 0.4};
        float velocityBoundary[2] = {-0.4, 0.4};
        float angle = 0.0;
        float cohesionRadius = 0.2;
        float separationRadius = 0.1;
        int windowWidth;
        int windowHeight;

        unsigned int compileShader(unsigned int type, const char* source);
        unsigned int createShader(const char* vertexShader, const char* fragmentShader);
        
        void setProjection(float aspectRatio);
        void updateVertices();
        void cohesion(std::vector<Bird*>& birds);
        void separation(std::vector<Bird*>& birds);
        void alignment(std::vector<Bird*>& birds);
        float calculateDistance(Bird* bird);

        // vertex shader
        const char* vertexShader = R"glsl(
            #version 330 core
            layout(location = 0) in vec3 position;
            uniform mat4 projection;
            uniform vec2 offset;
            uniform float rotation;
            void main() {
                float cosTheta = cos(rotation);
                float sinTheta = sin(rotation);
                mat3 rotationMatrix = mat3(
                    cosTheta, -sinTheta, 0.0,
                    sinTheta, cosTheta, 0.0,
                    .0, 0.0, 1.0
                );
                vec3 rotatedPosition = rotationMatrix * position;
                gl_Position = projection * vec4(rotatedPosition + vec3(offset, 0.0), 1.0);
            }
        )glsl";
        // fragment shader
        const char* fragmentShader = R"glsl(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0, 1.0, 1.0, 1.0);
            }
        )glsl";
};

#endif // BIRD_H