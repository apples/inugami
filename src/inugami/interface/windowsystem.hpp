#ifndef INUGAMI_INTERFACE_WINDOWSYSTEM_HPP
#define INUGAMI_INTERFACE_WINDOWSYSTEM_HPP

#include "window.hpp"

namespace Inugami {
namespace Interface_detail {

Window makeWindow(WindowParams wp);
void pollEvents();

} // namespace Interface_detail

using Interface_detail::makeWindow;
using Interface_detail::pollEvents;

} // namespace Inugami

#endif // INUGAMI_INTERFACE_WINDOWSYSTEM_HPP
