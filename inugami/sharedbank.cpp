#include "sharedbank.hpp"

namespace Inugami {

SharedBank::SharedBank() :
    shader(nullptr)
{}

SharedBank::~SharedBank()
{
    delete shader;
}

} // namespace Inugami
