#include "main.h"

using namespace glm;

void init(GLFWwindow* window) {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);
    glClearColor(0, 0, 0, 1);
   
    glfwGetFramebufferSize(window, &window_width_, &window_height_);
    cam_ = Camera();
    T_ = Transformations();
    light_ = Light();
    grid_ = Grid();

    grid_.init(render_size, render_size, 256, 256);
    
    // generate noise here
    /*
    for (int i = 0; i < tex_size; i++){
        for (int j = 0; j < tex_size; j++){
            float h = hyrbidMultifractal(vec2((float)i/tex_size, (float)j/tex_size),  H, f, octaves, offset);
            if (h < 0){
                h = 0;
            }
            noise_tex.set_vec3(j, i, vec3(h, 0.0f, 0.0f));
        }
    }

    noise_tex.save("../textures/noise.png");
    */
    
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

    T_.init(cam_.getViewMatrix(), cam_.getViewMatrixMirrored(), light_.getLightPos(), window_height_, window_width_, glm::radians(45.0f));
    program_id_ = loadShader("../shaders/terrain.vs", "../shaders/terrain.fs");

    glUseProgram(program_id_);

    // upload parameters
    grid_.uploadToGPU(program_id_);
    glUniform1f(glGetUniformLocation(program_id_, "max_height"), max_height);

    loadTextureFromFile("../textures/noise.png", &texture_noise_id, true);
    loadTextureFromFile("../textures/sand.bmp", &texture_sand_id, true);
    loadTextureFromFile("../textures/snow.bmp", &texture_snow_id, true);
    loadTextureFromFile("../textures/grass.tga", &texture_grass_id, true);
    loadTextureFromFile("../textures/rock.bmp", &texture_rock_id, true);

    glUseProgram(0);
    glBindVertexArray(0);

    static const vec3 sky_vertices[] = {
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(0.5, -0.5, 0.5),
    vec3(0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, -0.5),
    vec3(-0.5, 0.5, 0.5),
    vec3(-0.5, -0.5, 0.5),
    vec3(0.5, 0.5, -0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, -0.5),
    vec3(0.5, 0.5, 0.5),
    vec3(-0.5, 0.5, 0.5)
    };

    static const  vec2 sky_UVs[] = {
    vec2(0.333, 0.75),
    vec2(0.666, 0.75),
    vec2(0.333, 0.5),
    vec2(0.666, 0.75),
    vec2(0.333, 0.5),
    vec2(0.666, 0.5),

    vec2(0.666, 0.25),
    vec2(0.333, 0.25),
    vec2(0.666, 0.5),
    vec2(0.333, 0.25),
    vec2(0.666, 0.5),
    vec2(0.333, 0.5),

    vec2(0.666, 0.25),
    vec2(0.666, 0.0),
    vec2(0.333, 0.25),
    vec2(0.666, 0.0),
    vec2(0.333, 0.25),
    vec2(0.333, 0.0),

    vec2(0.0, 0.75),
    vec2(0.333, 0.75),
    vec2(0.0, 0.5),
    vec2(0.333, 0.75),
    vec2(0.0, 0.5),
    vec2(0.333, 0.5),

    vec2(0.666, 0.75),
    vec2(0.333, 0.75),
    vec2(0.666, 1.0),
    vec2(0.333, 0.75),
    vec2(0.666, 1.0),
    vec2(0.333, 1.0),

    vec2(0.666, 0.5),
    vec2(0.666, 0.75),
    vec2(1.0, 0.5),
    vec2(0.666, 0.75),
    vec2(1.0, 0.5),
    vec2(1.0, 0.75)
    };

    glGenBuffers(1, &sky_vboPosId);
    glBindBuffer(GL_ARRAY_BUFFER, sky_vboPosId);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(vec3), sky_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &sky_vboTexId);
    glBindBuffer(GL_ARRAY_BUFFER, sky_vboTexId);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(vec3), sky_UVs, GL_STATIC_DRAW);

    glGenVertexArrays(1, &sky_vaoId);
    glBindVertexArray(sky_vaoId);

    glEnableVertexAttribArray(1); // 1 -> Sending VertexPositions to array #0 in the active shader
    glBindBuffer(GL_ARRAY_BUFFER, sky_vboPosId); // always bind before setting data
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    glEnableVertexAttribArray(2); // 2 -> Sending VertexPositions to array #0 in the active shader
    glBindBuffer(GL_ARRAY_BUFFER, sky_vboTexId); // always bind before setting data
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);

    sky_program_id_ = loadShader("../shaders/sky.vs", "../shaders/sky.fs");

    glUseProgram(sky_program_id_);

    loadTextureFromFile("../textures/sky_texture_night.bmp", &texture_night_id, true);
    loadTextureFromFile("../textures/sky_texture.tga", &texture_day_id, true);

    glUseProgram(0);

}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers

    glUseProgram(sky_program_id_);
    glBindVertexArray(sky_vaoId);

    light_.updateLightPos(glfwGetTime() * 0.3);
    cam_.updateCamPos(vec3(dist * sin(glm::radians(azimuth)) * cos(glm::radians(elevation)), dist * sin(glm::radians(elevation)), 
        dist * cos(glm::radians(azimuth)) * cos(glm::radians(elevation))));
    cam_.updateViewMatrix();
    T_.updateTransformations(cam_.getViewMatrix(), cam_.getViewMatrixMirrored(), light_.getLightPos());

    cam_.uploadToGPU(sky_program_id_);
    T_.uploadToGPU(sky_program_id_);
    light_.uploadToGPU(sky_program_id_);

    glEnable(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_day_id);
    glUniform1i(glGetUniformLocation(sky_program_id_, "tex_day"), 0);

    glEnable(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_night_id);
    glUniform1i(glGetUniformLocation(sky_program_id_, "tex_night"), 1);

    glDrawArrays(GL_TRIANGLES,0, 36);

    glUseProgram(program_id_);
    glBindVertexArray(vaoId);

    cam_.updateCamPos(vec3(dist * sin(glm::radians(azimuth)) * cos(glm::radians(elevation)), dist * sin(glm::radians(elevation)), 
        dist * cos(glm::radians(azimuth)) * cos(glm::radians(elevation))));
    cam_.updateViewMatrix();
    T_.updateTransformations(cam_.getViewMatrix(), cam_.getViewMatrixMirrored(), light_.getLightPos());

    cam_.uploadToGPU(program_id_);
    T_.uploadToGPU(program_id_);
    light_.uploadToGPU(program_id_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    glEnable(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_noise_id);
    glUniform1i(glGetUniformLocation(program_id_, "tex_noise"), 0);

    glEnable(GL_TEXTURE1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_sand_id);
    glUniform1i(glGetUniformLocation(program_id_, "tex_sand"), 1);

    glEnable(GL_TEXTURE2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_grass_id);
    glUniform1i(glGetUniformLocation(program_id_, "tex_grass"), 2);
  
    glEnable(GL_TEXTURE3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture_snow_id);
    glUniform1i(glGetUniformLocation(program_id_, "tex_snow"), 3);

    glEnable(GL_TEXTURE4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture_rock_id);
    glUniform1i(glGetUniformLocation(program_id_, "tex_rock"), 4);

    glEnable(GL_CLIP_DISTANCE0);
    glDrawElements(GL_TRIANGLES, grid_.numTriangles() * 3, GL_UNSIGNED_INT, (void*)0);
    glDisable(GL_CLIP_DISTANCE0);
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