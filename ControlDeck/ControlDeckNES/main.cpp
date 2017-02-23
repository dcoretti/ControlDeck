#ifdef _MSC_VER
// Disable warnings for fopen
#pragma warning(disable:4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//#include <stdio.h>
#define GLEW_DLL
#include "GL/glew.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3.h"
#include "nes.h"
#include "shaderLoader.h"

bool pause = true;
NES::NesControlDeck controlDeck;

const unsigned int width = 256;
const unsigned int height = 240;
GLuint textureBufferId;
GLuint va;
GLuint vbo;
unsigned int shaderProgramId;
// matching bindings to shaders
static const int vertexShaderBinding = 0;
static const int uvShaderBinding = 1;

uint8_t test[3 * sizeof(uint8_t) * width * height]{ 1};
static const GLfloat tri[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

static const GLfloat square[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

static const GLfloat uv[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};
void setupRenderSurface() {
    memset(test, 200, 3*sizeof(uint8_t) * width * height);
    createShader(&shaderProgramId, "shader.vert", "shader.frag");


    glGenVertexArrays(1, &va);
    glBindVertexArray(va);
    GLuint vb;
    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
    // Bind this array buffer to the shader layout and define the data format.
    glVertexAttribPointer(vertexShaderBinding,  
        3,
        GL_FLOAT,
        GL_FALSE, 
        0, (void*)0);
    glEnableVertexAttribArray(vertexShaderBinding);

    GLuint uvb;
    glGenBuffers(1, &uvb);
    glBindBuffer(GL_ARRAY_BUFFER, uvb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
    // Bind this array buffer to the shader layout and define the data format.
    glVertexAttribPointer(uvShaderBinding,
        2,
        GL_FLOAT,
        GL_FALSE, // not normalized
        0, (void*)0);
    glEnableVertexAttribArray(uvShaderBinding);


    glGenTextures(1, &textureBufferId);
    glBindTexture(GL_TEXTURE_2D, textureBufferId);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, test); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (GLFW_PRESS == action) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_SPACE:
            pause = !pause;
        }
    }
}

void setupConsole() {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setupConsole();

    GLFWwindow* window;
    //glewExperimental = GL_TRUE;
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    // Deprecated functionality will be removed from being marked in a previous version. 
    // in 3.1 it will remove anything marked as deprecated in 3.0 but not removed in 3.1
    // This is ONLY useful between 3.0 and 3.1 for 3.2+ this does nothing as it was a deprecation path
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // instead of setting profile, giving hints and letting system decide
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(1);	//vsync

    window = glfwCreateWindow(width *2, height * 2, "RenderMode", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, keyCallback);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("Error initializing glew: %s\n", glewGetErrorString(err));
        return -1;
    }

    char *fname = "C:\\Users\\dc\\Desktop\\test.nes"; // "C:\\Users\\dc\\Desktop\\nestest\\cpu_reset\\registers.nes";
    initNes(fname, controlDeck);

    unsigned int iterations = 0;
    setupRenderSurface();
    while (!glfwWindowShouldClose(window)) {
         glfwPollEvents();
        // temporary hack to play around with
        if (!pause) {
            nesLoop(controlDeck, 500);
        }


        // Render the results
        glClear(GL_COLOR_BUFFER_BIT);
        // update the texture with the latest
        glUseProgram(shaderProgramId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBufferId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, controlDeck.renderBuffer.renderBuffer);
        glBindVertexArray(va);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}