/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "inugami/inugami.h"

#include <fstream>
#include <sstream>
#include <iomanip>

using namespace Inugami;

void init();
void draw();
void tick();
void idle();

Interface *interface;
Core *core;

//Most global variables should be added to this struct
struct
{
    float rot;
    Camera cam;
} gameState;

struct
{
    Texture *font, *shieldTex;
    Spritesheet *opopo;
    AnimatedSprite *derf;
    Mesh *shield;
    Shader *test;
} gameData;

int main()
{
    Core::RenderParams renparams;
    renparams.width = 1024;
    renparams.height = 768;
    renparams.fullscreen = false;
    renparams.vsync = false;
    renparams.fsaaSamples = 4;
    try {core = new Core(renparams);}
    catch (...) {return -1;}

    interface = core->getInterface();

    core->addCallback(draw, 60.0);
    core->addCallback(tick, 60.0);
    core->addCallback(idle, -1.0);

    try
    {
        init();
        core->go();
    }
    catch (std::exception &e)
    {
        std::ofstream("error.txt") << e.what();
    }

    delete core;

    return 0;
}

void init()
{
    gameData.font = new Texture("data/font.png", false, false);
    gameData.shieldTex = new Texture("data/shield.png", true, false);
    gameData.shield = core->loadMesh("data/shield.obj");

    gameData.opopo = new Spritesheet("data/font.png", 8, 8);

    gameData.derf = new AnimatedSprite();
    gameData.derf->setSpritesheet(gameData.opopo);
    AnimatedSprite::SpriteList sprites;
    for (int i=1; i<=26; ++i) sprites.push_back({4+i/16, i%16});
    gameData.derf->setSprites(sprites);
    AnimatedSprite::FrameList frames;
    for (int i=0; i<26; ++i) frames.push_back({i, 5});
    gameData.derf->setSequence(frames);
    gameData.derf->setMode(AnimatedSprite::Mode::NORMAL);

    gameState.rot = 0.0f;

    core->setWindowTitle("Inugami Test", true);

    Shader::Program pro;
    pro[Shader::Type::VERT] =
        "#version 400\n"
        "layout (location = 0) in vec3 VertexPosition;\n"
        "layout (location = 1) in vec3 VertexNormal;\n"
        "layout (location = 2) in vec2 VertexTexCoord;\n"
        "out vec3 Position;\n"
        "out vec3 Normal;\n"
        "out vec2 TexCoord;\n"
        "void main()\n"
        "{\n"
        "    TexCoord = VertexTexCoord;\n"
        "    Normal = normalize(VertexNormal);\n"
        "    Position = VertexPosition/5;\n"
        "    gl_Position = vec4(VertexPosition/5,1.0);\n"
        "}\n"
    ;
    pro[Shader::Type::FRAG] =
        "#version 400\n"
        "in vec3 Position;\n"
        "in vec3 Normal;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D Tex1;\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    vec4 texColor = texture( Tex1, TexCoord );\n"
        "    FragColor = texColor;\n"
        "}\n"
    ;
    gameData.test = new Shader(pro);
    gameData.test->bind();

    gameState.cam.depthTest = true;
}

void draw()
{
    core->beginFrame();

    {
        gameState.cam.perspective(90.f, 4.f/3.f, 0.1f, 100.f);
        core->applyCam(gameState.cam);

        auto mat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.5f, -3.f));
        mat = glm::rotate(mat, gameState.rot, glm::vec3(0.f, 1.f, 0.f));

        core->modelMatrix(mat);

        gameData.shieldTex->bind();
        gameData.shield->draw();
    }

    {
        gameState.cam.ortho(-8.f, 8.f, -6.f, 6.f, -1.f, 1.f);
        core->applyCam(gameState.cam);

        auto mat = glm::rotate(glm::mat4(1.f), gameState.rot, glm::vec3(0.0, 0.0, 1.0));

        core->modelMatrix(mat);

        gameData.derf->draw();
    }

    core->endFrame();
}

void tick()
{
    interface->poll();

    gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f);

    static auto keyQ     = interface->getProxy('Q');
    static auto keyE     = interface->getProxy('E');
    static auto keySpace = interface->getProxy(' ');

    if (keyQ) gameState.rot = wrap(gameState.rot-=3.0, 0.0f, 360.0f);
    if (keyE) gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f);

    if (keySpace) gameData.derf->reset();
}

void idle()
{
    if (interface->keyDown(GLFW_KEY_ESC) || core->getWindowParam(GLFW_CLOSE_REQUESTED))
    {
        core->running = false;
    }
}
