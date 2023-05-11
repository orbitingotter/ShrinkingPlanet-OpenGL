## Shrinking Planet - OpenGL

This repo is made in parallel with **[ShrinkingPlanet-Unity](https://github.com/orbitingotter/ShrinkingPlanet-Unity)** from scratch using
 my own engine from **[OpenGL repo](https://github.com/orbitingotter/OpenGL)**.

Preview -

![openglGIF](https://github.com/orbitingotter/ShrinkingPlanet-OpenGL/assets/112700146/7959a2e5-efc4-4107-9380-ca680662560b)


Compared to OpenGL repo, changes include the camera class (for following player) and material system for the shaders.


This game unlike the Unity version doesnot use any physics engine,
but uses matrix transformations to constrain the movement of player
around the sphere, and the falling of meteors.

Also included some debug info for fiddling around with the settings in ImGui.

