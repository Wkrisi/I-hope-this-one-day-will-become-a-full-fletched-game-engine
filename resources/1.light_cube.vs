#version 330 core
// ^ This MUST be the first line in any GLSL file.
//   "version"  = tells OpenGL which GLSL version we want.
//   "330"      = GLSL 3.30, which corresponds to OpenGL 3.3.
//   "core"     = use the modern Core Profile of OpenGL.
//
//   WHAT THIS SHADER IS FOR: This is a SECOND, much simpler vertex shader. It is only used to draw the small white cube
//   that we render at the light's position, just so we can visually SEE where the light is. Because it's just a marker,
//   it doesn't need normals or any complex logic — it only needs each vertex's position.

layout (location = 0) in vec3 aPos;
// "layout (location = 0)" = A layout qualifier. The 0 is a label/slot number. It matches the FIRST argument (0) of
//                           glVertexAttribPointer(0, ...) in our main.cpp file, so the GPU knows this input should read
//                           from slot 0, which holds the position data.
// "in"                    = This value comes INTO this shader from outside (our vertex data in C++).
// "vec3"                  = The type: a vector holding exactly 3 numbers.
// "aPos"                  = The name we chose ("attribute Position"). It will hold this vertex's x, y, z position.
//                           The cube's vertex data contains 6 floats per vertex, but for this simple shader we only
//                           care about the first 3 (the position).


uniform mat4 model;
// "uniform" = A value set ONCE from C++ per draw call; it is identical for every vertex.
// "mat4"    = A 4x4 matrix (a grid of 16 numbers).
// "model"   = The model matrix. For the light cube it (a) MOVES the cube to where the light is (lightPos) and
//             (b) SCALES it down to 20% size, so it's a small box sitting at the light's location.

uniform mat4 view;
// "uniform" = set once per frame from C++.
// "mat4"    = a 4x4 matrix.
// "view"    = The view matrix. It transforms world coordinates into coordinates relative to the camera.

uniform mat4 projection;
// "uniform" = set once per frame from C++.
// "mat4"    = a 4x4 matrix.
// "projection" = The projection matrix. It adds perspective and converts coordinates into the clip space range that
//                OpenGL uses to place vertices on screen.

void main()
// "void"    = this function returns nothing.
// "main"    = the required entry point; OpenGL runs this once per vertex (36 times for the light cube).
{   // Opening brace: start of the function body.

	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// "gl_Position" is OpenGL's built-in output variable. Every vertex shader MUST set it.
	//   It is the final clip-space position OpenGL uses to draw the vertex.
	// The multiplication happens RIGHT TO LEFT (the right-most multiplication is done first):
	//   vec4(aPos, 1.0)  -> pad the 3-number position with a 4th component, w = 1.0, so we can multiply by a 4x4 matrix.
	//   model * ...      -> FIRST move the vertex into world space (build the light cube at lightPos, scaled to 20%).
	//   view * ...       -> THEN make the world coordinate relative to the camera.
	//   projection * ... -> FINALLY apply perspective to get the clip-space coordinate.
	// The whole chain converts: local space -> world space -> view space -> clip space.
	// The result is stored in gl_Position.
}
// Closing brace: end of the function body.
