#include "inugami/scheduler.h"
#include "inugami/interface.h"
#include "inugami/renderer.h"
#include "inugami/math.h"

#include <GL/glfw.h>
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
    Texture font, shieldTex;
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
    renparams.mode2D.width = 16.0;
    renparams.mode2D.height = 9.0;
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
    Renderer::TexParams tp;
    tp.clamp = false;
    tp.smooth = true;
    gameData.font = renderer->loadTexture("data/font.png", tp);
    gameData.shieldTex = renderer->loadTexture("data/walls.png", tp);

    gameState.rot = 0.0f;

    renderer->setWindowTitle("Inugami Test", true);
}

void draw()
{
    renderer->beginFrame();

    renderer->setMode(Renderer::RenderMode::RM_3D, Renderer::RenderFace::RF_BOTH);

    renderer->setTexture(gameData.shieldTex);

    glTranslatef(0.0, -1.5, -3.0);
    glRotatef(gameState.rot, 0.0, 1.0, 0.0);
    gameData.shield->drawImmediate();

    renderer->setMode(Renderer::RenderMode::RM_INTERFACE, Renderer::RenderFace::RF_BOTH);

    renderer->setTexture(gameData.font);
    glScalef(8.0, 8.0, 1.0);
    renderer->printer << Renderer::PrintItem::COLOR << 1.0f << 1.0f << 1.0f;
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
}

void idle()
{
    if (interface->keyState(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
    {
        scheduler->running = false;
    }
}
