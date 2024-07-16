#include "bird.h"

Bird::Bird(float x, float y) {
    position[0] = x;
    position[1] = y;
}

Bird::~Bird() {
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteProgram(shader);
}

void Bird::createBird() {
    // compile and link shaders
    shader = createShader(vertexShader, fragmentShader);

    // set up vertex data, buffers, configure vertex attributes
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    // bind vertex array object
    glBindVertexArray(vertexArray);

    // bind vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // configure vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind vertex array and buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Bird::renderBird() {
    glUseProgram(shader);

    int offsetLocation = glGetUniformLocation(shader, "offset");
    glUniform2f(offsetLocation, position[0], position[1]);

    int rotationLocation = glGetUniformLocation(shader, "rotation");
    glUniform1f(rotationLocation, angle);

    glBindVertexArray(vertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void Bird::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    updateVertices();
    setProjection(static_cast<float>(width) / height);
}

float* Bird::getPosition() {
    return position;
}

void Bird::updatePosition(float dTime) {
    position[0] += velocity[0] * dTime;
    position[1] += velocity[1] * dTime;

    // wrap around screen edges
    if (position[0] > 1.0) position[0] = -1.0;
    if (position[0] < -1.0) position[0] = 1.0;
    if (position[1] > 1.0) position[1] = -1.0;
    if (position[1] < -1.0) position[1] = 1.0;

    angle = atan2(velocity[0], velocity[1]);
}

float* Bird::getVelocity() {
    return velocity;
}

void Bird::updateVelocity(std::vector<Bird*>& birds) {
    separation(birds);
    alignment(birds);
    cohesion(birds);

    // ensure the velocity remains within bounds
    float speed = sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]);
    if (speed > 0.2) {
        velocity[0] = (velocity[0] / speed) * 0.2;
        velocity[1] = (velocity[1] / speed) * 0.2;
    }
}


bool Bird::isNeighbor(Bird* bird) {
    if((calculateDistance(bird) < cohesionRadius)) {
        return 1;
    }
    return 0;
}

unsigned int Bird::compileShader(unsigned int type, const char* source) {
    // create shader object
    unsigned int id = glCreateShader(type);
    // provide source code to shader
    glShaderSource(id, 1, &source, nullptr);
    // compile shader
    glCompileShader(id);

    // check compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return id;
}

unsigned int Bird::createShader(const char* vertexSource, const char* fragmentSource) {
    // compile vertex and fragment shader
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // create shader program
    unsigned int program = glCreateProgram();
    // attach shaders to program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // link shaders to program
    glLinkProgram(program);

    // check linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // shaders linked to program, discard objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Bird::setProjection(float aspectRatio) {
    float left = -aspectRatio;
    float right = aspectRatio;
    float bottom = -1.0f;
    float top = 1.0f;
    float near = -1.0f;
    float far = 1.0f;

    float ortho[16] = {
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / (far - near), 0.0f,
        -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1.0f
    };

    glUseProgram(shader);
    int projectionLocation = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, ortho);
}


void Bird::updateVertices() {
    for(int i = 0; i < 9; i += 3) {
        vertices[i] = originalVertices[i] / (windowWidth / 2.0);
        vertices[i + 1] = originalVertices[i + 1] / (windowHeight / 2.0);
        vertices[i + 2] = originalVertices[i + 2];
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Bird::cohesion(std::vector<Bird*>& birds) {
    float avgPosX = 0.0;
    float avgPosY = 0.0;
    int count = 0;

    for(int i = 0; i < birds.size(); i++) {
        Bird* bird = birds[i];
        if (bird == this) {
            continue;
        }
        float distance = calculateDistance(bird);
        if (distance < cohesionRadius) {
            avgPosX += bird->getPosition()[0];
            avgPosY += bird->getPosition()[1];
            count++;
        }
    }

    if(count > 0) {
        avgPosX /= count;
        avgPosY /= count;
        velocity[0] += (avgPosX - position[0]) * 0.05;
        velocity[1] += (avgPosY - position[1]) * 0.05;
    }
}


void Bird::separation(std::vector<Bird*>& birds) {
    float separationX = 0.0;
    float separationY = 0.0;
    int count = 0;

    for(int i = 0; i < birds.size(); i++) {
        Bird* bird = birds[i];
        if(bird == this) {
            continue;
        }
        float distance = calculateDistance(bird);
        if(distance < separationRadius) {
            separationX += position[0] - bird->getPosition()[0];
            separationY += position[1] - bird->getPosition()[1];
            count++;
        }
    }

    if(count > 0) {
        separationX /= count;
        separationY /= count;
        float length = sqrt(separationX * separationX + separationY * separationY);
        if(length > 0) {
            separationX /= length;
            separationY /= length;
        }
        velocity[0] += separationX * 0.05;
        velocity[1] += separationY * 0.05;
    }
}


void Bird::alignment(std::vector<Bird*>& birds) {
    float avgVelX = 0.0;
    float avgVelY = 0.0;
    int count = 0;

    for(int i = 0; i < birds.size(); i++) {
        Bird* bird = birds[i];
        if(bird == this) {
            continue;
        }
        float distance = calculateDistance(bird);
        if(distance < cohesionRadius) {
            avgVelX += bird->getVelocity()[0];
            avgVelY += bird->getVelocity()[1];
            count++;
        }
    }

    if(count > 0) {
        avgVelX /= count;
        avgVelY /= count;
        velocity[0] += (avgVelX - velocity[0]) * 0.05;
        velocity[1] += (avgVelY - velocity[1]) * 0.05;
    }
}


float Bird::calculateDistance(Bird* bird) {
    float dx = position[0] - bird->getPosition()[0];
    float dy = position[1] - bird->getPosition()[1];
    return sqrt(dx * dx + dy * dy);
}