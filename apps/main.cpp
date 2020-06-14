#ifdef _MSC_VER
// Disable warnings for fopen
#pragma warning(disable:4996)
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL/gl3w.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3.h>
#include <ControlDeck/nes.h>
#include "shaderLoader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


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
    createShader(&shaderProgramId, "../shaders/shader.vert", "../shaders/shader.frag");


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

void errorCallback(int error, const char * description) {
	fprintf(stderr, "ERROR(%d): %s\n", error, description);
}

void setupConsole() {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
}


void setupPPUUI() {
	bool * open = nullptr;
	// if (!ImGui::Begin("ImGui Demo", open, 0))
	// {
	// 	// Early out if the window is collapsed, as an optimization.
	// 	ImGui::End();
	// 	return;
	// }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setupConsole();

    GLFWwindow* window;
    //glewExperimental = GL_TRUE;
    if (!glfwInit()) {
        return -1;
    }

    const char* glsl_version = "#version 130";
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


    window = glfwCreateWindow(width *2, height * 2, "ControlDeck", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
	// if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	// 	fprintf(stderr, "unable to initialize glad opengl context.\n");
	// 	return -1;
	// }
    if (gl3wInit()) {
        fprintf(stderr, "failed to initialize OpenGL via gl3w\n");
        return -1;
    }
    if (!gl3wIsSupported(3, 2)) {
            fprintf(stderr, "OpenGL 3.2 not supported\n");
            return -1;
    }

	glfwSwapInterval(1);	//vsync
    glEnable(GL_CULL_FACE);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetErrorCallback(errorCallback);
    //GLenum err = glewInit();
    //if (GLEW_OK != err) {
    //    printf("Error initializing glew: %s\n", glewGetErrorString(err));
    //    return -1;
    //}

    char *fname = "C:\\nes\\smb.nes";
    initNes(fname, controlDeck);

    unsigned int iterations = 0;
    setupRenderSurface();

	GLFWwindow* debugWindow = glfwCreateWindow(512, 256, "Debug", NULL, NULL);
    int windowX,windowY;
    glfwGetWindowPos(window, &windowX, &windowY);
    glfwSetWindowPos(debugWindow, windowX + (width * 2) + 100, windowY);

	glfwSetKeyCallback(debugWindow, keyCallback);
	glfwMakeContextCurrent(debugWindow);


    // imgui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(debugWindow, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool showDemoWindow = true;

    while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(debugWindow)) {
         glfwPollEvents();
        // temporary hack to play around with
        if (!pause) {
            nesLoop(controlDeck, 200);
        }

		glfwMakeContextCurrent(window);
        // Render the results
        glClear(GL_COLOR_BUFFER_BIT);
        // update the texture with the latest
        glUseProgram(shaderProgramId);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBufferId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, controlDeck.ppu.renderBuffer.renderBuffer);
        glBindVertexArray(va);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glfwSwapBuffers(window);




		// Render the pattern tables
		glfwMakeContextCurrent(debugWindow);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(debugWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(debugWindow);
    }



    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwDestroyWindow(debugWindow);
    glfwTerminate();

    return 0;
}