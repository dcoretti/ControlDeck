#pragma once
#include "common.h"

void checkShaderCompileStatus(unsigned int shaderId) {
    int status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        char *strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, strInfoLog);
        DBG_ASSERT(false, "Compile failure for shader %d : %s\n", shaderId, strInfoLog);
        delete[] strInfoLog;
    }
}

void checkShaderLinkStatus(unsigned int shaderProgram) {
    int status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, strInfoLog);
        DBG_ASSERT(false, "Shader linker failure %s\n", strInfoLog);
        delete[] strInfoLog;
    }
}

char * loadTextFile(const char * fname) {
    FILE *f = fopen(fname, "r");
    DBG_ASSERT(f != nullptr, "File %s couldn't be opened\n", fname);
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    rewind(f);
    char *buf = new char[size + 1];
    size_t res = fread(buf, sizeof(char), size, f);
    DBG_ASSERT(res != 0, " Expected non-zero result for fread\n");
    buf[res] = '\0';

    fclose(f);

    return buf;
}

void createShader(unsigned int *shaderProgramId, const char * vertexFileName, const char * fragmentFileName) {
    char * vertexSource = loadTextFile(vertexFileName);
    char * fragmentSource = loadTextFile(fragmentFileName);

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, 0);   // null for len assumes \0 terminated.
    glCompileShader(vertexShader);
    checkShaderCompileStatus(vertexShader);

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, 0);   // null for len assumes \0 terminated.
    glCompileShader(fragmentShader);
    checkShaderCompileStatus(fragmentShader);

    *shaderProgramId = glCreateProgram();
    glAttachShader(*shaderProgramId, vertexShader);
    glAttachShader(*shaderProgramId, fragmentShader);
    glLinkProgram(*shaderProgramId);
    checkShaderLinkStatus(*shaderProgramId);

    glDetachShader(*shaderProgramId, vertexShader);
    glDetachShader(*shaderProgramId, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    delete[] vertexSource;
    delete[] fragmentSource;
    printf("Shader compiled successfully.\n");
}