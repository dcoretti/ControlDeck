#ifdef _MSC_VER
// Disable warnings for fopen
#pragma warning(disable:4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//#include <stdio.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
//#include "GL/glew.h"
//#include "GLFW/glfw3.h"

#include "nes.h"


void setupConsole() {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
}

//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (GLFW_PRESS == action) {
//        switch (key) {
//        case GLFW_KEY_ESCAPE:
//            glfwSetWindowShouldClose(window, GL_TRUE);
//            break;
//        }
//    }
//}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setupConsole();

    //GLFWwindow* window;
    //glewExperimental = GL_TRUE;
    //if (!glfwInit()) {
    //    return -1;
    //}

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    //glfwWindowHint(GLFW_DEPTH_BITS, 16);
    //// Deprecated functionality will be removed from being marked in a previous version. 
    //// in 3.1 it will remove anything marked as deprecated in 3.0 but not removed in 3.1
    //// This is ONLY useful between 3.0 and 3.1 for 3.2+ this does nothing as it was a deprecation path
    ////glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //// instead of setting profile, giving hints and letting system decide
    ////glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwSwapInterval(1);	//vsync

    //window = glfwCreateWindow(520, 240, "RenderMode", NULL, NULL);
    //if (!window) {
    //    glfwTerminate();
    //    return -1;
    //}

    //glfwMakeContextCurrent(window);
    //glEnable(GL_CULL_FACE);
    //glfwSetKeyCallback(window, keyCallback);
    //GLenum err = glewInit();
    //if (GLEW_OK != err) {
    //    printf("Error initializing glew: %s\n", glewGetErrorString(err));
    //    return -1;
    //}

    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //while (!glfwWindowShouldClose(window)) {
    //    glfwPollEvents();

    //    glClear(GL_COLOR_BUFFER_BIT);
    //    glfwSwapBuffers(window);

    //}





    NES::NesControlDeck controlDeck;
    initNes("C:\\Users\\dc\\Desktop\\nestest\\cpu_reset\\registers.nes", controlDeck);

    nesLoop(controlDeck, 6200);

    int tmp;
    //scanf("%d", &tmp);

    //glfwTerminate();
    return 0;
}