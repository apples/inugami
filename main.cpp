#include "inugami/inugami.h"

#include <GL/glfw.h>
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
    renparams.width = 1366;
    renparams.height = 768;
    renparams.fullscreen = false;
    renparams.fov = 90.0;
    renparams.vsync = false;
    renparams.fsaaSamples = 4;
    renparams.mode2D.width = 160.0;
    renparams.mode2D.height = 90.0;
    renparams.mode2D.nearClip = -1.0;
    renparams.mode2D.farClip = 1.0;
    try {core = new Core(renparams);}
    catch (...) {return -1;}

    interface = new Interface();

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

    delete interface;
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
    //gameData.test->bind();
}

void draw()
{
    core->beginFrame();

    core->setMode(Core::RenderMode::PERSPECTIVE, Core::RenderFace::RF_FRONT);

    glTranslatef(0.0f, -1.5f, -3.0f);
    glRotatef(gameState.rot, 0.0f, 1.0f, 0.0f);
    gameData.shieldTex->bind();
    gameData.shield->draw();

    core->setMode(Core::RenderMode::ORTHOGONAL, Core::RenderFace::RF_BOTH);

    glRotatef(gameState.rot, 0.0, 0.0, 1.0);
    gameData.derf->draw();

    core->setMode(Core::RenderMode::INTERFACE, Core::RenderFace::RF_BOTH);

    gameData.font->bind();
    glScalef(8.0, 8.0, 1.0);

    core->endFrame();
}

void tick()
{
    interface->poll();
    gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f);
    if (interface->keyDown('Q'))
        gameState.rot = wrap(gameState.rot-=3.0, 0.0f, 360.0f)
    ;

    if (interface->keyDown('E'))
        gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f)
    ;

    if (interface->keyPressed(' ')) gameData.derf->reset();
}

void idle()
{
    if (interface->keyDown(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
    {
        core->running = false;
    }
}
