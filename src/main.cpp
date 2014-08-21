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

#include "inugami/inugami.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <exception>
#include <functional>
#include <unordered_map>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;
using namespace std::chrono;
using namespace Inugami;

void errorMessage(const char*);

int main(int argc, char* argv[]) try
{
    WindowParams renparams;
    renparams.samples = 4;

    {
        std::unordered_map<std::string, std::function<void()>> argf = {
              {"--fullscreen", [&]{renparams.fullscreen=true;}}
            , {"--windowed",   [&]{renparams.fullscreen=false;}}
            , {"--vsync",      [&]{renparams.vsync=true;}}
            , {"--no-vsync",   [&]{renparams.vsync=false;}}
        };

        while (*++argv)
        {
            auto iter = argf.find(*argv);
            if (iter != end(argf)) iter->second();
        }
    }

    auto win = makeWindow(renparams);
    win.activate();

    int i = 0;

    Shader shader (ShaderProgram::fromDefault());

    shader.bind();

    auto mvpu = shader.uniform("MVP");

    Texture tex (Image{1,1,{255,0,0,255}});

    Mesh square (Geometry::fromRect(1, 1));

    while (true)
    {
        cout << "Polling" << endl;
        pollEvents();

        if (win.shouldClose())
        {
            cout << "Bye!" << endl;
            break;
        }

        if (win.getKey(Key::character('A')).pressed())
        {
            cout << "A pressed! " << ++i << endl;
        }

        if (win.getKey(Key::character('A')).released())
        {
            cout << "A released! " << ++i << endl;
        }

        // Draw

        win.clearBuffer();

        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;

        projection = glm::perspective(90.f, 4.f/3.f, 0.05f, 100.f);
        view = glm::mat4(1.f);
        model = glm::translate(model, {0.f, 0.f, -5.f});
        model = glm::rotate(model, float(i), {0.f, 1.f, 0.f});

        glm::mat4 mvp = projection*view*model;

        if (!mvpu.set(mvp)) throw;

        tex.bind(0);
        square.draw();

        win.swapBuffers();

        cout << "---" << endl;
        this_thread::sleep_for(milliseconds(20));
    }

    return 0;
}
catch (const std::exception& e)
{
    errorMessage(e.what());
    return -1;
}

void errorMessage(const char* str)
{
#ifdef _WIN32
    MessageBoxA(nullptr, str, "Exception", MB_OK);
#endif
    cerr << str << endl;
}
