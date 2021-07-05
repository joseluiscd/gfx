# GFX (Low-level Graphics Library Abstraction)

Requires OpenGL 4.5 capable graphics card.

## Features

- Vertex Array Objects
- Buffers
- Vertex/fragment/geometry shaders
- Camera
- Render to texture
- GUI (via [imgui](https://github.com/ocornut/imgui))

## Dependencies

GFX requires at least:

- glfw 3.3
- glm 0.9
- fmtlib 7.0
- spdlog 1.8
- Freetype 2.10

## Usage

After creating a `gfx::Gfx`, you can start drawing in the function argument to its `main_loop()` method:

~~~cpp
gfx::Gfx app(gfx::InitOptions {
        .title = "Hello world",
        .resizable = false });

app.main_loop([&]() {
    // Your drawing code here
});
~~~

The most important objects are the `gfx::RenderPipeline` and `gfx::RenderPass`.
`RenderPipeline` is the description of a render pipeline (mainly the shaders).
`RenderPass` is the class used to describe a render pass on a given surface and draw things.

~~~cpp
app.main_loop([&]() {
    gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color())
            .set_pipeline(pipeline)
            .draw(triangle);
});
~~~

## Missing features (planned)

- Debug drawing API
- Using textures and samplers in shaders
- Compute shaders
- Instanced rendering

## TODO

Some tasks to improve the library:

- Camera refactor (different `gfx::CameraRig` controls)
- Make vertex attribute creation similar to uniforms
- Make some libraries optional (fmtlib, spdlog, freetype)