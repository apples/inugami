/*******************************************************************************
 * Inugami - An OpenGL framework designed for rapid game development
 * Version: 0.2.0
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

using namespace Inugami;

void dumpProfiles();

int main()
{
    profiler = new Profiler();
    ScopedProfile prof(profiler, "Main");

    std::ofstream logfile("log.txt");
    logger = new Logger<5>(logfile);

    CustomCore::RenderParams renparams;
    renparams.width = 1024;
    renparams.height = 768;
    renparams.fullscreen = false;
    renparams.vsync = false;
    renparams.fsaaSamples = 4;

    try
    {
        logger->log<5>("Creating Core...");
        CustomCore base(renparams);
        logger->log<5>("Go!");
        base.go();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        std::ofstream("error.txt") << e.what();
        return -1;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << std::endl;
        std::ofstream("error.txt") << "Unknown exception.";
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
    dumProf = [&pfile, &dumProf](const Prof::Ptr& in, std::string indent)
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
