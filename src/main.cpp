// ============================================================================
// INCLUDES — bringing in code other people already wrote so we can use it.
// ============================================================================
//   An #include line basically copies the contents of another file into this one before the program is compiled.
//   This lets us use functions and types that are defined in those other files without writing them ourselves.

#include <glad/glad.h>                                      // We include GLAD FIRST (order matters!), before GLFW.
// What it provides: GLAD is a small "loader" library. It knows the names of all the modern OpenGL functions, but it
//   needs special code to actually find those functions inside your graphics driver at runtime. GLAD handles that.
// Why we need it: Without GLAD, the OpenGL functions would all be empty pointers and everything would crash. The
//   < > angle brackets tell the compiler to look for this file in the standard/configured include directories.
// Why first: GLAD must be included before GLFW because GLFW's header references OpenGL constants that GLAD defines.

#include <GLFW/glfw3.h>                                    // GLFW (Graphics Library Framework) header.
// What it provides: GLFW is a general-purpose library for creating a window, creating the OpenGL context (the thing
//   that lets us issue rendering commands), handling keyboard/mouse input, and managing the main application loop.
// Why we need it: This is what physically opens the window on your operating system (Windows/macOS/Linux).

#include <glm/glm.hpp>                                     // GLM (OpenGL Mathematics) — math types header.
// What it provides: GLM is a header-only math library that gives us handy vector and matrix types and functions,
//   like glm::vec3 (a 3-number vector) and glm::mat4 (a 4x4 matrix). OpenGL has no built-in math types, so we use GLM
//   to represent positions, directions, and transformation matrices in C++.
// The .hpp extension is just the C++ header convention. glm.hpp is the base header with the core types.

#include <glm/gtc/matrix_transform.hpp>                    // GLM transformation helper functions.
// What it provides: This sub-header ("gtc/general template collection") defines convenient functions for building
//   transformation matrices, specifically glm::perspective(...) to make a projection matrix, and glm::translate(...)
//   and glm::scale(...) which we use to move and shrink the light cube.

#include <glm/gtc/type_ptr.hpp>                             // GLM type-pointer helpers.
// What it provides: Helper functions for converting between GLM vectors/matrices and raw C arrays/pointers that
//   OpenGL can understand (like glm::value_ptr). It's not heavily used in this file, but it's a very common include
//   in OpenGL projects so it's kept here as standard practice.

#include <shader_s.h>                                      // LearnOpenGL's Shader helper class.
// What it provides: A small class called "Shader". It reads our GLSL shader source files from disk, compiles them,
//   links them into a shader program, and exposes handy methods like .use() (activate the program), .setVec3(...),
//   .setFloat(...), and .setMat4(...) for uploading uniform values.
// Why: It saves us from writing dozens of lines of raw OpenGL shader-compilation boilerplate by hand.

#include <camera.h>                                        // A Camera helper class.
// What it provides: A class called "Camera" that stores the camera's position and orientation (position, front/up
//   vectors, zoom) and offers methods like GetViewMatrix(), ProcessKeyboard(...), ProcessMouseMovement(...), and
//   ProcessMouseScroll(...). This lets us move the camera with WASD/keyboard and rotate with the mouse.

#include "imgui.h"                                          // Dear ImGui — the main API header.
// What it provides: Dear ImGui is a library for building immediate-mode graphical user interfaces (GUI). It lets us
//   easily draw windows, sliders, and buttons. This header declares the core ImGui API.
// NOTE: quotes (") instead of < > mean the compiler looks for this file starting in our own source directories first.

#include "backends/imgui_impl_glfw.h"                      // ImGui's GLFW backend.
// What it provides: ImGui needs to hook into our window and input system to work. This backend connects ImGui to
//   GLFW, so ImGui knows about our window and can receive mouse/keyboard input from it.

#include "backends/imgui_impl_opengl3.h"                   // ImGui's OpenGL 3 backend.
// What it provides: This backend makes ImGui able to actually RENDER (draw) its interface using our OpenGL context,
//   specifically OpenGL 3.x+. It turns ImGui's UI descriptions into real rendered graphics.

#include <iostream>                                        // Standard input/output stream header.
// What it provides: std::cout (used to print text to the terminal) and related tools. We use it to print error
//   messages if something goes wrong, so we can see what failed.


// ============================================================================
// FUNCTION DECLARATIONS — telling the compiler "these functions exist".
// ============================================================================
//   A "function declaration" (also called a prototype) just announces to the compiler that a function exists, what it
//   is named, and what arguments it takes. The actual body (implementation) of each function is written further down in
//   this file. Declaring them up front lets us use them before their definitions appear.

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // Declares a callback GLFW will call whenever the framebuffer (drawing area) changes size (e.g. when you resize the window).
// What this is: "void" means the function returns no value. The name is what GLFW looks for to know our code reacts to
//   window resizing. "GLFWwindow* window" is a POINTER to our window object (a pointer is like an address telling us
//   where the window data lives in memory). The "int width" and "int height" are the new window/framebuffer dimensions.

void mouse_callback(GLFWwindow* window, double xpos, double ypos);         // Declares a callback GLFW calls every time the mouse moves.
// "void" = returns nothing. Takes the window pointer and the mouse's current x and y positions (as doubles/decimals).

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // Declares a callback GLFW calls when the mouse wheel scrolls; xoffset/yoffset are the scroll amounts.
// "void" = returns nothing. Takes the window and two scroll offsets. We only use yoffset (vertical scroll) for zooming.

void processInput(GLFWwindow* window);                                      // Declares our OWN helper function that checks keyboard input each frame.
// "void" = returns nothing. This is NOT a GLFW callback; WE call it ourselves every frame. It takes the window so it
//   can ask GLFW which keys are currently pressed.


// ============================================================================
// WINDOW SETTINGS — fixed constants describing our window.
// ============================================================================

const unsigned int SCR_WIDTH = 800;                                       // Width of our application window in pixels.
// "const" means this value cannot be changed after we set it (it's a constant). "unsigned" means it can never be
//   negative. "int" means a whole number. "SCR_WIDTH" is the name (short for "screen width"). 800 is how wide the
//   window is.

const unsigned int SCR_HEIGHT = 600;                                      // Height of our application window in pixels.
// Same idea as above, but for the window's height: 600 pixels.


// ============================================================================
// CAMERA — the object that represents where we are looking from.
// ============================================================================

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));                              // Creates a Camera object positioned at world coordinates (0, 0, 3).
// This line creates an actual instance of our Camera class and names it "camera". The thing in parentheses,
//   glm::vec3(0.0f, 0.0f, 3.0f), is the camera's starting WORLD position given to its constructor: x=0, y=0, z=3.
// "glm::vec3" is GLM's 3-number vector type; "0.0f" is the number 0 as a float (the 'f' makes it a float literal).
// Because z=3 and the cube sits at z=0, the camera starts 3 units in front of the cube, so we should see it.

float lastX = SCR_WIDTH / 2.0f;                                           // Stores the previous horizontal (x) mouse position.
// "float" = a number with decimals. "lastX" is our own variable. We initialize it to SCR_WIDTH/2.0f = 800/2 = 400,
//   the horizontal CENTER of the screen. Starting at the center prevents a sudden jump in the camera when the mouse
//   is first used (because we can't know the mouse position until the first mouse event).

float lastY = SCR_HEIGHT / 2.0f;                                          // Stores the previous vertical (y) mouse position.
// Same as lastX, but for the y axis. We start it at SCR_HEIGHT/2.0f = 600/2 = 300, the vertical center of the screen.

bool firstMouse = true;                                                   // A flag that tells us whether we've received the first mouse movement yet.
// "bool" = a boolean, which can only be true or false. "firstMouse" starts as true (we have NOT processed the first
//   mouse event). It lets us handle the very first mouse movement specially so the camera doesn't jump unexpectedly.


// ============================================================================
// MOUSE MODE — whether the mouse controls the camera or the GUI.
// ============================================================================

bool mouseCaptured = true;                                                // true = mouse is captured by GLFW for camera control; false = mouse is free for ImGui.
// This decides who "owns" the mouse. When true, GLFW hides/captures the cursor and mouse movement rotates the camera.
//   When false, the cursor is visible and usable for ImGui sliders/buttons. We toggle this with the TAB key.

bool tabWasPressed = false;                                               // Remembers whether TAB was pressed during the PREVIOUS frame.
// We use this to detect a single, fresh press of TAB rather than a held key, so that holding TAB doesn't flip modes
//   over and over. Every frame we compare the current TAB state with this stored previous state.


// ============================================================================
// TIMING — values used to keep movement speed constant regardless of frame rate.
// ============================================================================

float deltaTime = 9.5f;                                                   // How much time (in seconds) elapsed between the last frame and this frame.
// "delta" is just a fancy word for "change/amount of difference". Storing this lets us scale camera movement by the
//   actual time that passed, so the camera moves at the same speed on fast and slow computers.

float lastFrame = 0.0f;                                                   // The timestamp (in seconds) of the PREVIOUS frame.
// We keep track of the previous frame's time so we can compute deltaTime = currentTime - lastFrame each frame.


// ============================================================================
// MATERIAL — a value describing how shiny our main cube's surface is.
// ============================================================================

float shininess = 32.0f;                                                  // The material's shininess, controlling how tight/bright the specular highlight is.
// A "material" describes the surface's visual properties. "shininess" specifically controls the glossiness:
//   big values (like 256) = a small, intense, mirror-like highlight; small values (like 1) = a big, dull smudge.
// We start at 32.0f and allow the user to change it with an ImGui slider (range 1 to 256). The 'f' makes it a float.


// ============================================================================
// LIGHT — the position of our light source in the 3D world.
// ============================================================================

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);                                    // The light's starting world-space position (x=1.2, y=1.0, z=2.0).
// "glm::vec3" = GLM's 3-number vector type. "lightPos" is our variable name ("light Position").
// The three numbers are x=1.2, y=1.0, z=2.0, the light's starting position in the world. The ImGui sliders in the
//   render loop let us change these x/y/z values live, which moves the light and updates the lighting in real time.


int main()                                                                    // This is the entry point of the program — where execution begins.
// "int" means the function eventually returns a whole number (we return 0 on success, a non-zero on failure).
// "main" is a special, RESERVED name. The operating system ALWAYS looks for a function named main() to start a C++
//   program. Everything the program does happens from inside this function.
{                                                                            // This opening curly brace marks the START of main()'s body (the block of code that runs).

    glfwInit();                                                              // Initializes the GLFW library so we can use GLFW functions.
    // Before we can create a window, GLFW needs to set up its internal state (detect the operating system, window
    //   system, etc.). glfwInit() does all that setup. It must be called before almost any other GLFW function.
    // (In a production app we'd check its return value for success, but that's omitted here for simplicity.)

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                         // Requests that the OpenGL context use major version 3.
    // "glfwWindowHint" sets a "hint" (a preference) that GLFW will honor when it creates the window.
    // The first argument "GLFW_CONTEXT_VERSION_MAJOR" refers to the major version number of OpenGL we want.
    // The second argument "3" is the actual value: major version = 3.

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                         // Requests that the OpenGL context use minor version 3.
    // Same as above but for the MINOR version: 3. Together, major 3 + minor 3 means OpenGL 3.3.
    // The "context" is the environment that holds all OpenGL state and lets us issue rendering commands.

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);          // Requests the modern OpenGL "Core Profile" (instead of the old compatibility profile).
    // OpenGL has two profiles: Core (modern, only supports current features) and Compatibility (legacy backward
    //   compatibility). We want Core, which is why the shaders all declare "#version 330 core". The constant
    //   GLFW_OPENGL_CORE_PROFILE tells GLFW which one to use.

#ifdef __APPLE__                                                             // A preprocessor directive: the following code only runs when compiling on Apple macOS.
    // "#ifdef" = "if defined". If the special macro __APPLE__ is defined (which it automatically is when compiling on
    //   a Mac), then the code below is compiled in. On other systems (Windows/Linux) this block is skipped entirely.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                   // On macOS, requests forward-compatible OpenGL behavior.
    // macOS only supports forward-compatible contexts; this hint (meaning "true") makes GLFW set that up so the
    //   context works reliably on Mac. GL_TRUE is just GLFW's way of writing the value "true".
#endif                                                                       // Marks the end of the #ifdef __APPLE__ conditional block.


    // ======================================================================
    // CREATE WINDOW — actually ask the OS to open a window.
    // ======================================================================

    GLFWwindow* window = glfwCreateWindow(                                  // Asks GLFW to create a new window (and its OpenGL context).
    // "GLFWwindow*" is a type — a POINTER to a GLFWwindow object. The '*' means "the address of". GLFW stores all the
    //   window's data somewhere in memory and gives us back its address, which we store in the variable named "window".
    // "glfwCreateWindow(...)" is the function that actually opens the window and returns that pointer.
        SCR_WIDTH,                                                           // Argument 1: the window's requested width (800, from our constant).
        SCR_HEIGHT,                                                          // Argument 2: the window's requested height (600, from our constant).
        "why am I doing that to myself",                                                         // Argument 3: the text shown in the window's title bar.
        nullptr,                                                             // Argument 4: monitor to use for fullscreen. "nullptr" means "no monitor", so we get a normal (non-fullscreen) window.
        nullptr                                                              // Argument 5: a window to share an GL context with. nullptr = no sharing, we get our own context.
    );                                                                       // This closing parenthesis + semicolon finishes the glfwCreateWindow(...) call.

    if (window == nullptr)                                                   // Checks whether glfwCreateWindow failed (returned "no window").
    // "if" is a conditional: the code inside the braces runs only if the condition is true. "nullptr" means "empty /
    //   no address". So if the window pointer is empty, the window creation failed.
    {                                                                        // Opening brace of the error-handling block.
        std::cout << "Failed to create GLFW window\n";                       // Prints an error message to the terminal.
        // "std::cout" is the standard "console output" stream; the "<<" pushes text into it to print.
        // The "\n" is a newline so the message appears on its own line. This tells the user what went wrong.
        glfwTerminate();                                                     // Cleans up/terminates GLFW before we exit.
        // Since we can't continue without a window, we shut down GLFW cleanly to free its resources.
        return -1;                                                           // Returns -1 from main(), telling the OS the program failed.
        // A non-zero return value signals failure to the operating system (0 would mean success). We stop here.
    }                                                                        // Closing brace of the error-handling block.

    glfwMakeContextCurrent(window);                                          // Makes THIS window's OpenGL context the "current" context for this thread.
    // OpenGL commands affect whatever context is marked "current". This line tells OpenGL: "from now on, all our
    //   rendering commands apply to this window's context." Without this, our OpenGL calls would have no target.


    // ======================================================================
    // CALLBACKS — telling GLFW which of OUR functions to call on certain events.
    // ======================================================================
    // A "callback" is a function pointer we give to GLFW. GLFW will CALL our function automatically whenever the
    //   corresponding event happens.

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);       // Registers our framebuffer-size callback with GLFW.
    // This tells GLFW: "whenever this window's drawing area changes size, call our framebuffer_size_callback function".
    //   We pass the window and the NAME of our function. This keeps our viewport matching the window size.

    glfwSetCursorPosCallback(window, mouse_callback);                        // Registers our mouse-movement callback.
    // Tells GLFW: "whenever the cursor moves over this window, call our mouse_callback function." This lets us rotate
    //   the camera with the mouse.

    glfwSetScrollCallback(window, scroll_callback);                          // Registers our mouse-scroll callback.
    // Tells GLFW: "whenever the mouse wheel scrolls, call our scroll_callback." This lets us zoom with the wheel.


    // ======================================================================
    // CAPTURE MOUSE — hide the cursor so only the camera gets mouse input.
    // ======================================================================

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);              // Captures and hides the cursor.
    // "glfwSetInputMode" changes how the mouse behaves. GLFW_CURSOR means "we're changing cursor behavior", and
    //   GLFW_CURSOR_DISABLED hides the cursor and locks it in place, so moving the mouse rotates the camera instead of
    //   moving an on-screen pointer. (TAB toggles this later so we can use the ImGui interface.)


    // ======================================================================
    // INITIALIZE GLAD — load all the OpenGL function pointers.
    // ======================================================================

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))                 // Load all OpenGL functions using GLFW as the source.
    // "gladLoadGLLoader" is GLAD's initialization function. It needs a way to look up OpenGL functions; we give it
    //   glfwGetProcAddress, which GLFW provides for retrieving function addresses from the driver.
    // "(GLADloadproc)" is a type cast that converts the function signature to what GLAD expects.
    // The "!" means NOT, so this block runs only if the loading FAILS (returns false).
    {                                                                        // Opening brace of the error block.
        std::cout << "Failed to initialize GLAD\n";                          // Prints an error message to the terminal.
        return -1;                                                           // Stops the program because we can't safely call OpenGL functions.
    }                                                                        // Closing brace of the error block.


    // ======================================================================
    // INITIALIZE IMGUI — set up the GUI library.
    // ======================================================================

    IMGUI_CHECKVERSION();                                                    // Verifies that the ImGui headers and compiled code are the same version.
    // ImGui is strict about version matching. This macro validates that the header (which our code was compiled
    //   against) and the compiled ImGui library are compatible, to prevent subtle crashes.

    ImGui::CreateContext();                                                   // Creates the main ImGui context (its memory/state).
    // ImGui needs a "context" to hold all of its state. This creates one. "::" means we're calling a function that
    //   belongs to the "ImGui" namespace (a way to group things).

    ImGui::StyleColorsDark();                                                 // Applies ImGui's built-in dark color theme.
    // Just sets the default look (colors, spacing) of the widgets so they render nicely out of the box.

    ImGui_ImplGlfw_InitForOpenGL(window, true);                              // Connects ImGui to our GLFW window and lets it receive input.
    // This wires the GLFW backend to our specific window. Passing "true" means ImGui should install its own callbacks
    //   to capture mouse/keyboard input. Now ImGui knows about our window.

    ImGui_ImplOpenGL3_Init("#version 330");                                  // Initializes ImGui's OpenGL 3 renderer with a GLSL version string.
    // This sets up the OpenGL backend so ImGui can draw. The string "#version 330" tells ImGui which GLSL version to
    //   use when building its internal shader for drawing the UI (must match our OpenGL 3.3 setup).


    // ======================================================================
    // OPENGL STATE — global things that affect all subsequent drawing.
    // ======================================================================

    glEnable(GL_DEPTH_TEST);                                                 // Enables depth testing.
    // By default OpenGL draws in the order you submit geometry, which means farther objects can draw ON TOP of nearer
    //   ones. Depth testing fixes this: it keeps a per-pixel "depth" buffer, and only draws a fragment if it's closer
    //   than what's already there. This makes the 3D world look correct (near objects hide far ones).


    // ======================================================================
    // LOAD SHADERS — read + compile our GLSL shaders into shader programs.
    // ======================================================================

    Shader lightingShader(                                                  // Creates a Shader object named "lightingShader" for our lit cube.
    // "Shader" is the class from shader_s.h. "lightingShader" is the name of this instance. It will manage the shader
    //   program that renders the main, properly-lit cube. The constructor reads, compiles, and links the shaders given.
        RESOURCES_PATH "1.colors.vs",                                        // Argument 1: the full path to our VERTEX shader source file.
        // "RESOURCES_PATH" is a macro (pre-set by the build system) that expands to the folder path where our resource
        //   files live. The compiler concatenates it with the string "1.colors.vs" into one complete path.
        RESOURCES_PATH "1.colors.fs"                                         // Argument 2: the full path to our FRAGMENT shader source file.
        // Same as above but for the fragment shader "1.colors.fs". These two files contain the GLSL code.
    );                                                                       // Finishes constructing lightingShader.

    Shader lightCubeShader(                                                  // Creates a second Shader object for the small light-placeholder cube.
        RESOURCES_PATH "1.light_cube.vs",                                   // Argument 1: the vertex shader for the light cube.
        RESOURCES_PATH "1.light_cube.fs"                                    // Argument 2: the fragment shader for the light cube.
        // These simpler shaders just draw a solid white cube at the light position.
    );                                                                       // Finishes constructing lightCubeShader.


    // ======================================================================
    // VERTEX DATA — the raw numbers describing the cube's shape.
    //
    // Every vertex (corner) of our cube is stored as 6 floats:
    //      3 floats = position (x, y, z)
    //      3 floats = normal   (nx, ny, nz, the direction the surface faces)
    // So each vertex takes 6 floats of storage, and the data is interleaved
    // (position then normal, repeating for every vertex).
    // ======================================================================

    float vertices[] =                                                       // Declares an array (a list) of floats holding ALL the cube's vertex data.
    // "float" is the type (decimal numbers), "vertices" is the name, and the [] means this is an array/list. The
    //   { } below lists every number. This is the raw geometry our cube is made from.
    {                                                                        // Opening brace of the array initializer list.
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 1 of the BACK face (normal points toward -z/backward).
        // First 3 numbers (-0.5,-0.5,-0.5) are the position. Second 3 (0,0,-1) are the normal: 0 sideways, 0 up, -1
        //   backward. Units are "world units"; the cube spans -0.5 to +0.5 on each axis, so it's 1 unit wide.
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 2 of the BACK face (same backward normal).
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 3 of the BACK face (same backward normal).
        // These 3 vertices form the first triangle of the back face. Next 3 vectors form the 2nd triangle of that face.
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 4 = same as vertex 3 (repeats a corner to form the 2nd triangle).
        // Reusing a corner is normal: each face is 2 triangles, so a face uses the same shared corners.
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 5 of the BACK face (top-left, backward normal).
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,                          // Vertex 6 of the BACK face (bottom-left, backward normal).

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,                         // Vertex 1 of the FRONT face (normal points toward +z/forward).
        // Note the normal here is (0,0,1) — pointing toward the viewer. Positions now have z = +0.5.
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,                         // Vertex 2 of the FRONT face (forward normal).
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,                         // Vertex 3 of the FRONT face (forward normal).
         0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,                            // Vertex 4 of the FRONT face (repeated corner for triangle 2).
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,                             // Vertex 5 of the FRONT face (forward normal).
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,                             // Vertex 6 of the FRONT face (forward normal).

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,                             // Vertex 1 of the LEFT face (normal points toward -x/left).
        // Positions all have x = -0.5; normals are (-1, 0, 0), pointing left/negative x.
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,                            // Vertex 2 of the LEFT face (left normal).
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,                           // Vertex 3 of the LEFT face (left normal).
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,                           // Vertex 4 of the LEFT face (repeated corner).
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,                            // Vertex 5 of the LEFT face (left normal).
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,                             // Vertex 6 of the LEFT face (left normal).

         0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,                              // Vertex 1 of the RIGHT face (normal points toward +x/right).
        // Positions all have x = +0.5; normals are (1, 0, 0), pointing right/positive x.
         0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,                             // Vertex 2 of the RIGHT face (right normal).
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,                            // Vertex 3 of the RIGHT face (right normal).
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,                            // Vertex 4 of the RIGHT face (repeated corner).
         0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,                             // Vertex 5 of the RIGHT face (right normal).
         0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,                              // Vertex 6 of the RIGHT face (right normal).

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,                           // Vertex 1 of the BOTTOM face (normal points toward -y/down).
        // Positions all have y = -0.5; normals are (0, -1, 0), pointing down/negative y.
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,                           // Vertex 2 of the BOTTOM face (down normal).
         0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,                            // Vertex 3 of the BOTTOM face (down normal).
         0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,                            // Vertex 4 of the BOTTOM face (repeated corner).
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,                            // Vertex 5 of the BOTTOM face (down normal).
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,                           // Vertex 6 of the BOTTOM face (down normal).

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,                             // Vertex 1 of the TOP face (normal points toward +y/up).
        // Positions all have y = +0.5; normals are (0, 1, 0), pointing up/positive y.
         0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,                             // Vertex 2 of the TOP face (up normal).
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,                              // Vertex 3 of the TOP face (up normal).
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,                              // Vertex 4 of the TOP face (repeated corner).
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,                             // Vertex 5 of the TOP face (up normal).
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f                             // Vertex 6 of the TOP face (up normal).
        // NOTE: no trailing comma after the last number (a comma after the final element isn't allowed).
    };                                                                       // Closing brace + semicolon ends the vertices array.


    // ======================================================================
    // CREATE CUBE VAO AND VBO — give OpenGL our vertex data in a usable form.
    // ======================================================================
    //   Two OpenGL concepts matter here:
    //   - A VBO (Vertex Buffer Object) is just a chunk of memory that lives on the GPU, holding our raw vertex numbers.
    //   - A VAO (Vertex Array Object) is a "recipe"/container that remembers HOW to interpret that buffer: which
    //     attribute is which, how many components each has, their stride, their offset. We bind the VAO and the right
    //     layout "comes along" automatically, so we don't have to re-specify it every frame.

    unsigned int VBO;                                                        // Declares an unsigned integer that will hold our VBO's ID number.
    // OpenGL refers to objects (buffers, arrays) by ID numbers, not pointers. "unsigned int" can hold that ID.
    // At this point VBO is just an uninitialized variable; glGenBuffers below gives it its actual ID.

    unsigned int cubeVAO;                                                    // Declares an unsigned integer that will hold our cube's VAO ID.
    // This will store the ID of our cube's Vertex Array Object after we generate it.

    glGenVertexArrays(1, &cubeVAO);                                          // Asks OpenGL to create ONE vertex array object and store its ID in cubeVAO.
    // "glGenVertexArrays(count, array)" generates the given number of VAOs. "1" = create one. "&cubeVAO" is the
    //   ADDRESS of our cubeVAO variable, so glGenVertexArrays writes the new ID directly into it.
    // (The & gives the memory address of cubeVAO so OpenGL's C API can modify the variable.)

    glGenBuffers(1, &VBO);                                                   // Asks OpenGL to create ONE vertex buffer object and store its ID in VBO.
    // Same idea, but for our VBO (the buffer that will hold the actual vertex data).

    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                      // Binds (activates) our VBO to the GL_ARRAY_BUFFER binding point.
    // OpenGL uses "binding points" as the current working slot. "glBindBuffer(gl_ARRAY_BUFFER, ...)" says:
    //   "the buffer I'm about to work with is the one bound to GL_ARRAY_BUFFER". After this, buffer operations affect
    //   our VBO until we bind something else. GL_ARRAY_BUFFER is the target for generic vertex data.

    glBufferData(                                                            // Allocates GPU memory and copies our vertex data into the bound VBO.
        GL_ARRAY_BUFFER,                                                     // Argument 1: which buffer target to upload to (the one we just bound).
        sizeof(vertices),                                                    // Argument 2: the total number of BYTES to allocate for this buffer.
        // "sizeof(vertices)" computes the byte size of our vertices array (number of elements x bytes per float).
        vertices,                                                            // Argument 3: a pointer to the actual data to copy into the GPU buffer.
        GL_STATIC_DRAW                                                       // Argument 4: a hint about how we'll use the data.
        // GL_STATIC_DRAW tells OpenGL "this data won't change much", so it can optimize storage/access for speed.
    );                                                                       // Closing parenthesis + semicolon finishes the glBufferData(...) call.

    glBindVertexArray(cubeVAO);                                              // Binds (activates) our cube VAO so attribute setup below is stored inside it.
    // From this point on, any vertex attribute configuration we do is remembered by the cubeVAO. This means later we
    //   can just "bind cubeVAO" and all these attribute definitions come back automatically.

    glVertexAttribPointer(                                                   // Defines how OpenGL should interpret the first data (the position) of each vertex.
        // This function tells OpenGL: "in the currently bound buffer, here's where each vertex's position starts,
        //   how it's laid out, etc." The settings we set now are recorded in the bound VAO.
        0,                                                                   // Argument 1: the attribute LOCATION this applies to (must match "location = 0" in the vertex shader).
        3,                                                                   // Argument 2: 3 components per position (x, y, z).
        GL_FLOAT,                                                            // Argument 3: the data type — each component is a 32-bit float.
        GL_FALSE,                                                            // Argument 4: whether to normalize the values. false = use them as-is (don't map to 0..1).
        6 * sizeof(float),                                                   // Argument 5: the "stride" — the number of bytes from the start of one vertex to the next.
        // Since each vertex is 6 floats (position 3 + normal 3), the next vertex starts 6 floats later. "6 * 4 bytes".
        (void*)0                                                             // Argument 6: the byte OFFSET where the position data starts inside each vertex (0 = at the very start).
    );                                                                       // Closing parenthesis + semicolon finishes the position attribute setup.

    glEnableVertexAttribArray(0);                                            // Enables attribute location 0 so OpenGL actually reads position data.
    // By default, attribute arrays are disabled. We must explicitly turn this one on, otherwise the data at location
    //   0 wouldn't be passed to the shader even though we configured the pointer.

    glVertexAttribPointer(                                                   // Defines how OpenGL should interpret the SECOND part (the normal) of each vertex.
        1,                                                                   // Argument 1: attribute location 1 (matches "location = 1" in the vertex shader).
        3,                                                                   // Argument 2: 3 components per normal (nx, ny, nz).
        GL_FLOAT,                                                            // Argument 3: data type is float.
        GL_FALSE,                                                            // Argument 4: don't normalize.
        6 * sizeof(float),                                                   // Argument 5: same stride — each vertex is still 6 floats total.
        (void*)(3 * sizeof(float))                                           // Argument 6: the OFFSET IN BYTES where the normal begins inside each vertex.
        // The position occupies the first 3 floats, so the normal starts at byte (3 floats x 4 bytes) = 12 bytes in.
    );                                                                       // Closing parenthesis + semicolon finishes the normal attribute setup.

    glEnableVertexAttribArray(1);                                            // Enables attribute location 1 so OpenGL passes the normal data to the shader.
    // Same as before, but for location 1 (the normals), turning on the reading of that attribute.


    // ======================================================================
    // LIGHT CUBE VAO — set up a second VAO to draw the small light-marker cube.
    // ======================================================================
    //   We REUSE the same vertex data (same VBO) because the light cube is the same shape, just at a different spot.
    //   The light cube only needs POSITIONS (no normals), so we only configure attribute location 0.

    unsigned int lightCubeVAO;                                               // Declares an unsigned int to hold the light cube's VAO ID.
    glGenVertexArrays(1, &lightCubeVAO);                                    // Generates one VAO and stores its new ID in lightCubeVAO.
    glBindVertexArray(lightCubeVAO);                                        // Binds/makes the light cube VAO active so we can configure it.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                      // Re-uses the same VBO (which still holds the cube's position data).
    // We don't need to re-upload data; the positions are already in VBO from earlier. Binding tells OpenGL which
    //   buffer the attribute pointer below will refer to.

    glVertexAttribPointer(                                                   // Configures the light cube's position attribute.
        0,                                                                   // Attribute location 0 (matches its vertex shader, which only declares aPos at location 0).
        3,                                                                   // 3 components per position.
        GL_FLOAT,                                                            // Float data.
        GL_FALSE,                                                            // Don't normalize.
        6 * sizeof(float),                                                   // Stride: each original vertex is still 6 floats (even though the light cube ignores the normal).
        (void*)0                                                             // Offset: position starts at the very beginning of each vertex.
    );                                                                       // Finishes the light cube's position attribute setup.
    glEnableVertexAttribArray(0);                                            // Enables attribute location 0 so the light cube gets its positions.


    // ======================================================================
    // RENDER LOOP — the heart of the program; runs over and over until the window closes.
    // ======================================================================

    while (!glfwWindowShouldClose(window))                                  // Keeps looping as long as the window should stay open.
    // "while" is a loop: the block inside repeats as long as the condition is true.
    // "glfwWindowShouldClose(window)" returns true when the user asked to close (X button, ESC, etc.).
    // The "!" negates it, so we loop while the window should NOT close — i.e., the whole time it's open.
    // Each pass through this loop is called one "frame". Typical games do this 60+ times per second.
    {                                                                        // Opening brace of the render-loop body.


        // --------------------------------------------------------------
        // TIMING — compute how much time passed this frame (for smooth movement).
        // --------------------------------------------------------------

        float currentFrame =                                                  // Begins computing the current time.
            // "currentFrame" will store the current time in seconds. It's used to compute deltaTime.
            static_cast<float>(glfwGetTime());                               // Gets seconds since GLFW started and converts it to a float.
            // "glfwGetTime()" returns the number of seconds since glfwInit(), as a double (double precision).
            // "static_cast<float>(...)" is a safe C++ way to convert that double into a float. We use floats to match
            //   our other time variables.
        // (The line assigns whatever glfwGetTime() returned into currentFrame.)

        deltaTime =                                                           // Computes how much time passed since the previous frame.
            currentFrame - lastFrame;                                        // Subtracts the previous frame's timestamp from this frame's timestamp.
        // If the current time is 2.0 and the last frame was 1.8, deltaTime = 0.2 (0.2 seconds passed between frames).
        // Multiplying movement by deltaTime makes it consistent regardless of whether frames come fast or slow.

        lastFrame = currentFrame;                                            // Stores this frame's time so it can be used as "previous" next frame.
        // Next time through the loop, "lastFrame" will be the time we just recorded here, enabling the subtraction.

        processInput(window);                                                // Checks keyboard input and updates the camera / mouse mode accordingly.
        // We call our own processInput function, passing the window so it can query which keys are pressed.


        // --------------------------------------------------------------
        // CLEAR THE SCREEN — wipe the color and depth buffers for a fresh frame.
        // --------------------------------------------------------------

        glClearColor(                                                        // Specifies the color used when clearing the color buffer.
            0.1f,                                                            // Red component of the clear color (very dark).
            0.1f,                                                            // Green component (very dark).
            0.1f,                                                            // Blue component (very dark).
            1.0f                                                             // Alpha (fully opaque; the background is solid).
        );                                                                   // Finishes specifying the clear color.
        // (0.1, 0.1, 0.1) produces a dark gray background. This is the "empty" color you see behind the cube.

        glClear(                                                             // Clears the specified buffers.
            GL_COLOR_BUFFER_BIT |                                            // Clear the color buffer (the picture) with our clear color.
            // "|" is the bitwise OR operator — it lets us clear MULTIPLE buffers in one call by combining flags.
            GL_DEPTH_BUFFER_BIT                                              // Also clear the depth buffer so depth testing starts fresh this frame.
            // If we didn't clear depth, leftover depth from last frame would hide new geometry incorrectly.
        );                                                                   // Finishes the clear operation.


        // --------------------------------------------------------------
        // START THE IMGUI FRAME — begin building this frame's GUI.
        // --------------------------------------------------------------

        ImGui_ImplOpenGL3_NewFrame();                                        // Tells ImGui's OpenGL backend that a new GUI frame is starting.
        // ImGui works in "frames" like our render loop. This is step 1 of 3 to prepare a GUI frame.
        ImGui_ImplGlfw_NewFrame();                                           // Tells ImGui's GLFW backend to collect the latest input/window state.
        // Step 2: ImGui grabs the mouse/keyboard state from GLFW so it can drive the widgets.
        ImGui::NewFrame();                                                   // Step 3: begins building the ImGui frame's UI content.
        // After this, any ImGui widgets we create belong to this new frame.


        // --------------------------------------------------------------
        // IMGUI WINDOW — the "Light" control panel on screen.
        // --------------------------------------------------------------

        ImGui::Begin("Light");                                               // Opens (starts) an ImGui window titled "Light".
        // Everything between Begin and the matching End belongs to a movable/resizable window named "Light".
        // The "::" means Begin is a function in the ImGui namespace.

        ImGui::Text("Move the light around!");                               // Displays a simple text label inside the window.
        // Just prints a line of instruction text to guide the user.

        ImGui::Separator();                                                  // Draws a horizontal line to visually separate sections.
        // A decorative divider to organize the UI.

        ImGui::SliderFloat(                                                  // Creates a slider widget that edits a float value.
            "X",                                                             // Argument 1: the label shown next to the slider.
            &lightPos.x,                                                     // Argument 2: a POINTER (&) to the variable ImGui should modify — the light's x coordinate.
            // "&lightPos.x" passes the address of the x component, so moving the slider directly changes lightPos.x.
            -5.0f,                                                           // Argument 3: the minimum value the slider allows.
            5.0f                                                             // Argument 4: the maximum value the slider allows.
        );                                                                   // Finishes the X slider.

        ImGui::SliderFloat(                                                  // Creates the Y slider.
            "Y",                                                             // Label.
            &lightPos.y,                                                     // Link to the light's y coordinate.
            -5.0f,                                                           // Min.
            5.0f                                                             // Max.
        );                                                                   // Finishes the Y slider.

        ImGui::SliderFloat(                                                  // Creates the Z slider.
            "Z",                                                             // Label.
            &lightPos.z,                                                     // Link to the light's z coordinate.
            -5.0f,                                                           // Min.
            5.0f                                                             // Max.
        );                                                                   // Finishes the Z slider.

        ImGui::Separator();                                                  // Another horizontal divider before the next section.

        ImGui::Text(                                                         // Displays formatted text (like printf) showing the current light position.
            "Position: %.2f, %.2f, %.2f",                                    // The format string; %.2f means "show each number with 2 decimal places".
            lightPos.x,                                                      // Value to substitute into the first %.2f.
            lightPos.y,                                                      // Value for the second %.2f.
            lightPos.z                                                       // Value for the third %.2f.
        );                                                                   // Finishes the position text.

        if (ImGui::Button("Reset Light"))                                    // Creates a button; the block runs only when it is clicked.
        // "ImGui::Button" returns true for exactly one frame after the user clicks it, which triggers the code below.
        {                                                                    // Opening brace of the reset-button block.
            lightPos = glm::vec3(                                             // Sets the light position back to its starting value.
                1.2f,                                                         // x = 1.2 (the original value).
                1.0f,                                                         // y = 1.0 (the original value).
                2.0f                                                          // z = 2.0 (the original value).
            );                                                               // Finishes constructing and assigning the new vector.
        }                                                                    // Closing brace of the reset-button block.

        ImGui::Separator();                                                  // Divider before the material controls.

        ImGui::Text("Material");                                             // A heading label for the following material section.

        ImGui::SliderFloat(                                                  // Creates a slider for the material's shininess.
            "Shininess",                                                     // Label.
            &shininess,                                                      // Link to our shininess variable (ImGui edits it directly).
            1.0f,                                                            // Minimum allowed value.
            256.0f                                                           // Maximum allowed value.
        );                                                                   // Finishes the shininess slider.

        ImGui::Separator();                                                  // Divider before the help text.

        ImGui::Text("Press TAB to toggle mouse capture.");                   // Displays a hint about how to switch modes.

        ImGui::End();                                                        // Closes the "Light" window we opened with Begin.
        // This must come after all the widgets for this window. It ends the ImGui frame content.



        // ==================================================================
        // USE LIGHTING SHADER — activate the shader that renders the lit cube.
        // ==================================================================

        lightingShader.use();                                                // Makes the lightingShader program the "active" shader program.
        // "use()" (provided by our Shader class) calls glUseProgram behind the scenes. The active shader is what OpenGL
        //   uses for any subsequent draw calls, and it's the only program whose uniforms we can set until we switch.

        // Set the "viewPos" uniform: tell the fragment shader where the camera is.
        lightingShader.setVec3(                                              // Calls the Shader class method to upload a vec3 uniform.
            "viewPos",                                                       // Argument 1: the exact NAME of the uniform in the GLSL fragment shader.
            camera.Position                                                  // Argument 2: the camera's actual world position (a glm::vec3).
        );                                                                   // Finishes uploading viewPos.
        // The fragment shader uses viewPos to compute the "direction to the camera" for the specular highlight.


        // ==================================================================
        // SEND LIGHT POSITION — upload where the light is.
        // ==================================================================

        lightingShader.setVec3(                                              // Uploads the light's position to the shader.
            "light.position",                                                // Argument 1: the uniform name. "light" is the struct, ".position" is its member.
            lightPos                                                         // Argument 2: the glm::vec3 holding the light's world position.
        );                                                                   // Finishes uploading the light position.
        // This connects our C++ lightPos variable (possibly moved by the sliders) to the GLSL Light struct.


        // ==================================================================
        // SEND MATERIAL — upload the cube's material properties.
        // ==================================================================

        lightingShader.setVec3(                                              // Uploads the material's ambient color.
            "material.ambient",                                              // The uniform name: "material" struct, ".ambient" member.
            1.0f,                                                            // Red component (full).
            0.5f,                                                            // Green component (half).
            0.31f                                                            // Blue component (about a third).
        );                                                                   // Finishes setting material.ambient.
        // This is the color the cube reflects under ambient/base light (an orange-ish tint here).

        lightingShader.setVec3(                                              // Uploads the material's diffuse (main) color.
            "material.diffuse",                                              // The uniform name.
            1.0f,                                                            // Red.
            0.5f,                                                            // Green.
            0.31f                                                            // Blue.
        );                                                                   // Finishes setting material.diffuse.
        // The primary lit color of the cube (same orange tint).

        lightingShader.setVec3(                                              // Uploads the material's specular color.
            "material.specular",                                             // The uniform name.
            1.0f,                                                            // Red.
            1.0f,                                                            // Green.
            1.0f                                                             // Blue.
        );                                                                   // Finishes setting material.specular.
        // (1,1,1) = white, so the glossy highlight will appear white rather than tinted.

        lightingShader.setFloat(                                             // Uploads a single float value to the shader.
            "material.shininess",                                            // The uniform name (a float in the material struct).
            shininess                                                        // The shininess value currently chosen by the ImGui slider.
        );                                                                   // Finishes setting material.shininess.
        // This makes the highlight tighter/brighter as the user drags the "Shininess" slider.


        // ==================================================================
        // ANIMATED LIGHT COLOR — create a color that changes over time.
        // ==================================================================

        float time =                                                           // Declares a local float that stores the current time.
            static_cast<float>(glfwGetTime());                               // Gets seconds since start and converts double -> float.
        // We need "time" in this frame to animate the light color via sine waves below.

        glm::vec3 lightColor(                                                // Creates a glm::vec3 holding an animated RGB color.
            sin(time * 2.0f) * 0.5f + 0.5f,                                 // Red component.
            // "sin(...)" is the mathematical sine wave, which swings between -1 and 1 over time.
            // "time * 2.0f" makes the wave cycle faster. "sin(...)*0.5f+0.5f" remaps the -1..1 range to 0..1, which
            //   is the valid range for a color channel (no negatives). So red drifts smoothly between 0 and 1.
            sin(time * 0.7f) * 0.5f + 0.5f,                                 // Green component.
            // Same trick but with a slower speed factor (0.7), so green changes at a different rate.
            sin(time * 1.3f) * 0.5f + 0.5f                                  // Blue component.
            // A different speed (1.3) so blue drifts at yet another rate, giving a continuously shifting color.
        );                                                                   // Finishes constructing lightColor.


        // ==================================================================
        // DIFFUSE LIGHT COLOR — copy the animated color for direct lighting.
        // ==================================================================

        glm::vec3 diffuseColor =                                             // Declares diffuseColor.
            lightColor;                                                      // Copies the animated lightColor into it.
        // We make a copy so the diffuse light color is exactly the animated color we computed.


        // ==================================================================
        // AMBIENT LIGHT COLOR — make a dimmer version for base lighting.
        // ==================================================================

        glm::vec3 ambientColor =                                             // Declares ambientColor.
            diffuseColor * glm::vec3(0.3f);                                 // Scales each RGB channel to 30% of diffuse.
        // Multiplying a vec3 by another vec3 multiplies component-by-component. glm::vec3(0.3f) fills all three channels
        //   with 0.3, so ambient ends up 30% as bright as diffuse — a faint base light so the cube is never pure black.


        // ==================================================================
        // SEND LIGHT COLORS TO THE SHADER — upload the three light color components.
        // ==================================================================

        lightingShader.setVec3(                                              // Uploads the ambient light color.
            "light.ambient",                                                 // Uniform name: light struct, .ambient member.
            ambientColor                                                     // The dimmed color we computed.
        );                                                                   // Finishes setting light.ambient.

        lightingShader.setVec3(                                              // Uploads the diffuse light color.
            "light.diffuse",                                                 // Uniform name.
            diffuseColor                                                     // The animated color.
        );                                                                   // Finishes setting light.diffuse.

        lightingShader.setVec3(                                              // Uploads the specular light color.
            "light.specular",                                                // Uniform name.
            1.0f,                                                            // Red.
            1.0f,                                                            // Green.
            1.0f                                                             // Blue.
        );                                                                   // Finishes setting light.specular.
        // White specular light keeps the highlight neutral/white.


        // ==================================================================
        // PROJECTION MATRIX — the "camera lens" that adds perspective.
        // ==================================================================

        glm::mat4 projection =                                               // Declares a 4x4 matrix called "projection".
            glm::perspective(                                                // Builds a perspective projection matrix.
                glm::radians(camera.Zoom),                                    // Argument 1: field of view, converted from degrees to radians.
                // "camera.Zoom" is our zoom (like 45.0 by default). glfw's scroll callback changes it to zoom in/out.
                // "glm::radians(...)" converts degrees -> radians because the math functions need radians.
                (float)SCR_WIDTH / (float)SCR_HEIGHT,                       // Argument 2: the aspect ratio (width / height).
                // Converting constants to float prevents integer division (which would give 800/600 = 1, not 1.33...).
                0.1f,                                                        // Argument 3: the NEAR clipping plane distance.
                // Anything closer than 0.1 units to the camera is not drawn.
                100.0f                                                       // Argument 4: the FAR clipping plane distance.
                // Anything farther than 100 units is not drawn. Objects between 0.1 and 100 are visible.
            );                                                               // Finishes the perspective() call.
        // This matrix is what makes far-away things look smaller (perspective). It produces clip space coordinates.


        // ==================================================================
        // VIEW MATRIX — represents the camera's position and orientation.
        // ==================================================================

        glm::mat4 view =                                                     // Declares a 4x4 matrix called "view".
            camera.GetViewMatrix();                                         // Asks the Camera object to build the view matrix.
        // The Camera class computes a matrix using its position and where it's looking. This makes world space appear
        //   relative to the camera (moving the camera = looking around the scene).


        // Upload the "projection" uniform to the lighting shader.
        lightingShader.setMat4(                                              // Calls the Shader class method to upload a matrix uniform.
            "projection",                                                    // Argument 1: the uniform name in the vertex shader.
            projection                                                       // Argument 2: the glm::mat4 we just built.
        );                                                                   // Finishes uploading the projection matrix.

        // Upload the "view" uniform to the lighting shader.
        lightingShader.setMat4(                                              // Uploads the view matrix.
            "view",                                                          // Uniform name.
            view                                                             // The glm::mat4 from the camera.
        );                                                                   // Finishes uploading the view matrix.


        // ==================================================================
        // MODEL MATRIX — describes the cube's position/rotation/scale in the world.
        // ==================================================================

        glm::mat4 model =                                                    // Declares a 4x4 matrix called "model".
            glm::mat4(1.0f);                                                 // Creates an IDENTITY matrix (all zeros except 1s on the diagonal).
        // The identity matrix does nothing: multiplying by it leaves coordinates unchanged. Starting from identity means
        //   our main cube is NOT moved, rotated, or scaled — it stays exactly where its vertex data says (at the origin).

        // Upload the "model" uniform to the lighting shader.
        lightingShader.setMat4(                                              // Uploads the model matrix.
            "model",                                                         // Uniform name.
            model                                                            // The identity matrix.
        );                                                                   // Finishes uploading the model matrix.


        // ==================================================================
        // DRAW MAIN CUBE — issue the draw call that renders the lit cube.
        // ==================================================================

        glBindVertexArray(cubeVAO);                                          // Binds the cube's VAO so OpenGL uses its vertex data + layout.
        // This reactivates the cube's attribute setup (positions + normals) we configured earlier.

        glDrawArrays(                                                        // Tells OpenGL to draw triangles using the bound VAO.
            GL_TRIANGLES,                                                    // Argument 1: the draw mode — interpret every 3 vertices as one triangle.
            0,                                                               // Argument 2: the starting index into the vertex data (begin at the first vertex).
            36                                                               // Argument 3: how many vertices to use (36 = 6 faces x 6 vertices).
        );                                                                   // Finishes the cube draw call. This actually draws the lit cube on screen.


        // ==================================================================
        // USE LIGHT CUBE SHADER — switch to the simpler shader for the light marker.
        // ==================================================================

        lightCubeShader.use();                                               // Activates the light cube's shader program.
        // Any draw calls from now on (until we switch again) use this simpler white shader.

        // Upload the projection matrix to the light cube shader.
        lightCubeShader.setMat4(                                             // Sets the projection uniform.
            "projection",                                                    // Uniform name.
            projection                                                       // Reuses the same projection matrix as the main cube.
        );                                                                   // Finishes uploading.

        // Upload the view matrix to the light cube shader.
        lightCubeShader.setMat4(                                             // Sets the view uniform.
            "view",                                                          // Uniform name.
            view                                                             // Reuses the same view matrix (same camera).
        );                                                                   // Finishes uploading.

        model = glm::mat4(1.0f);                                             // Resets model back to the identity matrix before building the light cube's transform.
        // We reuse the "model" variable but start fresh so the light cube's transform is independent of anything else.

        model = glm::translate(                                               // Applies a TRANSLATION (movement) to the model matrix.
            model,                                                            // Argument 1: the matrix to start from (our fresh identity matrix).
            lightPos                                                         // Argument 2: how far to move, in world units, = the light's position.
        );                                                                   // Finishes the translation. Now the model matrix moves things to the light position.

        model = glm::scale(                                                  // Applies a SCALE (resize) to the model matrix.
            model,                                                            // Argument 1: the matrix to start from (the translated matrix).
            glm::vec3(0.2f)                                                  // Argument 2: scale factor — 0.2 in all three axes.
        );                                                                   // Finishes scaling. glm::vec3(0.2f) fills all components with 0.2 so it shrinks to 20% size.

        lightCubeShader.setMat4(                                             // Uploads the built model matrix to the light cube shader.
            "model",                                                         // Uniform name.
            model                                                            // The translated+scaled matrix.
        );                                                                   // Finishes uploading.

        glBindVertexArray(lightCubeVAO);                                     // Binds the light cube's VAO.
        // This VAO reads only the positions from our VBO (no normals), which is all this simple shader needs.

        glDrawArrays(                                                        // Draws the light cube.
            GL_TRIANGLES,                                                    // Draw as triangles.
            0,                                                               // Start at the first vertex.
            36                                                               // Use all 36 vertices (same cube geometry).
        );                                                                   // Finishes drawing the small white light cube.


        // ==================================================================
        // RENDER IMGUI — draw the GUI on top of the 3D scene.
        // ==================================================================

        ImGui::Render();                                                     // Finalizes/implements the ImGui frame, producing draw data.
        // Prepares all the GUI widgets we made into GPU draw commands.

        ImGui_ImplOpenGL3_RenderDrawData(                                    // Sends ImGui's draw commands to the GPU to render them.
            ImGui::GetDrawData()                                             // Retrieves the finalized draw data produced by ImGui::Render().
        );                                                                   // Finishes rendering the GUI.


        // ==================================================================
        // PRESENT FRAME — show what we just drew and process events.
        // ==================================================================

        glfwSwapBuffers(window);                                             // Swaps the "back buffer" with the "front buffer".
        // OpenGL draws into an invisible "back buffer" while the "front buffer" is what's on screen. Swapping them
        //   instantly displays the new frame and prevents flickering. This is how double-buffering works.

        glfwPollEvents();                                                    // Checks for and processes pending events (input, window, etc.).
        // This is what lets GLFW notice key presses, mouse movement, scrolling, and window resizing, and invoke our
        //   callbacks. It does NOT wait — it just handles whatever events already happened.

        // End of the loop body.
    }                                                                         // This closing brace ends the render loop; control returns to the while(...) check at the top.


    // ======================================================================
    // CLEANUP OPENGL RESOURCES — delete our GPU objects now that we're done.
    // ======================================================================

    glDeleteVertexArrays(1, &cubeVAO);                                       // Deletes the cube's VAO and frees its GPU memory.
    // We're done rendering, so we politely release the GPU resources to avoid memory leaks. "1" = delete one VAO.

    glDeleteVertexArrays(1, &lightCubeVAO);                                  // Deletes the light cube's VAO.
    glDeleteBuffers(1, &VBO);                                                // Deletes the vertex buffer object (the raw vertex data on the GPU).
    // (The Shader objects are cleaned up automatically by their class destructors.)


    // ======================================================================
    // SHUTDOWN IMGUI — release the GUI library's resources.
    // ======================================================================

    ImGui_ImplOpenGL3_Shutdown();                                            // Shuts down ImGui's OpenGL renderer backend.
    ImGui_ImplGlfw_Shutdown();                                               // Shuts down ImGui's GLFW input backend.
    ImGui::DestroyContext();                                                 // Destroys/frees the main ImGui context and its memory.
    // These three calls are bookkeeping to release everything ImGui allocated, in the reverse order we set it up.


    // ======================================================================
    // SHUTDOWN GLFW — release GLFW and clean up.
    // ======================================================================

    glfwTerminate();                                                         // Terminates GLFW and cleans up its resources/window.
    return 0;                                                                // Returns 0 from main, telling the OS the program ran successfully.
    // By convention, returning 0 from main() means success; a non-zero value means an error occurred.
}                                                                            // This closing brace marks the END of main().




// ======================================================================
// KEYBOARD INPUT — our function that processes the keyboard every frame.
// ======================================================================
// Note: "void processInput(GLFWwindow* window)" was declared up near the top of the file; here we provide its body.

void processInput(GLFWwindow* window)                                        // Defines the function; it takes a pointer to our GLFW window.
{                                                                            // Opening the function body.
    // ---------------------------------------------------------------
    // ESCAPE KEY — close the application when Escape is pressed.
    // ---------------------------------------------------------------

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)                   // Checks whether the Escape key is currently held down.
    // "glfwGetKey(window, key)" asks GLFW whether that specific key is pressed. GLFW_PRESS is a constant meaning "down".
    // So this condition is true while the user holds Escape.
    {                                                                        // Opening the Escape-key block.
        glfwSetWindowShouldClose(window, true);                              // Requests that the window close.
        // This sets a flag that makes glfwWindowShouldClose() return true, which ends our render loop (window closes).
    }                                                                        // Closing the Escape-key block.

    // ---------------------------------------------------------------
    // TAB KEY — toggle between camera mode and ImGui/editor mode.
    // ---------------------------------------------------------------

    bool tabPressed =                                                         // Declares a boolean for the current TAB state.
        glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;                      // true while TAB is currently pressed.
    // We store the raw state so we can compare it with last frame's state (tabWasPressed).

    if (tabPressed && !tabWasPressed)                                        // Runs only on the "fresh press" of TAB (not when held).
    // "&&" means AND — both must be true. "!tabWasPressed" means TAB was NOT pressed last frame.
    // Together: TAB is down now AND was up last frame = a brand-new press. This avoids toggling repeatedly when held.
    {                                                                        // Opening the TAB-toggle block.
        mouseCaptured = !mouseCaptured;                                      // Flips the mouseCaptured boolean (true<->false).
        // "!" means NOT, so we invert it: if it was true (camera control) it becomes false (ImGui mode), and vice versa.

        if (mouseCaptured)                                                   // Checks whether we just switched TO camera mode.
        {                                                                    // Opening camera-mode block.
            glfwSetInputMode(                                                 // Changes the cursor behavior.
                window,                                                       // Argument 1: which window.
                GLFW_CURSOR,                                                  // Argument 2: we're changing the cursor setting.
                GLFW_CURSOR_DISABLED                                          // Argument 3: hide & capture the cursor for camera control.
            );                                                               // Finishes the input-mode change.
            firstMouse = true;                                                // Reset the "first mouse" flag.
            // This forces the next mouse movement to be treated as the starting position, preventing a camera jump.
        }                                                                    // Closing camera-mode block.
        else                                                                 // Otherwise we just switched TO ImGui/editor mode.
        {                                                                    // Opening editor-mode block.
            glfwSetInputMode(                                                 // Changes the cursor behavior back.
                window,                                                       // Which window.
                GLFW_CURSOR,                                                  // The cursor setting.
                GLFW_CURSOR_NORMAL                                             // Make the cursor visible and usable by ImGui.
            );                                                               // Finishes the input-mode change.
        }                                                                    // Closing editor-mode block.
    }                                                                        // Closing the TAB-toggle block.

    tabWasPressed = tabPressed;                                               // Store the current TAB state for the next frame.
    // Next frame, "tabWasPressed" holds whether TAB was pressed in THIS frame, letting us detect the next fresh press.

    // ---------------------------------------------------------------
    // WASD KEYS — move the camera around the scene.
    // ---------------------------------------------------------------

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)                        // If W is pressed...
    {                                                                        // Open the W block.
        camera.ProcessKeyboard(                                              // Tells the camera to move.
            FORWARD,                                                          // Direction: move forward (the way the camera faces).
            deltaTime                                                         // Amount: scaled by elapsed time for consistent speed.
        );                                                                   // Finish the movement call.
    }                                                                        // Close the W block.

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)                        // If S is pressed...
    {                                                                        // Open the S block.
        camera.ProcessKeyboard(                                              // Move the camera.
            BACKWARD,                                                         // Backward.
            deltaTime                                                         // Scaled by time.
        );                                                                   // Finish the call.
    }                                                                        // Close the S block.

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)                        // If A is pressed...
    {                                                                        // Open the A block.
        camera.ProcessKeyboard(                                              // Move the camera.
            LEFT,                                                             // Left (strafe).
            deltaTime                                                         // Scaled by time.
        );                                                                   // Finish the call.
    }                                                                        // Close the A block.

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)                        // If D is pressed...
    {                                                                        // Open the D block.
        camera.ProcessKeyboard(                                              // Move the camera.
            RIGHT,                                                            // Right (strafe).
            deltaTime                                                         // Scaled by time.
        );                                                                   // Finish the call.
    }                                                                        // Close the D block.
}                                                                            // Closing brace of processInput.




// ======================================================================
// WINDOW RESIZE CALLBACK — keeps the viewport matching the window size.
// ======================================================================

void framebuffer_size_callback(                                             // Definition of the callback GLFW calls on resize.
    GLFWwindow* window,                                                       // Parameter 1: the window that resized.
    int width,                                                                // Parameter 2: the new framebuffer width.
    int height                                                                // Parameter 3: the new framebuffer height.
)                                                                            // Ends the parameter list.
{                                                                            // Opens the function body.
    glViewport(                                                              // Tells OpenGL what region of the window to render into.
        0,                                                                    // x of the viewport's bottom-left corner.
        0,                                                                    // y of the viewport's bottom-left corner.
        width,                                                                // viewport width (= new window width).
        height                                                                // viewport height (= new window height).
    );                                                                       // Finishes updating the viewport.
    // Without this, a resized window would render into a region that no longer matches its new size (image would be
    //   stretched or letterboxed). (Note: "window" is unused here, which is fine.)
}                                                                            // Closing brace of the callback.




// ======================================================================
// MOUSE CALLBACK — turns mouse movement into camera rotation.
// ======================================================================

void mouse_callback(                                                          // Definition of the callback GLFW calls on mouse movement.
    GLFWwindow* window,                                                       // Parameter 1: the window with the event.
    double xposIn,                                                            // Parameter 2: the mouse's current x position (a double).
    double yposIn                                                             // Parameter 3: the mouse's current y position (a double).
)                                                                            // Ends the parameter list.
{                                                                            // Opens the function body.

    if (!mouseCaptured)                                                       // If the mouse is NOT captured (i.e., ImGui mode)...
    // "!mouseCaptured" is true when mouseCaptured is false, meaning the cursor is controlling the GUI.
    {                                                                        // Opens the early-return block.
        return;                                                               // Immediately exit this function.
        // We don't want to rotate the camera while the user is interacting with the ImGui panel, so we do nothing.
    }                                                                        // Closes the early-return block.

    float xpos =                                                              // Declares a float for the x position.
        static_cast<float>(xposIn);                                          // Converts the incoming double to a float.
    // "static_cast<float>(...)" is the safe, explicit C++ way to convert a double to a float, so numbers match.

    float ypos =                                                              // Declares a float for the y position.
        static_cast<float>(yposIn);                                          // Converts to float.
    // (GLFW gives us doubles, but our camera math expects floats, so we convert.)

    if (firstMouse)                                                          // If this is the first mouse event after capture...
    {                                                                        // Opens the first-mouse block.
        lastX = xpos;                                                         // Adopt the current x as the "previous" x.
        lastY = ypos;                                                         // Adopt the current y as the "previous" y.
        firstMouse = false;                                                   // Record that the initial position is now established.
        // This prevents a huge initial camera jump caused by the pointer starting somewhere unexpected.
    }                                                                        // Closes the first-mouse block.

    float xoffset =                                                           // Declares a float for horizontal movement.
        xpos - lastX;                                                         // Difference between the current and previous x positions.
    // A positive offset means the mouse moved right; negative means left.

    float yoffset =                                                           // Declares a float for vertical movement.
        lastY - ypos;                                                         // Difference between previous and current y (note the order).
    // We compute lastY - ypos (instead of ypos - lastY) because screen coordinates grow DOWNWARD, while we want camera
    //   pitch to feel natural (moving the mouse up should look up). This reverses the sign to correct for that.

    lastX = xpos;                                                            // Store the current x as "previous" for the next event.
    lastY = ypos;                                                            // Store the current y as "previous" for the next event.

    camera.ProcessMouseMovement(                                              // Sends the mouse movement to the camera so it rotates.
        xoffset,                                                             // Argument 1: how much the mouse moved horizontally.
        yoffset                                                              // Argument 2: how much it moved vertically.
    );                                                                       // Finishes updating the camera orientation.
    // The Camera class turns these offsets into yaw (left/right look) and pitch (up/down look) changes.
}                                                                            // Closing brace of the mouse callback.
// (The "window" parameter is not used in this callback, which is fine — it's required by GLFW's signature.)




// ======================================================================
// MOUSE SCROLL CALLBACK — zooms the camera in/out with the mouse wheel.
// ======================================================================

void scroll_callback(                                                         // Definition of the callback GLFW calls on scroll.
    GLFWwindow* window,                                                       // Parameter 1: the window with the event.
    double xoffset,                                                           // Parameter 2: horizontal scroll amount (unused here).
    double yoffset                                                            // Parameter 3: vertical scroll amount = our zoom control.
)                                                                            // Ends the parameter list.
{                                                                            // Opens the function body.
    camera.ProcessMouseScroll(                                                // Sends the scroll amount to the camera for zooming.
        static_cast<float>(yoffset)                                           // Converts the vertical wheel movement (double) to a float.
    );                                                                       // Finishes zooming.
    // The Camera class adjusts its zoom (field of view) based on this, which changes how much of the scene you see.
}                                                                            // Closing brace of the scroll callback.
