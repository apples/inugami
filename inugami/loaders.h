#ifndef INUGAMI_LOADERS_H
#define INUGAMI_LOADERS_H

#include "mesh.h"

#include <string>
#include <vector>

namespace Inugami {

bool loadImageFromFile(const std::string &fileName, std::vector<char> &target);
bool loadObjFromFile(const std::string &fileName, Mesh &target);

} // namespace Inugami

#endif // INUGAMI_LOADERS_H
