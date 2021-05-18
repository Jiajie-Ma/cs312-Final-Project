#include "main.h"

void init(GLFWwindow* window) {
   glClearColor(0, 0, 0, 1);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_MULTISAMPLE);
   
   cam_ = Camera();
   T_ = Transformations();
   noise_ = Noise();
   light_ = Light();
   terrain_ = Terrain();

   glfwGetFramebufferSize(window, &window_width_, &window_height_);
   noise_.init(tex_size);
   terrain_.init(&transformations_, &light_, render_size, render_size, 256, 256);
   T_.init(cam_.getViewMatrix(), light_.getLightPos(), cam_getPosition(), window_height_, window_width_, glm::radians(45.0f));
   
   fb_tex_noise_ = height_buffer_.init(tex_size, tex_size, GL_R32F, 1);
   
   height_buffer_.bind(); {
       initViewport(tex_size,tex_size);
       noise_.drawHeightMap();
    } height_buffer_.unBind();
}

void display() {
    light_.updateLightPos(glfwGetTime());
    transformations_.updateTransformations(cam_.getViewMatrix(), light_.getLightPos());
    transformations_.updateCamPos(cam_.getPosition());

    /*
    height_buffer_.bind(); {
       initViewport(tex_size,tex_size);
       noise_.drawHeightMap();
    } height_buffer_.unBind();
    */

    glfwGetFramebufferSize(window_, &window_width_, &window_height_);
    terrain_.draw(fb_tex_noise_);
}

int main() {
    initGlfw();
    glfwSetErrorCallback(errorCallback);
    initWindow();

    window_ = openWindow("Terrain Rendering");
    glfwMakeContextCurrent(window_);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

#ifndef APPLE
    if (glewInit() != GLEW_OK)
    {
        return -1;
    }
#endif

    init(window_);

    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    /// Render loop & keyboard input
    while(!glfwWindowShouldClose(window_)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        display();
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    closeWindow(window_);
}