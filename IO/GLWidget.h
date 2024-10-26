#pragma once
#include "Common/common.h"
#include "UI/main_window.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
public:
  GLWidget(QWidget *parent = nullptr) : QOpenGLWidget(parent) {
    cfg = qobject_cast<MainWindow*>(parent->parent())->get_config();
    textureBuffer = new uint8_t[cfg->displaySize]{};
  }

  ~GLWidget() {
    delete textureBuffer;
  }

  void update_buffer(const uint8_t *buffer) {
    for(uint32_t p = 0; p < cfg->displaySize; p++) {
      uint8_t bytePos = p / 8, bitPos = 7 - p % 8;
      textureBuffer[p] = buffer && (buffer[bytePos] >> bitPos & 1) ? 0xFF : 0x00;
    }

    update();
  }

private:
  void initializeGL() override {
    initializeOpenGLFunctions();
    glViewport(0, 0, cfg->displayWidth * cfg->scale, cfg->displayHeight * cfg->scale);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);  
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);  

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, cfg->displayWidth, cfg->displayHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  void resizeGL(int w, int h) override {
    glViewport(0, 0, w, h);
  }

  void paintGL() override {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cfg->displayWidth, cfg->displayHeight, GL_RED, GL_UNSIGNED_BYTE, textureBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  }

  Config *cfg;
  uint vao, vbo, ebo, vertexShader, fragmentShader, shaderProgram, texture;
  uint8_t *textureBuffer;

  static constexpr float vertices[] = {
    -1.0f, 1.0f, 0.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f,
    1.0f,  -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
  };  

  static constexpr uint indices[] = {
    0, 1, 3,
    1, 2, 3
  };  
  const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "  TexCoord = aTexCoord;\n"
    "}\0";
  const char *fragmentShaderSource = "#version 330 core\n"
    "in vec2 TexCoord;"
    "out vec4 FragColor;"
    "uniform sampler2D texture1;"
    "void main()\n"
    "{\n"
    "  FragColor = texture(texture1, TexCoord);\n"
    "}\0";
};