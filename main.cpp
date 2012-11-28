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

    init();
    scheduler->go();

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

    try{
        gameData.opopo = new Spritesheet("data/font.png", 8, 8);
        gameData.derf = new AnimatedSprite();
        gameData.derf->setSpritesheet(gameData.opopo);
        gameData.derf->setSprites({{4,1}, {4,2}, {4,3}, {4,4}});
        gameData.derf->setSequence({{0,60}, {1,60}, {2,60}, {3,60}});
        gameData.derf->setMode(AnimatedSprite::Mode::NORMAL);
    }
    catch (...)
    {

    }
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

    if (gameData.derf->done()) gameData.derf->reset();

    if (interface->keyPressed(GLFW_KEY_F1)) renderer->dumpState(std::ofstream("dump.txt"));
}

void idle()
{
    if (interface->keyState(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
    {
        scheduler->running = false;
    }
}
