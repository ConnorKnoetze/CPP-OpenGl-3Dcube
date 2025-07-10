#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vertexColor = aColor;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 vertexColor;

out vec4 FragColor;
void main() {
    FragColor = vec4(vertexColor, 1.0f);
}
)";


// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Initial camera position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Direction the camera is facing
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up vector
float cameraSpeed = 2.5f; // Speed of camera movement

// Mouse settings
float yaw = -90.0f; // Initial yaw (looking along the negative Z-axis)
float pitch = 0.0f; // Initial pitch
float lastX = 400.0f; // Initial mouse X position (center of the screen)
float lastY = 300.0f; // Initial mouse Y position (center of the screen)
bool firstMouse = true; // To ignore the first mouse movement
float mouseSensitivity = 0.1f; // Mouse sensitivity

// Function to process input
void processInput(GLFWwindow* window, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // Move forward
        cameraPos += velocity * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Move backward
        cameraPos -= velocity * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // Move left
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Move right
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Move up
        cameraPos += cameraUp * velocity; // Move up
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) // Move down
        cameraPos -= cameraUp * velocity; // Move down

	// Close the window if Escape is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Exit on Escape
        glfwSetWindowShouldClose(window, true);
    
}

// Mouse callback function to handle mouse movement
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // Constrain the pitch to avoid flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update the camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main(void)
{   
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL Fullscreen", monitor, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return -1;
    }

	// Set the viewport
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)); // Move the camera back

    // Set the viewport to match the fullscreen resolution
    glViewport(0, 0, mode->width, mode->height);

    // Set up the perspective projection matrix
    float aspect = static_cast<float>(mode->width) / static_cast<float>(mode->height);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    float vertices[] = {
        // Positions          // Colors
        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  

        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  

        // Left face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  

        // Right face
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  
         0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  

         // Top face
         -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  
          0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  
          0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  
         -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  

         // Bottom face
         -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  
          0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  
          0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  
         -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f   
    };

    unsigned int indices[] = {
        // Front face
        0, 1, 2,
        2, 3, 0,

        // Back face
        4, 5, 6,
        6, 7, 4,

        // Left face
        8, 9, 10,
        10, 11, 8,

        // Right face
        12, 13, 14,
        14, 15, 12,

        // Top face
        16, 17, 18,
        18, 19, 16,

        // Bottom face
        20, 21, 22,
        22, 23, 20
    };

	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Bind the Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the Element Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
	// Define the vertex attributes
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Use the shader program
    glUseProgram(shaderProgram);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
    float deltaTime = 0.0f; // Time between current frame and last frame
    float lastFrame = 0.0f; // Time of the last frame


	// Main loop
	glClearColor(0.4f, 0.0f, 0.0f, 1.0f); // Set clear color
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        // Rendering code
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
	glfwDestroyWindow(window);
	// Terminate GLFW
    glfwTerminate();
    return 0;
}