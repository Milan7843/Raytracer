Raytracer made in C++, using [OpenGL](https://www.opengl.org/), [GLFW](https://www.glfw.org/), [Glad](https://glad.dav1d.de/), [assimp](https://github.com/assimp/assimp)

# How to run
Simply clone the repository and run `Raytracer.exe` from inside the `Build` folder.

# Notable features
- [Full user interface with rasterized and rendered view](#full-user-interface)
- Custom model loading
- [Loading scene from / saving scene to disk](#scene-disk-management)
- [Fully integrated object translation system](#object-translation-system)
- [Material creation + texture, normal map support](#materials)
- Various light sources
- [Rendering using the GPU via compute shaders](#compute-shaders)
- [Fast rendering at high tri count using Bounding Volume Hierarchy](#bounding-volume-hierarchy)
- Basic importance sampling
- And much more!

## Full user interface
The application includes a currently work-in-progress user interface made with ImGui.
There are two modes:

### Rendered view
Rasterized view, which shows the current scene in a high-performance way.
This way the user can easily make changes to the scene and quickly get an idea of what the final render will look like.
![Rendered View](https://github.com/Milan7843/Raytracer/assets/55874545/43e19033-1e49-4fd3-84ec-ce577cb604ef)

### Raytraced view
This view allows the user to view the image that is currently being rendered. It displays the image as it is being rendered so the user can get an idea of the progress.
![Raytraced View](https://github.com/Milan7843/Raytracer/assets/55874545/136fe221-70b5-4427-af41-a8081b2524ab)

## Scene disk management
Scenes can be saved to the disk and loaded from the disk.<br>
![Scene Disk Management](https://github.com/Milan7843/Raytracer/assets/55874545/58d08196-5e25-4dd8-a459-bae845feca53)<br>
The program will also keep track of any changes, so no changes will accidentally be lost.<br>
![Scene Disk Management](https://github.com/Milan7843/Raytracer/assets/55874545/3bafaced-9062-4c99-86b5-80181456ff40)

## Object translation system
The position, rotation and scale of objects in the scene can easily be manipulated using gizmos.
<div style="display: flex; justify-content: space-between; align-items: center;">
    <div style="flex: 1;">
        <p align="center">Position</p>
        <img src="https://github.com/Milan7843/Raytracer/assets/55874545/396750ce-5379-44e7-a516-912c12bc1a32" alt="Position" width="100%">
    </div>
    <div style="flex: 1;">
        <p align="center">Rotation</p>
        <img src="https://github.com/Milan7843/Raytracer/assets/55874545/75278383-4dcf-455f-ab81-0a9edc9268bb" alt="Rotation" width="100%">
    </div>
    <div style="flex: 1;">
        <p align="center">Scale</p>
        <img src="https://github.com/Milan7843/Raytracer/assets/55874545/cf8adb51-7610-4d15-bf6d-e0cbfafb6a5a" alt="Scale" width="100%">
    </div>
</div>

## Materials
Materials can be created, used and manipulated within the application.
![image](https://github.com/Milan7843/Raytracer/assets/55874545/eef87361-202e-4638-8b24-34ae26f23534)


## Compute shaders
Compute shaders are used to offload the rendering load to the GPU.

## Bounding Volume Hierarchy
Rendering objects is implemented with a BVH system that automatically generates BVHs for all objects.
This allows objects with $N$ triangles to be rendered in $O(\log N)$ time.
![Bounding Volume Hierarchy](https://github.com/Milan7843/Raytracer/assets/55874545/49e62175-527c-44b4-b63c-4ce7bb50ffc6)
