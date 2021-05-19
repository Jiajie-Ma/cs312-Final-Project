#include "terrain_fbm.h"

using namespace glm;

void init(GLFWwindow* window) {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 1);
   
    cam_ = Camera();
    T_ = Transformations();
    light_ = Light();
    grid_ = Grid();

    grid_.init(render_size, render_size, 256, 256);

    // bind data
    glGenBuffers(1, &vboPosId);
    glBindBuffer(GL_ARRAY_BUFFER, vboPosId);
    glBufferData(GL_ARRAY_BUFFER, 3 * grid_.numVertices() * sizeof(float), grid_.positions(), GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * grid_.numTriangles() * sizeof(unsigned int), grid_.indices(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    glEnableVertexAttribArray(0); // 0 -> Sending VertexPositions to array #0 in the active shader
    glBindBuffer(GL_ARRAY_BUFFER, vboPosId); // always bind before setting data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    T_.init(cam_.getViewMatrix(),cam_.getViewMatrixMirrored(), light_.getLightPos(), window_height_, window_width_, glm::radians(45.0f));
    program_id_ = loadShader("../shaders/terrain_plain.vs", "../shaders/terrain_plain.fs");

    glUseProgram(program_id_);

    grid_.uploadToGPU(program_id_);
    // upload noise parameters
    glUniform1i(glGetUniformLocation(program_id_, "octaves"), octaves);
    glUniform1f(glGetUniformLocation(program_id_, "H"), H);
    glUniform1f(glGetUniformLocation(program_id_, "f"), f);
    glUniform1f(glGetUniformLocation(program_id_, "density"), density);
    glUniform1f(glGetUniformLocation(program_id_, "height"), height);
    glUniform1i(glGetUniformLocation(program_id_, "mode"), mode);
}

void display() {
    /*
    light_.updateLightPos(glfwGetTime());
    */

    cam_.updateCamPos(vec3(dist * sin(glm::radians(azimuth)) * cos(glm::radians(elevation)), dist * sin(glm::radians(elevation)), 
        dist * cos(glm::radians(azimuth)) * cos(glm::radians(elevation))));
    cam_.updateViewMatrix();
    T_.updateTransformations(cam_.getViewMatrix(), cam_.getViewMatrixMirrored(), light_.getLightPos());

    cam_.uploadToGPU(program_id_);
    T_.uploadToGPU(program_id_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, grid_.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);
}

int main() {
    
    initGlfw();
    initWindow();

    window = openWindow("Terrain Rendering");
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
    if (glewInit() != GLEW_OK)
    {
        cout << "Cannot initialize GLEW\n";
        return -1;
    }
#endif

    init(window);

    /// Render loop & keyboard input
    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    closeWindow(window);
}