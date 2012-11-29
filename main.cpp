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

Scheduler *scheduler;
Interface *interface;
Renderer *renderer;

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
} gameData;

int main()
{
    scheduler = new Scheduler();

    Renderer::RenderParams renparams;
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
    try {renderer = new Renderer(renparams);}
    catch (...) {return -1;}

    interface = new Interface();

    scheduler->addCallback(draw, 60.0);
    scheduler->addCallback(tick, 60.0);
    scheduler->addCallback(idle, -1.0);

    try
    {
        init();
        scheduler->go();
    }
    catch (std::exception &e)
    {

    }

    delete interface;
    delete renderer;
    delete scheduler;

    return 0;
}

void init()
{
    gameData.font = new Texture("data/font.png", false, false);
    gameData.shieldTex = new Texture("data/shield.png", true, false);
    gameData.shield = renderer->loadMesh("data/shield.obj");

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

    renderer->setWindowTitle("Inugami Test", true);
}

void draw()
{
    renderer->beginFrame();

    renderer->setMode(Renderer::RenderMode::RM_3D, Renderer::RenderFace::RF_FRONT);

    glTranslatef(0.0f, -1.5f, -3.0f);
    glRotatef(gameState.rot, 0.0f, 1.0f, 0.0f);
    gameData.shieldTex->bind();
    gameData.shield->draw();

    renderer->setMode(Renderer::RenderMode::RM_2D, Renderer::RenderFace::RF_BOTH);

    glRotatef(gameState.rot, 0.0, 0.0, 1.0);
    gameData.derf->draw();

    renderer->setMode(Renderer::RenderMode::RM_INTERFACE, Renderer::RenderFace::RF_BOTH);

    gameData.font->bind();
    glScalef(8.0, 8.0, 1.0);
    renderer->printer << "Rot: " << gameState.rot << '\n';
    renderer->printer.print();

    renderer->endFrame();
}

void tick()
{
    interface->poll();
    gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f);
    if (interface->keyState('Q'))
        gameState.rot = wrap(gameState.rot-=3.0, 0.0f, 360.0f)
    ;

    if (interface->keyState('E'))
        gameState.rot = wrap(gameState.rot+=1.0, 0.0f, 360.0f)
    ;

    if (interface->keyPressed(' ')) gameData.derf->reset();
}

void idle()
{
    if (interface->keyState(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
    {
        scheduler->running = false;
    }
}
