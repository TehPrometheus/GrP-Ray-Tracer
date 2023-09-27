![image](https://github.com/TehPrometheus/GrP-Ray-Tracer/assets/95450900/37e50478-86ce-4a27-b532-2eee49188de0)![image](https://github.com/TehPrometheus/GrP-Ray-Tracer/assets/95450900/94b9c58e-39a6-49b6-ad3a-895c676622a2)![image](https://github.com/TehPrometheus/GrP-Ray-Tracer/assets/95450900/d26fb712-fd31-4fe4-800d-07f9227a1829)outline:
* We raytrace primitives like spheres and planes using a fixed camera and simple material
* We implement the camera and add basic shadows
* We implement more complex, physically based lighting (PBR)
* We expand to raytrace trianglemeshes
* We optimize using multithreading techniques and acceleration structures

# Graphics Production Course Project: Ray Tracer
A custom ray tracer developed in C++ from the ground up for the Graphics Production Course at the DAE school.

## §1. Spheres & Planes
First we write out the code for a basic raytracer using a fixed camera and simple material on primitives like spheres and planes:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

A simple scene of spheres and planes using basic materials gives us the following result:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig2.png)

## §2. Camera & Shadows
Second, we add camera functionality and a basic directional light. If a pixel hits a material, we check if it's shadowed for each light:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

Now our scene looks like this:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

## §3. Physically Based Lighting (PBR)
Third, we implement more complex lighting by approximating the rendering equation. This involves describing how light scatters off of a material, the radiance of said light and the observed area:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

Light scattering can be simulated using the Bidirectional Reflectance Distribution Function (BRDF), which is a part of the rendering equation. After implementing a Lambert Diffuse BRDF and accounting for specular reflection, we get the following:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

No surface is perfectly smooth, you'll always find tiny dents and bumps on a molecular level. These imperfections shape how we view objects when light interacts with them. We need to take these into account if we're serious about having physically accurate scattering. Implementing these imperfections by tweaking the BRDF renders the final scene:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)
## §4. Triangle Meshes

So far we've only collided our rays with primitives, what about triangle meshes? We first write out the code for a single triangle:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)
Once this works, we give the renderer a simple .obj file containing a bunch of triangles which constitute a mesh:

![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)


## §5. Optimization

Running a ray tracer is expensive due to the amount of loops it requires. Some optimization is in order.
First we add a basic acceleration structure called the slab-test. This test defines a box around our mesh using the smallest and largest x,y,z values as the corners:


![](https://github.com/TehPrometheus/GrP-Ray-Tracer/blob/main/lib/spheres_and_planes_fig1.png)

Lastly, we leverage our multicore processors by delegating the task of rendering a pixel to each thread. To stay as close to C++ standard as possible, we use `std::async` and `std::parallel_for`.
Implementing these optimisations increases our framerate almost sevenfold, from ± 13FPS to ± 87FPS.




























