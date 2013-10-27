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

#include "inugami/exception.hpp"

#include <fstream>
#include <iostream>
#include <exception>
#include <functional>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Inugami;

void dumpProfiles();
void errorMessage(const char*);

int main(int argc, char* argv[])
{
    profiler = new Profiler();
    ScopedProfile prof(profiler, "Main");

    std::ofstream logfile("log.txt");
    logger = new Logger<>(logfile);

    CustomCore::RenderParams renparams;
    renparams.fsaaSamples = 4;

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

    try
    {
        logger->log("Creating Core...");
        CustomCore base(renparams);
        logger->log("Go!");
        base.go();
    }
    catch (const std::exception& e)
    {
        errorMessage(e.what());
        return -1;
    }
    catch (...)
    {
        errorMessage("Unknown error!");
        return -1;
    }

    dumpProfiles();

    return 0;
}

void dumpProfiles()
{
    using Prof = Inugami::Profiler::Profile;

    std::ofstream pfile("profile.txt");

    auto all = profiler->getAll();

    std::function<void(const Prof::Ptr&, std::string)> dumProf;
    dumProf = [&](const Prof::Ptr& in, std::string indent)
    {
        pfile << indent << "Min: " << in->min     << "\n";
        pfile << indent << "Max: " << in->max     << "\n";
        pfile << indent << "Avg: " << in->average << "\n";
        pfile << indent << "Num: " << in->samples << "\n\n";
        for (auto& p : in->getChildren())
        {
            pfile << indent << p.first << ":\n";
            dumProf(p.second, indent+"\t");
        }
    };

    for (auto& p : all)
    {
        pfile << p.first << ":\n";
        dumProf(p.second, "\t");
    }
}

void errorMessage(const char* str)
{
#ifdef _WIN32
    MessageBoxA(nullptr, str, "Exception", MB_OK);
#endif
    logger->log(str);
}
