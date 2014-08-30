#WolfPack

I'm writing a clone of the Wolf3D renderer. It probably won't be anywhere close to performing on era hardware, but I'm using similar techniques and trying to make it clean, if not maximally efficient. 

Currently a raycaster with texture mapping (greyscale only for a short interim), with player movement and look (z rotation only).

Use WASD to move - it's tank controls like in original Wolfenstein.

#Building

Requires openframeworks, no plugins.
The project is set up for VS2013 but can probably be built just fine under Linux or OSX, unless OF has bugs on those platforms - I'm really just using OF for handling input and drawing the frames I generate (on the CPU) to the screen. 


Once I finish this I may clone the DOOM renderer, though something tells me that will be significantly harder.
