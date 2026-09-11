#version 330 core
// ^ This is a "version directive" and it MUST be the very first thing in any GLSL shader file.
//   The word "version" tells OpenGL which version of the GLSL language we want to use.
//   The "330" means version 3.30 (OpenGL 3.3).
//   The word "core" means we are using the "Core Profile", which is the modern, streamlined set of OpenGL features
//   (as opposed to the old "Compatibility Profile" that kept legacy features for backwards compatibility).
//
//   IMPORTANT CONCEPT: Every 3D object is drawn using "vertices" (the corners/points of the shape).
//   A "vertex shader" is a small program that runs on the GPU and gets called ONE TIME for EVERY single vertex.
//   Our cube below has 36 vertices (6 faces x 6 vertices, because each face is made of 2 triangles of 3 vertices each).
//   So whenever we draw the cube, this vertex shader function runs 36 times, once per vertex.

// ---------------------------------------------------------------------------
// STEP 1: Declaring what data comes IN to this shader from the C++ program.
// ---------------------------------------------------------------------------

layout (location = 0) in vec3 aPos;
// "layout (location = 0)"  = This unusual-looking syntax is called a "layout qualifier". Location 0 is basically a
//                            label/number that the GPU uses to find this piece of data. In our main.cpp file we used
//                            glVertexAttribPointer(0, ...) — notice the first argument is 0. Those two zeros MATCH:
//                            the C++ side writes position data to "slot 0", and this GLSL line reads it from "slot 0".
// "in"                     = This keyword means "this value comes INTO the shader from outside" (specifically from the
//                            vertex data array that we uploaded to the GPU from C++). It is an INPUT to this shader.
// "vec3"                   = This is the type. A vector with exactly 3 numbers inside it (like a small list of 3 floats).
//                            For a position, those 3 numbers are x, y, and z (left/right, up/down, near/far).
// "aPos"                   = This is a NAME we get to choose. It stands for "attribute Position". It will hold the x,y,z
//                            position of this particular vertex. Remember the C++ vertex array has each vertex as 6 floats,
//                            where the FIRST 3 floats (x,y,z) are the position — those are what gets stored here.

layout (location = 1) in vec3 aNormal;
// Same idea as the line above, but this input reads from "slot 1" instead of "slot 0".
// In main.cpp this matches glVertexAttribPointer(1, ...) where the first argument is 1.
// "vec3" holds three numbers.
// "aNormal" = "attribute Normal". A normal is a direction (a vector) that tells us which way a surface is facing.
//             For example, a flat top surface of our cube has a normal of (0, 1, 0), meaning "pointing straight up".
//             We need this direction later so we can calculate lighting (whether the light is shining on this surface).
//             In the C++ vertex array, each vertex's LAST 3 floats (the normal) are what get stored in this variable.

// ---------------------------------------------------------------------------
// STEP 2: Declaring what data goes OUT of this shader to the fragment shader.
// ---------------------------------------------------------------------------

out vec3 FragPos;
// "out"    = This keyword means "this value leaves this shader and is passed on to the next stage".
//            The next stage after a vertex shader is the "fragment shader" (the program that colors each pixel).
// "vec3"   = Three numbers (x, y, z).
// "FragPos"= "Fragment Position". We will calculate the world-space position of this vertex and put it here.
//            The fragment shader needs this so it can figure out lighting (e.g., distance to the light).
// IMPORTANT: The GPU will take the FragPos values from all 3 corners of a triangle and SMOOTHLY INTERPOLATE them
//            (blend them) across every pixel of that triangle, so inside the fragment shader every pixel sees its
//            own exact position, not just the corners.

out vec3 Normal;
// Same idea as the line above, but for the surface direction instead of the position.
// "out"    = leaving this shader, going to the fragment shader.
// "vec3"   = three numbers (x, y, z).
// "Normal" = the (world-space) direction this vertex's surface is facing.
// Like FragPos, this is interpolated across the triangle so every pixel gets a smoothly blended normal.
// This is needed so lighting looks smooth instead of broken up into flat triangles.

// ---------------------------------------------------------------------------
// STEP 3: Declaring "uniform" variables (values we set ONCE from C++, same for every vertex).
// ---------------------------------------------------------------------------

uniform mat4 model;
// "uniform" = A special kind of variable. Unlike "in", a uniform is NOT different per vertex.
//             It is the SAME value for every vertex in the whole draw call, but we can change it between draw calls.
//             We set its value from C++ using code like lightingShader.setMat4("model", model).
// "mat4"    = A 4x4 matrix. A matrix is like a grid of 16 numbers (4 rows x 4 columns) that represents a mathematical
//             operation. We use matrices to MOVE, ROTATE, and SCALE 3D objects.
// "model"   = The "model matrix". Its job is to transform a vertex from local/object space (the coordinates we typed
//             in the vertex array, centered around the origin 0,0,0) into world space (where the object actually sits
//             in the 3D scene, possibly translated/rotated/scaled). For our main cube this is the identity matrix
//             (no movement), but for the light cube it moves it to lightPos and shrinks it.

uniform mat4 view;
// "uniform"  = same value for every vertex, set once per frame from C++.
// "mat4"     = a 4x4 matrix.
// "view"     = The "view matrix". It transforms coordinates from WORLD space into VIEW space, which means
//              coordinates relative to the camera's position and orientation. Effectively, it answers:
//              "If I'm the camera, where is everything relative to me?" This is what lets us move the camera around.
//              We set it in C++ from camera.GetViewMatrix().

uniform mat4 projection;
// "uniform" = same value for every vertex, set once per frame from C++.
// "mat4"    = a 4x4 matrix.
// "projection" = The "projection matrix". It transforms coordinates from VIEW space into CLIP space and applies
//                 perspective. Perspective is what makes things farther away appear smaller. After this matrix,
//                 the coordinates are in a special normalized range that OpenGL uses to decide where on the screen
//                 (and how deep) each vertex appears. We build it in C++ with glm::perspective(...).

// ---------------------------------------------------------------------------
// STEP 4: The main function — the actual work of the vertex shader.
// ---------------------------------------------------------------------------

void main()
// "void" tells us this function does not return any value back to whoever called it.
// "main" is the special entry-point function name that OpenGL ALWAYS looks for in a shader.
// Every line of work inside the { } braces below runs for EACH individual vertex (36 times for our cube).
{   // This opening curly brace marks the START of the main() function's body (the code that runs).

    // Record the vertex's position in WORLD space so the fragment shader can use it for lighting.
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Reading this from the inside out:
    //   vec4(aPos, 1.0)  -> "aPos" only has 3 numbers (vec3). But we need to multiply it by a 4x4 matrix, and matrix
    //                       math requires 4-component vectors. So we "pad" it with an extra 1.0. That 1.0 is the "w"
    //                       component, which for a POSITION (a point) is always 1.0. (0.0 would mean a direction instead.)
    //   model * ...      -> Multiply the 4x4 model matrix by this 4-component position. Multiplication here is special
    //                       matrix-times-vector math, which applies rotation, scaling, and translation all at once.
    //                       The result is the vertex's position in WORLD space.
    //   vec3(...)        -> The multiplication gives us a 4-component result, but FragPos is only a vec3 (3 numbers).
    //                       So we wrap it in vec3(...) to discard the 4th (w) component and keep just x, y, z.
    //   FragPos = ...    -> Store the resulting world-space position into our "out" variable so it gets passed to the
    //                       fragment shader (and smoothly interpolated across the triangle as explained earlier).

    // Rotate the surface direction (normal) into world space so lighting is correct once the cube is rotated/scaled.
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // A normal is a DIRECTION, not a position. If we just multiplied aNormal by the model matrix the way we did aPos,
    // the normal would break whenever we scale the object unevenly. So we use a special matrix called the "normal matrix".
    // Reading from the inside out:
    //   inverse(model)  -> "inverse" builds a matrix that does exactly the OPPOSITE of the model matrix.
    //                       (If model moves an object forward, inverse moves it backward.)
    //   transpose(...)  -> "transpose" flips the rows and columns of that matrix (swaps them diagonally).
    //                       Combining inverse + transpose gives us the "normal matrix", which correctly transforms
    //                       normal directions even with uneven scaling.
    //   mat3(...)       -> Convert the 4x4 result down to a 3x3. Normals only have 3 components, and 3x3 is all we need
    //                       for rotating a direction (we don't need translation, which lives in the 4th column).
    //   ... * aNormal   -> Multiply this 3x3 normal matrix by the original local-space normal.
    //   Normal = ...    -> Store the now-world-space normal into our "out" variable for the fragment shader.

    // This is THE single most important line in a vertex shader: it defines where the vertex ends up on the screen.
    gl_Position = projection * view * vec4(FragPos, 1.0);
    // "gl_Position" is a SPECIAL built-in variable that OpenGL itself created. Every vertex shader MUST assign a value
    //   to it, otherwise OpenGL doesn't know where to put the vertex. Do not try to name it yourself — the "gl_" prefix
    //   means it's reserved and provided by OpenGL.
    // Reading the multiplication right-to-left (this is how matrix math works, the right side is applied first):
    //   vec4(FragPos, 1.0)  -> Take the world position we just computed and pad it to 4 components with w = 1.0.
    //   view * ...          -> Apply the view matrix: now the position is relative to the camera.
    //   projection * ...    -> Apply the projection matrix: now it's in clip space with perspective applied.
    // The final result is stored in gl_Position, which OpenGL uses to draw the vertex at the correct spot on screen.
}
// This closing curly brace marks the END of the main() function body.
