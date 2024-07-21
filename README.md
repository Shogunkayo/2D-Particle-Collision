## Introduction

This project is an implementation of a 2D particle collision simulation.
- It consists of multiple particles of various masses ( the larger the particle, the heavier it is ) moving around in a fixed space.
- These particles may :
1. Collide with the boundaries.
2. Collide with each other.
- Each collision is treated as a perfectly elastic collision.

## Demo

## High Level implementation

1. Particle-Boundary Collision :
- It is simpler since the particle is colliding with a simple rigid body.
- A simple distance measure between the extremes of the particle and the boundary’s coordinates is made to determine if there is a possible collision.
- The velocity’s signs are reversed accordingly.

2. Particle-Particle Detection :
- Collision Detection: While the naive method would be to test for collision between each and every pair of particles before the next frame, we use an optimized algorithm called sweep-and-prune.
- Collision Handling: Once a collision is detected between 2 particles, we apply laws of conservation of momentum and conservation of kinetic energy to determine the new velocities of the particles in question.

## Low Level Implementation

- All the rendering work is done on the GPU using shader cores. We used OpenGL and GLSL.
- Fragment shader takes care of drawing circles from a two triangle vertice set.
- Instancing takes care of rendering objects of similar vertex attributes, and allows us to render all the particles in a single OpenGL Draw() call.
- For the sweep-and-prune algorithm, we have parallelized the for loops (used for sweeping through the list of particles) using OpenMP directives. Using CPU cores rather than GPU cores made more sense as there are a lot of branching statements.
