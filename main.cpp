#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include "glad/glad.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"

SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
SDL_Window* m_window = nullptr;
SDL_GLContext m_glContext = nullptr;
bool running = false;

// Shader sources
const GLchar* vertexSource = R"glsl(
    #version 130 core
    in vec2 position;
    in vec3 color;
    out vec3 Color;
    void main()
    {
        Color = color;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
const GLchar* fragmentSource = R"glsl(
    #version 130 core
    in vec3 Color;
    out vec4 outColor;
    void main()
    {
        outColor = vec4(Color, 1.0);
    }
)glsl";

//Buffers
GLuint vao, vboPos, vboCol, ebo;
GLuint shaderProgram;

std::vector<GLfloat> verticesColor = {
    0.f, 0.f, 1.f,
    0.f, 1.f, 0.f,
    1.f, 0.f, 0.f,
    1.f, 1.f, 0.f
}; //Colors for each Vertex

std::vector<GLfloat> vertices = {
    -0.5f, 0.5f,
    0.5f, 0.5f,
    0.5f, -0.5f,
    -0.5f, -0.5f
}; //Positions for each Vertex

GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
}; //Vertex Indices

void openGLINFO()
{
    std::cout << "OpenGL Vendor: " <<  glad_glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL Renderer: " <<  glad_glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " <<  glad_glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Shading Language: " <<  glad_glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
} //Displays Some OpenGL info

void initOpenGL()
{
    gladLoadGL();

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_MakeCurrent(m_window, m_glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    //Shaders
    GLuint vertexShader = glad_glCreateShader(GL_VERTEX_SHADER);
    glad_glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glad_glCompileShader(vertexShader);

    GLuint fragmentShader = glad_glCreateShader(GL_FRAGMENT_SHADER);
    glad_glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glad_glCompileShader(fragmentShader);

    shaderProgram = glad_glCreateProgram();
    glad_glAttachShader(shaderProgram, vertexShader);
    glad_glAttachShader(shaderProgram, fragmentShader);
    glad_glBindFragDataLocation(shaderProgram, 0, "outColor");
    glad_glLinkProgram(shaderProgram);
    glad_glUseProgram(shaderProgram);

    //Buffer
    glad_glGenVertexArrays(1, &vao);
    glad_glBindVertexArray(vao);

    glad_glGenBuffers(1, &ebo);
    glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    //Position Vertex Buffer
    glad_glGenBuffers(1, &vboPos);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glad_glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW); //Assign data to the Position Vertex Buffer

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glad_glEnableVertexAttribArray(posAttrib);
    glad_glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //Color Vertex Buffer
    glad_glGenBuffers(1, &vboCol);
    glad_glBindBuffer(GL_ARRAY_BUFFER, vboCol);
    glad_glBufferData(GL_ARRAY_BUFFER, verticesColor.size() * sizeof(GLfloat), verticesColor.data(), GL_STATIC_DRAW); //Assign data to the Color Vertex Buffer

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glad_glEnableVertexAttribArray(colAttrib);
    glad_glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
};

void init()
{
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow("TestSDLGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, winFlags);
    if(!m_window)
    {
        std::cout << SDL_GetError() << std::endl;
        exit(-1);
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if(!m_glContext)
    {
        std::cout << "COULD NOT CREATE CONTEXT" << std::endl;
        exit(-1);
    }

    initOpenGL();
    openGLINFO();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui_ImplSDL2_InitForOpenGL(m_window, (void*)m_glContext);
};

void render()
{
    ImVec4 color[] = {
        {verticesColor[0], verticesColor[1], verticesColor[2], 0.f},
        {verticesColor[3], verticesColor[4], verticesColor[5], 0.f},
        {verticesColor[6], verticesColor[7], verticesColor[8], 0.f},
        {verticesColor[9], verticesColor[10], verticesColor[11], 0.f}
    }; //Placeholder array to change vertex colors

    bool open = true; //Just a placeholder

    //Create New Frame for ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    //Debug Window
    {
        ImGui::Begin("Debug", &open, (ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove));
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if(ImGui::ColorEdit3("Vertex 1", (float*)&color[0]))
        {
            verticesColor[0] = color[0].x;
            verticesColor[1] = color[0].y;
            verticesColor[2] = color[0].z;
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, (verticesColor.size()/3)*sizeof(GLfloat), verticesColor.data());
        }
        if(ImGui::ColorEdit3("Vertex 2", (float*)&color[1]))
        {
            verticesColor[3] = color[1].x;
            verticesColor[4] = color[1].y;
            verticesColor[5] = color[1].z;
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, (verticesColor.size()/3*2)*sizeof(GLfloat), verticesColor.data());
        }
        if(ImGui::ColorEdit3("Vertex 3", (float*)&color[2]))
        {
            verticesColor[6] = color[2].x;
            verticesColor[7] = color[2].y;
            verticesColor[8] = color[2].z;
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, (verticesColor.size()/3*3)*sizeof(GLfloat), verticesColor.data());
        }
        if(ImGui::ColorEdit3("Vertex 4", (float*)&color[3]))
        {
            verticesColor[9] = color[3].x;
            verticesColor[10] = color[3].y;
            verticesColor[11] = color[3].z;
            glad_glBufferSubData(GL_ARRAY_BUFFER, 0, (verticesColor.size())*sizeof(GLfloat), verticesColor.data());
        }
        ImGui::End();
    }

    //ImGui::ShowDemoWindow();

    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

    ImGui::Render();
    glClearColor(0, 0, 0, 255); // Background Color
    glClear(GL_COLOR_BUFFER_BIT);

    glad_glDrawElements(GL_POLYGON, 6, GL_UNSIGNED_INT, 0); //Render the Shape

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
};

void events()
{
    SDL_Event evnt;
    SDL_PollEvent(&evnt);
    switch (evnt.type)
    {
        case SDL_QUIT: running = false; break;
        default: break;
    }
    ImGui_ImplSDL2_ProcessEvent(&evnt);
};

void clean()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glad_glDeleteBuffers(1, &vboPos);
    glad_glDeleteBuffers(1, &vboCol);
    glad_glDeleteVertexArrays(1, &vao);
    glad_glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    init();
    running = true;
    while(running)
    {
        events();
        render();
    }
    clean();
    return 0;
} // main loop
