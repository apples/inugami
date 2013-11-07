/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.3.0
 * https://github.com/DBRalir/Inugami
 *
 * Copyright (c) 2012 Jeramy Harrison <dbralir@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "customcore.hpp"

#include "meta.hpp"

#include "inugami/camera.hpp"
#include "inugami/geometry.hpp"
#include "inugami/image.hpp"
#include "inugami/interface.hpp"
#include "inugami/loaders.hpp"
#include "inugami/math.hpp"
#include "inugami/shader.hpp"
#include "inugami/shaderprogram.hpp"
#include "inugami/sixense.hpp"
#include "inugami/transform.hpp"
#include "inugami/utility.hpp"

#include <fstream>
#include <utility>

using namespace Inugami;

CustomCore::CustomCore(const RenderParams &params)
    : Core(params)
    , rotation(0.f)
    , dissolveMin(0.25f)
    , dissolveMax(0.75f)
    , gemScale(1.f)
    , ticks(0)
    , highDef(true)
    , shaderOn(true)

    , noise    (64, 64)
    , noiseDir (64, 64, {1,1,1,1})

    , shieldTex       (Image::fromPNG("data/shield.png"), true, false)
    , noiseTex        ()
    , glassTex        (Image(32,32,{32,32,255,128}), false, false)
    , fontRoll        (Spritesheet(Image::fromPNG("data/font.png"), 8, 8))
    , shield          (reCenter(Geometry::fromOBJ("data/shield.obj")))
    , shieldHD        (reCenter(Geometry::fromOBJ("data/shieldHD.obj")))
    , defaultShader   (getShader())
    , crazyShader     (ShaderProgram::fromName("shaders/crazy"))

    , sixense         (Sixense::inst())
{
    ScopedProfile prof(profiler, "CustomCore: Constructor");

    logger->log("Initting spritesheet...");

    //Generate sprites using list comprehension
    fontRoll.setSprites(container_cast<AnimatedSprite::SpriteList>(
        range(1, 27, [](int i){return std::make_pair(4+i/16, i%16);})
    ));

    //Generate frames
    fontRoll.setSequence(container_cast<AnimatedSprite::FrameList>(
        range(0, 26, [](int i){return std::make_pair(i,10);})
    ));

    //Mode::NORMAL causes the animation to stop when done
    fontRoll.setMode(AnimatedSprite::Mode::LOOP);

    logger->log("Adding callbacks...");
    addCallback([&]{ tick(); draw(); }, 60.0);

    setWindowTitle("Inugami Demo (Sixense)", true);

    crazyShader.bind();
    crazyShader.uniform("Tex0"    ).set(0);
    crazyShader.uniform("noiseTex").set(1);

    Image tmp = Image::fromNoise(64, 64);
    //tmp = blur(tmp);
    //tmp = blur(tmp);

    for (int r=0; r<noise.height; ++r)
    {
        for (int c=0; c<noise.width; ++c)
        {
            double f = float(c)/(noise.width/2.0);
            if (f>1.0) f = 2.0-f;
            SubPixel sp = 255*f;
            noise[r][c] = {sp, sp, sp, sp};
            for (int i=0; i<4; ++i)
            {
                noise[r][c][i] = clamp(noise[r][c][i]+(128-tmp[r][c][i])/8, 0, 255);
            }
        }
    }
}

CustomCore::~CustomCore()
{}

void CustomCore::tick()
{
    ScopedProfile prof(profiler, "CustomCore: Tick");

    //Keybinds can be stored in proxies
    auto keyQ     = iface->key('Q');
    auto keyE     = iface->key('E');
    auto keyO     = iface->key('O');
    auto keyP     = iface->key('P');
    auto keyMinus = iface->key('-');
    auto keyEqual = iface->key('=');
    auto keyLSB   = iface->key('[');
    auto keyRSB   = iface->key(']');
    auto keyW     = iface->key('W');
    auto keySpace = iface->key(' ');
    auto keyESC   = iface->key(0_ivkFunc);
    auto keyF1    = iface->key(1_ivkFunc);
    auto keyF2    = iface->key(2_ivkFunc);
    auto keyF5    = iface->key(5_ivkFunc);

    //Poll must be called every frame
    iface->poll();
    sixense.poll();

    // Sixense input
    gemScale = 1.f - sixense.getTrigger(0);

    auto mousePos = iface->getMousePos();

    //Key Proxies can be cast to bool
    if (keyESC || shouldClose())
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

    //Key Proxies have edge detectors
    if (keyF1.pressed())
    {
        shaderOn = !shaderOn;
    }

    //Key Proxies have edge detectors
    if (keyF2.pressed())
    {
        highDef = !highDef;
    }

    fontRoll.tick();
    if (keySpace) fontRoll.reset();

    if (keyQ) rotation = wrap(rotation-=2.0, 0.0f, 360.0f);
    if (keyE) rotation = wrap(rotation+=2.0, 0.0f, 360.0f);

    //Inugami has several math functions, including wrap()
    rotation = wrap(rotation, 0.0f, 360.0f);

    if (keyO || keyMinus) dissolveMin = clamp(dissolveMin-=0.005f, -0.25f, 1.25f);
    if (keyP || keyEqual) dissolveMin = clamp(dissolveMin+=0.005f, -0.25f, 1.25f);

    if (keyLSB || keyMinus) dissolveMax = clamp(dissolveMax-=0.005f, -0.25f, 1.25f);
    if (keyRSB || keyEqual) dissolveMax = clamp(dissolveMax+=0.005f, -0.25f, 1.25f);

    //This is used for the shaders below
    ++ticks;

    //Shaders have easy access to uniforms
    if (shaderOn)
    {
        crazyShader.bind();
        crazyShader.uniform("dissolveMin").set( float(dissolveMin) );
        crazyShader.uniform("dissolveMax").set( float(dissolveMax) );
        crazyShader.uniform("hue").set( float(ticks/67.0) );

        Vec3 light(0.f,0.f,2.5f);
        light.x = mapRange(mousePos.x, 0,  getParams().width, -4.0, 4.0);
        light.y = mapRange(mousePos.y, getParams().height, 0, -3.0, 3.0);
        crazyShader.uniform("lightPos").set( light );

        for (int r=0; r<noise.height; ++r)
        {
            for (int c=0; c<noise.width; ++c)
            {
                auto&& pix = noise   .at(c, r);
                auto&& dir = noiseDir.at(c, r);
                for (int i=0; i<4; ++i)
                {
                    if (dir[i] == 1)
                    {
                        pix[i] += 1;
                        if (pix[i] == 255) dir[i] = 2;
                    }
                    else
                    {
                        pix[i] -= 1;
                        if (pix[i] == 0) dir[i] = 1;
                    }
                }
            }
        }

        noiseTex = Texture(noise, true, false);
        noiseTex.bind(1);
    }
}

void CustomCore::draw()
{
    ScopedProfile prof(profiler, "CustomCore: Draw");

    //beginFrame() sets the OpenGL context to the proper initial state
    beginFrame();

    {
        ScopedProfile prof(profiler, "3D");

        setShader((shaderOn)? crazyShader : defaultShader);

        //Cameras have view setters based on GLUT
        Camera cam;
        float aspect = float(getParams().width)/float(getParams().height);
        cam.perspective(70.f, aspect, 0.1f, 100.f);

        //Transforms are matrix stacks
        Transform mat;
        mat.translate(Vec3{0.f, 0.f, -3.f});

        // Sixense rotation
        {
            Vec3 pos {0.f, 0.f, 0.f};
            Vec3 target = sixense.getForward(0);
            Vec3 up {0.f, 1.f, 0.f};

            target.x = -target.x;
            mat.push(glm::lookAt(pos, target, up));
        }

        {
            cam.depthTest = true;
            cam.cullFaces = false;
            applyCam(cam);

            modelMatrix(mat);

            //Textures are set using bind()
            shieldTex.bind(0);
            noiseTex.bind(1);

            if (shaderOn) crazyShader.uniform("useDissolve").set(1);

            //Models use the currently bound texture
            if (highDef) shieldHD.draw();
            else         shield  .draw();
        }

        {
            cam.depthTest = true;
            cam.cullFaces = true;
            applyCam(cam);

            mat.push();
            mat.scale(gemScale*Vec3{0.995f, 0.995f, 0.995f});
            modelMatrix(mat);
            mat.pop();

            //Textures are set using bind()
            glassTex.bind(0);
            noiseTex.bind(1);

            if (shaderOn) crazyShader.uniform("useDissolve").set(0);

            //Models use the currently bound texture
            if (highDef) shieldHD.draw();
            else         shield  .draw();
        }
    }

    {
        ScopedProfile prof(profiler, "2D");

        setShader(defaultShader);

        Camera cam;
        cam.ortho(-8.f, 8.f, -6.f, 6.f, -10.f, 10.f);

        applyCam(cam);

        Vec3 target = sixense.getForward(0);
        float pi = std::atan(1.0)*4.f;
        float scalex = 1.f/(7.f*pi/8.f);
        float scaley = 1.f/(7.5f*pi/8.f);
        target.x /= scalex*target.z;
        target.y /= scaley*target.z;
        target *= -1.f;

        Transform mat;
        mat.translate(Vec3{target.x*4.f, target.y*3.f, -5.f});
        mat.rotate(rotation, Vec3{0.0, 0.0, 1.0});
        mat.scale(Vec3{0.125, 0.125, 1.0});

        //AnimatedSprites are completely automatic, and bind their own textures
        fontRoll.draw(*this, mat);
    }

    //endFrame() swaps the buffer to the screen
    endFrame();
}
