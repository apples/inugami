#ifndef INUGAMI_LOADERS_H
#define INUGAMI_LOADERS_H

#include "mesh.h"

#include <string>
#include <vector>

namespace Inugami {

bool loadImageFromFile(const std::string &filename, std::vector<char> &target);
bool loadTextFromFile(const std::string &filename, std::string &target);
bool loadObjFromFile(const std::string &filename, Mesh &target);

} // namespace Inugami

#endif // INUGAMI_LOADERS_H
