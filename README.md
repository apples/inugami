# README

## Introduction

### What is Inugami?

Inugami is an cross-platform OpenGL game framework that is built upon [GLFW](http://www.glfw.org/).
It is designed to encapsulate common OpenGL boilerplate code into a fully object-oriented system.
Although Inugami is somewhat complex, it is very easy to use.

Inugami makes it very easy to just "get something on the screen" while
still providing access to low-level features.

#### Some things Inugami can do:

  - Get a window on the screen (or multiple windows!)
  - Load textures, models, and shaders
  - Manage 2D sprites and animations
  - Take input from keyboard, mouse, and gamepad

### What is Inugami *not*?

Inugami is *not* a library or engine, it is a framework.
It is not designed to be simply linked to the user's code;
the user is expected to build their project on top of Inugami.

Inugami does *not* provide a scene graph or any similar construct.


#### Some things Inugami *cannot* do:

  - Physics
  - Game logic
  - Scripting and events
  - Manage world state

### What will Inugami become?

Inugami started off as a simple boilerplate file that I carried from one project to the next,
but it soon became a project itself. I would like Inugami to become a beginner-friendly
framework that doesn't restrict the user. There is a distinct lack of game frameworks for C++.
Sure, there are gems such as SDL, SFML, and Ogre3D, but almost all of them are either too outdated,
too bloated, or too restrictive.

#### Some things Inugami might be able to do in the future:

  - 3D Animations
  
    3D animations can be very difficult to program properly.
    This is a huge barrier for beginners looking to make their first 3D game.
    
  - Networking
    
    There aren't any good networking frameworks for C++.
    Engines like Raknet are too much for small games,
    and more "low-level" networking libraries are not beginner-friendly.
  
  - Other things?
  
    I would love to hear what people want out of a game framework.
    Inugami is currently developed according to my personal needs,
    so there are definitely a lot of useful features that I have not thought of yet.
    If you have a feature suggestion for Inugami, just send me an email.

## Installation

Inugami currently uses [Code::Blocks](http://www.codeblocks.org/) as a build system.
Just duplicate the Inugami project, rename it, and start coding!

Of course, you don't *need* Code::Blocks,
but I haven't bothered making other build methods.
Other IDEs might be able to convert the project,
but the code has only been tested with Code::Blocks.

**Make sure to enable C++11 in your compiler!**
Inugami makes heavy use of C++11 features,
such as lambdas, move semantics, and string literals.

### Dependencies

  - All platforms
  
      - GLFW 3.0.0
      
      - GLEW 1.9.0
      
      - GLM 0.9
      
  - Windows has no other dependencies, as far as I can tell.
  
      - I recommend using [this version of MinGW](http://nuwen.net/mingw.html).
  
  - Linux
  
      - X11 development libraries
      
  - Mac - I have no idea, you're on your own.

## Optional dependencies

Right now, the only other library is libpng, used to load images.
If you don't want to load PNG images, you can simply remove `Image::fromPNG()`.

## Documentation

Doxygen documentation is available for pretty much everything.
Although a full tutorial is planned,
the example project should be enough to learn the basics.

## License

Inugami is licensed under the unmodified zlib/libpng liscense. Full text can be
found [here](http://opensource.org/licenses/zlib-license.php).

For those unfamiliar, this basically means that you can do whatever you want
with the source code as long as you don't claim ownership or remove the
license.

## Contact

Jeramy Harrison

dbralir@gmail.com
