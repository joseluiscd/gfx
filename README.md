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

Optional dependencies:
- Freetype 2.10: If package is found and `GFX_IMGUI_FREETYPE` CMake option is enabled,
it is used as the default rasterizer for ImGui fonts.

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
`gfx::RenderPipeline` is the description of a render pipeline (currently only the shaders).
`gfx::RenderPass` is the class used to describe a render pass on a given surface and draw things.

~~~cpp
app.main_loop([&]() {
    gfx::RenderPass(app.get_surface(), gfx::ClearOperation::color())
            .set_pipeline(pipeline)
            .draw(triangle);
});
~~~

## Missing features (planned)

- Compute shaders
- Instanced rendering
- Finish the debug drawing API
- Shader Storage Buffer
- Uniform Storage Buffer

## TODO

Some tasks to improve the library:

- Camera refactor (different `gfx::CameraRig` controls)
