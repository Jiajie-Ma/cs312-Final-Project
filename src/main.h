// Haverford College, Jiajie Ma, 2021

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

#include "util/AGL.h"
#include "util/AGLM.h"
#include "util/camera.h"
#include "util/transformations.h"
#include "util/light.h"
#include "util/grid.h"
#include "util/image.h"
#include "util/noise.h"

#include "stb/stb_image_write.h"
#include "stb/stb_image.h"

using namespace std;
using namespace glm;
using namespace agl;

// globals
int window_width_ = 1920;
int window_height_ = 1080;
int tex_size = 1024;
float render_size = 25.0f;

Camera cam_;
Light light_;
Transformations T_;
Grid grid_;

GLFWwindow* window;
GLuint program_id_, vboPosId, elementbuffer, vaoId;
GLuint sky_program_id_, sky_vboPosId, sky_vboTexId, sky_vaoId;
GLuint texture_sand_id, texture_grass_id, texture_snow_id, texture_rock_id, texture_noise_id, texture_day_id, texture_night_id;

Image noise_tex(tex_size, tex_size);

// control
const float cameraSpeed = 0.25f;
float lastX = 250.0f, lastY = 250.0f, dist = 1.0f, azimuth = 0.0f, elevation = 0.0f;
bool control = false, zoom = false;

//noise
int octaves = 5, mode = 1;
float H = 0.15f, f = 2.0f, max_height = 15.0f, density = 7.5f, offset= 0.9;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
   {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
   }
}

static void initViewport(int width, int height){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0 , 0, width, height);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   // Prevent a divide by zero
   if(height == 0) height = 1;
	
   // Set Viewport to window dimensions
   initViewport(width, height);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
   double xpos, ypos;
   glfwGetCursorPos(window, &xpos, &ypos);

   int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (state == GLFW_PRESS)
   {
      control = true;
      int keyPress = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
      if (keyPress == GLFW_PRESS) {
         zoom = true;
      }

   }
   else if (state == GLFW_RELEASE)
   {
      control = false;
      zoom = false;
   }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
   if (control)
   {
      // zoom in/out with shift and cursor
      if (zoom){
         float distDelta = ypos - lastY;
         
         distDelta *= cameraSpeed;

         dist += distDelta;

         if (dist < 1.0f){
            dist = 1.0f;
         }

         if (dist > 50.0f){
            dist = 50.0f;
         }

         if (elevation < 0.0f && dist > 1.0f){
            dist = 1.0f;
         }

         if (elevation > 0.0f && elevation < 20.0f  && dist >8.0f){
            dist = 8.0f;
         }
      }
      else{
         // rotate with cursor
         float xDelta = lastX - xpos;
         float yDelta = ypos - lastY;

         xDelta *= cameraSpeed;
         yDelta *= cameraSpeed;

         azimuth += xDelta;
         elevation += yDelta;

         if(elevation > 89.9f){
            elevation = 89.9f;
         }   
         else if(elevation < -89.9f){
            elevation = -89.9f;
         } 

         if(dist > 8.0f && elevation < 0.0f){
            elevation = 0.0f;
         }
      }
   }

   lastX = xpos;
   lastY = ypos;
}

static void initWindow(){
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static GLFWwindow* openWindow(string title){
    const char* c = title.c_str();
    window = glfwCreateWindow(window_width_, window_height_, c, NULL, NULL);
    if( !window ){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    return window;
}

static void initGlfw(){
    /// GLEW Initialization
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }
}

static void closeWindow(GLFWwindow* window){
    /// Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}


static void PrintShaderErrors(GLuint id, const std::string label)
{
   std::cerr << label << " failed\n";
   GLint logLen;
   glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);
   if (logLen > 0)
   {
      char* log = (char*)malloc(logLen);
      GLsizei written;
      glGetShaderInfoLog(id, logLen, &written, log);
      std::cerr << "Shader log: " << log << std::endl;
      free(log);
   }
}

inline std::string loadShaderFromFile(const std::string& fileName)
{
   std::ifstream file(fileName);
   if (!file)
   {
      std::cout << "Cannot load file: " << fileName << std::endl;
      return "";
   }

   std::stringstream code;
   code << file.rdbuf();
   file.close();

   return code.str();
}

inline GLuint loadShader(const std::string& vertex, const std::string& fragment)
{
   GLint result;
   std::string vertexShader = loadShaderFromFile(vertex);
   const char* vertexShaderRaw = vertexShader.c_str();
   GLuint vshaderId = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vshaderId, 1, &vertexShaderRaw, NULL);
   glCompileShader(vshaderId);
   glGetShaderiv(vshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(vshaderId, "Vertex shader");
      return -1;
   }

   std::string fragmentShader = loadShaderFromFile(fragment);
   const char* fragmentShaderRaw = fragmentShader.c_str();
   GLuint fshaderId = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fshaderId, 1, &fragmentShaderRaw, NULL);
   glCompileShader(fshaderId);
   glGetShaderiv(fshaderId, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(fshaderId, "Fragment shader");
      return -1;
   }

   GLuint shaderId = glCreateProgram();
   glAttachShader(shaderId, vshaderId);
   glAttachShader(shaderId, fshaderId);
   glLinkProgram(shaderId);
   glGetShaderiv(shaderId, GL_LINK_STATUS, &result);
   if (result == GL_FALSE)
   {
      PrintShaderErrors(shaderId, "Shader link");
      return -1;
   }
   return shaderId;
}

inline GLuint loadTexture(const std::string& filename)
{
   Image image;
   image.load(filename);
   

   GLuint texId;
   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image.width(), image.height());
   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(),
      GL_RGBA, GL_UNSIGNED_BYTE, image.data());

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   // cleanup
   glBindTexture(GL_TEXTURE_2D, 0);

   return texId;
}

static void loadTextureFromFile(string filename, GLuint* id, bool mipmap){
    // load texture
    int width;
    int height;
    int nb_component;
    // set stb_image to have the same coordinates as OpenGL
    stbi_set_flip_vertically_on_load(1);
    unsigned char* image = stbi_load(filename.c_str(), &width,
                                     &height, &nb_component, 0);
    if(image == nullptr) {
        throw(string("Failed to load texture"));
    }
    glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(mipmap > 0) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0.0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 3.0);
    } else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if(nb_component == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image);
    } else if(nb_component == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    if(mipmap>0)
        glGenerateMipmap(GL_TEXTURE_2D);
    // cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);
}
