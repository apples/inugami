/*******************************************************************************

Copyright (c) 2012 Jeramy Harrison

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.

*******************************************************************************/

#include "customcore.hpp"

#include "meta.hpp"

#include "inugami/camera.hpp"
#include "inugami/interface.hpp"
#include "inugami/loaders.hpp"
#include "inugami/math.hpp"
#include "inugami/transform.hpp"
#include "inugami/utility.hpp"

#include <libnoise/noise.h>

#include <fstream>
#include <utility>

using namespace Inugami;

CustomCore::CustomCore(const RenderParams &params) :
    Core(params),
    rotation(0.f),
    dissolveMin(0.25f), dissolveMax(0.75f),
    ticks(0),
    highDef(true),
    shaderOn(true),

    shieldTex   (new Texture(*this, "data/shield.png", true, false)),
    noiseTex    (generateNoise(shieldTex->getWidth()/2, shieldTex->getHeight()/2, 16.0)),
    font        (new Spritesheet(*this, "data/font.png", 8, 8)),
    fontRoll    (new AnimatedSprite(*this)),
    shield      (new Mesh(*this, "data/shield.obj")),
    shieldHD    (new Mesh(*this, "data/shieldHD.obj")),
    crazy       (nullptr)
{
    ScopedProfile prof(profiler, "CustomCore: Constructor");

    noiseTex->bind(1);

    logger->log<5>("Creating spritesheet...");
    fontRoll->setSpritesheet(font);

    //Compiler bugs prevent this from working
    //auto sprites = comprehend<AnimatedSprite::SpriteList>(1,27,,,[](const int& i){return std::make_pair(4+i/16, i%16);});

    //Generate sprites using list comprehension
    auto sprites = comprehend<AnimatedSprite::SpriteList>(
        1,27,
        std::function<void(int&)>(), std::function<bool(const int&)>(),
        std::function<AnimatedSprite::Sprite(const int&)>(
            [](const int& i)
            {return std::make_pair(4+i/16, i%16);}
        )
    );
    fontRoll->setSprites(sprites);

    //Generate frames
    auto frames = comprehend<AnimatedSprite::FrameList>(
        0,26,
        std::function<void(int&)>(), std::function<bool(const int&)>(),
        std::function<AnimatedSprite::Frame(const int&)>(
            [](const int& i)
            {return std::make_pair(i,5);}
        )
    );
    fontRoll->setSequence(frames);

    //Mode::NORMAL causes the animation to stop when done
    fontRoll->setMode(AnimatedSprite::Mode::NORMAL);

    logger->log<5>("Loading shaders...");
    //Load shader program
    Shader::Program program;
    logger->log<5>("\tVertex...");
    program[Shader::Type::VERT] = loadTextFromFile("shaders/crazy.vert");
    logger->log<5>("\tFragment...");
    program[Shader::Type::FRAG] = loadTextFromFile("shaders/crazy.frag");
    logger->log<5>("\tLinking...");
    crazy = new Shader(program);
    logger->log<5>("\tBinding...");
    setShader(crazy);

    crazy->bind();
    crazy->setUniform("Tex0", 0);
    crazy->setUniform("noiseTex", 1);

    logger->log<5>("Adding callbacks...");
    addCallback(std::bind(&CustomCore::tick, this), 60.0);
    addCallback(std::bind(&CustomCore::draw, this), 60.0);
    addCallback(std::bind(&CustomCore::idle, this), 120.0);

    setWindowTitle("Inugami Demo", true);
}

CustomCore::~CustomCore()
{
    ScopedProfile prof(profiler, "CustomCore: Destructor");

    delete crazy;
    delete shieldHD;
    delete shield;
    delete fontRoll;
    delete font;
    delete noiseTex;
    delete shieldTex;
}

void CustomCore::tick()
{
    ScopedProfile prof(profiler, "CustomCore: Tick");

    //Keybinds can be stored in proxies
    auto keyQ     = iface->getProxy('Q');
    auto keyE     = iface->getProxy('E');
    auto keyO     = iface->getProxy('O');
    auto keyP     = iface->getProxy('P');
    auto keyMinus = iface->getProxy('-');
    auto keyEqual = iface->getProxy('=');
    auto keyLSB   = iface->getProxy('[');
    auto keyRSB   = iface->getProxy(']');
    auto keyW     = iface->getProxy('W');
    auto keySpace = iface->getProxy(' ');
    auto keyESC   = iface->getProxy(0_ivkFunc);
    auto keyF1    = iface->getProxy(1_ivkFunc);
    auto keyF2    = iface->getProxy(2_ivkFunc);
    auto keyF5    = iface->getProxy(5_ivkFunc);
    auto keyF12   = iface->getProxy(12_ivkFunc);

    //Poll must be called every frame
    iface->poll();

    //Key Proxies can be cast to bool
    if (keyESC)
    {
        running = false;
        return;
    }

    //Opening another window is as simple as creating another Core
    if (keyF5)
    {
        CustomCore popup(getParams());
        popup.go();
        do iface->poll();
        while (keyESC);
        return;
    }

    if (!keyW) rotation+=1.0;

    //Inugami has several math functions, including wrap()
    rotation = wrap(rotation, 0.0f, 360.0f);

    if (keyQ) rotation = wrap(rotation-=3.0, 0.0f, 360.0f);
    if (keyE) rotation = wrap(rotation+=1.0, 0.0f, 360.0f);

    if (keyO || keyMinus) dissolveMin = clamp(dissolveMin-=0.005f, -0.25f, 1.25f);
    if (keyP || keyEqual) dissolveMin = clamp(dissolveMin+=0.005f, -0.25f, 1.25f);

    if (keyLSB || keyMinus) dissolveMax = clamp(dissolveMax-=0.005f, -0.25f, 1.25f);
    if (keyRSB || keyEqual) dissolveMax = clamp(dissolveMax+=0.005f, -0.25f, 1.25f);

    //Animations can be reset to their initial state
    if (keySpace) fontRoll->reset();

    //Key Proxies have edge detectors
    if (keyF1.pressed())
    {
        shaderOn = !shaderOn;
        if (shaderOn) setShader(crazy);
        else setShader(nullptr);
    }

    //Key Proxies have edge detectors
    if (keyF2.pressed())
    {
        highDef = !highDef;
    }

    //This is used for the shaders below
    ++ticks;

    //Shaders have easy access to uniforms
    crazy->setUniform("wobbleX", 1.0+0.25*std::sin(ticks/27.0));
    crazy->setUniform("wobbleY", 1.0+0.25*std::cos(ticks/43.0));
    crazy->setUniform("dissolveMin", dissolveMin );
    crazy->setUniform("dissolveMax", dissolveMax );
    crazy->setUniform("hue", ticks/67.0 );

    Vec3 light(0.f,0.f,-1.5f);
    light.x = 4.0*(iface->getMousePos().x/1024.0-0.5)*4.0/3.0;
    light.y = 4.0*(0.5-iface->getMousePos().y/768.0);
    crazy->setUniform( "lightPos", light );

    //A diagnostic can be useful for finding errors
    if (keyF12.pressed()) std::ofstream("core_diagnostic.txt") << getDiagnostic();
}

void CustomCore::draw()
{
    ScopedProfile prof(profiler, "CustomCore: Draw");

    //beginFrame() sets the OpenGL context to the proper initial state
    beginFrame();

    {
        ScopedProfile prof(profiler, "3D");

        //Cameras have view setters based on GLUT
        Camera cam;
        cam.perspective(90.f, 4.f/3.f, 0.1f, 100.f);
        cam.depthTest = true;

        //The camera must be applied to a core
        applyCam(cam);

        //Transforms are matrix stacks
        Transform mat;
        mat.translate(Vec3{0.f, -1.5f, -3.f});
        mat.rotate(rotation, Vec3{0.f, 1.f, 0.f});

        //The core uploads the model matrix (this can also be done manually)
        modelMatrix(mat);

        //Textures are set using bind()
        shieldTex->bind(0);

        //Models use the currently bound texture
        if (highDef) shieldHD->draw();
        else         shield  ->draw();
    }

    {
        ScopedProfile prof(profiler, "2D");
        Camera cam;
        cam.ortho(-8.f, 8.f, -6.f, 6.f, -10.f, 10.f);
        applyCam(cam);

        Transform mat;
        mat.translate(Vec3{-4.f, 3.f, -5.f});
        mat.rotate(rotation, Vec3{0.0, 0.0, 1.0});

        //AnimatedSprites are completely automatic, and bind their own textures
        fontRoll->draw(*this, mat);
    }

    //endFrame() swaps the buffer to the screen
    endFrame();
}

void CustomCore::idle()
{
    if (iface->keyDown(GLFW_KEY_ESC) || getWindowParam(GLFW_SHOULD_CLOSE))
    {
        //Stopping a core at any time will cause the scheduler to return
        running = false;
    }
}

Texture* CustomCore::generateNoise(int width, int height, double freq)
{
    static Texture::Data* data = nullptr;

    if (!data)
    {
        data = new Texture::Data;

        Texture::initData(*data, width, height);

        noise::module::Perlin perlin;
        perlin.SetFrequency(freq);

        constexpr double xRange = 1.0;
        constexpr double yRange = 1.0;
        const double xFactor = xRange/double(width);
        const double yFactor = yRange/double(height);

        for (int i=0; i<height; ++i)
        {
            for (int j=0; j<width; ++j)
            {
                const double x = xFactor*double(i);
                const double y = yFactor*double(j);
                const double z = 0.0;

                for (int octave = 0; octave < 4; ++octave)
                {
                    perlin.SetOctaveCount(octave*2+2);

                    double val = perlin.GetValue(x,y,z);
                    val = val*0.5 + 0.5;
                    val = clamp(val, 0.0, 1.0);

                    Texture::dataPixel(*data, j, i)[octave] = 255*val;
                }
            }
        }
    }

    return new Texture(*this, *data, true, false);
}
