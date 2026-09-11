#version 330 core
// ^ The version directive, always the first line of a GLSL file.
//   "version"  = tells OpenGL which GLSL version to use.
//   "330"      = GLSL 3.30 (OpenGL 3.3).
//   "core"     = the modern Core Profile.
//
//   WHAT THIS SHADER IS FOR: This is a "fragment shader". The word "fragment" means a single pixel-sized piece of a
//   rendered shape. A fragment shader runs ONCE for EVERY pixel the object covers on the screen. Its only real job is
//   to decide the final color of each pixel (that is, what you actually SEE).
//   This particular fragment shader is used for the small light-marking cube: it just makes every pixel solid white.

out vec4 FragColor;
// "out"     = The value is sent OUT of the shader, to OpenGL's framebuffer (the memory that eventually becomes the
//             pixels you see on your monitor).
// "vec4"    = A vector with FOUR components: red, green, blue, AND alpha (transparency).
//             Alpha works on a scale of 0.0 (fully transparent / invisible) to 1.0 (fully opaque / solid).
// "FragColor" = The name we chose for our output color. This is the color that gets written to the screen for each pixel.

in vec3 Normal;
// "in"     = This value comes INTO this shader from the previous stage (the vertex shader).
// "vec3"   = A vector with 3 components.
// "Normal" = This is declared to represent a surface normal, but it is never actually used below and the vertex shader
//            for the light cube never even sends one. So in THIS shader it does nothing — it's harmless leftover code.
//            (Because we don't read it, it has no effect on what we see.)

void main()
// "void" = this function returns nothing.
// "main" = the required entry point; the GPU runs this once for every pixel of the light cube.
{   // Opening brace: start of the function body.

    FragColor = vec4(1.0); // set all 4 vector values to 1.0
    // Assign our output color. vec4(1.0) is shorthand that sets ALL FOUR components to the value 1.0, meaning:
    //   red   = 1.0
    //   green = 1.0
    //   blue  = 1.0
    //   alpha = 1.0
    // Having red, green, and blue all at their maximum value (1.0) produces WHITE (because red + green + blue light
    // combined at full strength looks white). An alpha of 1.0 means the pixel is fully opaque (not see-through).
    // The result: every pixel of the light cube is drawn as solid, bright white.
}
// Closing brace: end of the function body.
