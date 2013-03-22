#ifndef INUGAMI_SHAREDBANK_H
#define INUGAMI_SHAREDBANK_H

#include "inugami.hpp"

#include "mesh.hpp"
#include "texture.hpp"
#include "spritesheet.hpp"
#include "shader.hpp"

namespace Inugami {

class SharedBank final
{
    friend class Core;

public:
    ~SharedBank();

    Mesh::Bank        meshBank;
    Texture::Bank     textureBank;
    Spritesheet::Bank spritesheetBank;

    Shader* shader;

    int users;

private:
    SharedBank();
};

} // namespace Inugami

#endif // INUGAMI_SHAREDBANK_H
