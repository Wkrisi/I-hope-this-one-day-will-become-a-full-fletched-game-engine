#version 330 core
// ^ Version directive: GLSL 3.30, Core Profile. This MUST be the first line of the file.
//
//   WHAT THIS SHADER IS FOR: This is the FRAGMENT shader for our main lit cube. It runs once for every single pixel of
//   the cube and decides the final color of that pixel by computing real-time lighting. The lighting uses a common
//   model called "Phong lighting", which combines THREE separate contributions for each pixel:
//       1) AMBIENT  - a small, constant base light so the object is never pitch black.
//       2) DIFFUSE  - light reflecting off a surface depends on how directly the surface faces the light.
//       3) SPECULAR - a bright, concentrated highlight (like a glossy shine) where the light reflects toward the camera.


// ---------------------------------------------------------------------------
// Define a struct (a small "blueprint"/custom type) describing a surface's material.
// ---------------------------------------------------------------------------

struct Material
// "struct" creates a NEW custom type (like a blueprint) that can bundle several related values into one object.
// "Material" is the name of this new type. We say a material defines how a surface LOOKS and reacts to light.
{                                    // Opening brace: start of the struct definition.

    vec3 ambient;
    // vec3 = three numbers (red, green, blue are implied).
    // "ambient" = the color the material reflects when lit ONLY by ambient (background) light.
    //             Setting this to (1.0, 0.5, 0.31) in our main.cpp gives the object an orange-ish tint.

    vec3 diffuse;
    // vec3 = three numbers.
    // "diffuse" = the main/base color of the material that you see in normal direct lighting.

    vec3 specular;
    // vec3 = three numbers.
    // "specular" = the color of the shiny highlight. We set it to white (1,1,1) in main.cpp so highlights look white.

    float shininess;
    // float = a single decimal number.
    // "shininess" = controls how TIGHT/small and bright the specular highlight is.
    //               A small value (like 2) makes a large, spread-out, dim highlight (dull surface).
    //               A large value (like 256) makes a tiny, concentrated, bright highlight (glossy/mirror-like).
};                                   // Closing brace + semicolon: end of the struct definition.


// ---------------------------------------------------------------------------
// Define a struct (custom type) describing a light source.
// ---------------------------------------------------------------------------

struct Light
// "struct" = create a new custom type.
// "Light" = name of this new type, describing the properties of our single light.
{                                    // Opening brace: start of the struct definition.

    vec3 position;
    // vec3 = three numbers (x, y, z).
    // "position" = where in the world this light sits. We pass lightPos from C++.

    vec3 ambient;
    // vec3 = the light's ambient color/intensity (we make it 30% strength in main.cpp so there's a faint base light).

    vec3 diffuse;
    // vec3 = the light's color/intensity for direct illumination (we animate this color over time).

    vec3 specular;
    // vec3 = the light's color for the shiny highlight (we keep it white).
};                                   // Closing brace + semicolon: end of the struct definition.


// ---------------------------------------------------------------------------
// Declare our "uniform" variables (set from C++, same value for every pixel).
// ---------------------------------------------------------------------------

uniform Material material;
// "uniform"  = a value uploaded from C++. For this shader, uniforms are the SAME for every pixel in the draw call.
// "Material" = the custom type we defined above.
// "material" = the actual variable, holding all the material properties. In main.cpp we upload values like
//              lightingShader.setVec3("material.ambient", ...) — notice the "material." part connects the two.

uniform Light light;
// "uniform" = uploaded from C++ once per frame.
// "Light"   = the custom type we defined above.
// "light"   = the actual light variable holding position + ambient + diffuse + specular.
//             In main.cpp we upload with names like "light.position", "light.diffuse", etc.

uniform vec3 viewPos;
// "uniform" = uploaded from C++ (we send camera.Position).
// "vec3"    = three numbers (x, y, z).
// "viewPos" = the world-space position of the camera. We need to know where the camera is so we can figure out the
//             direction "from each pixel toward the camera", which is required for the specular highlight.


// ---------------------------------------------------------------------------
// Declare our inputs that arrive from the vertex shader (interpolated per pixel).
// ---------------------------------------------------------------------------

in vec3 Normal;
// "in"    = this value is passed down from the vertex shader.
// "vec3"  = three numbers.
// "Normal"= the world-space surface direction for this pixel. The vertex shader computed it per-vertex and the GPU
//           smoothly interpolated (blended) it across all pixels of the triangle, so each pixel gets its own normal.

in vec3 FragPos;
// "in"    = passed down from the vertex shader.
// "vec3"  = three numbers.
// "FragPos" = the world-space position of this specific pixel (interpolated from the vertex positions).
//             We compare this against light.position and viewPos to get directions.


// ---------------------------------------------------------------------------
// Declare our output (the final color of this pixel).
// ---------------------------------------------------------------------------

out vec4 FragColor;
// "out"     = sent out of the shader to OpenGL's framebuffer (becomes the visible pixel).
// "vec4"    = four components: red, green, blue, alpha.
// "FragColor" = the final color we write. This is the color displayed for this pixel.


// ---------------------------------------------------------------------------
// The main function — where we actually compute the lighting. Runs per pixel.
// ---------------------------------------------------------------------------

void main()
// "void" = returns nothing.
// "main" = required entry point. Runs once for every pixel of the cube.
{                                      // Opening brace: start of main().

    // ======================================================================
    // PART 1: AMBIENT LIGHTING
    // ======================================================================
    //
    // Ambient light is a small, constant amount of light that exists everywhere (like the soft light in a room that
    // comes from no specific direction). Its purpose is to make sure that surfaces facing AWAY from the light are
    // still dimly visible instead of being completely black.
    // The formula is simply:  light's ambient color  MULTIPLIED BY  the material's ambient color.

    vec3 ambient =
        // Declare a vec3 variable named "ambient" to store our result, and set it equal to the product below.

        light.ambient *
        // "light.ambient" = the light's ambient color/intensity (we made it 30% strength in main.cpp).
        // The multiplication happens next with material.ambient (the line below).

        material.ambient;
        // "material.ambient" = the material's base color.
        // Multiplying the two colors together combines them (this is how you multiply colors: component-wise).
        // Remember, a color value like (0.5, 0.3, 0.2) means each channel (red/green/blue) separately.

    // ======================================================================
    // PART 2: NORMALIZATION
    // ======================================================================
    //
    // We need a mathematically "clean" normal to do the lighting math. A "normalized" vector has a length of exactly
    // 1 (a "unit vector"). Normalizing makes the math correct and consistent — dots products and angles only work
    // properly with unit-length vectors.

    vec3 norm =
        // Declare a vec3 named "norm" (short for normalized normal) and give it a value.

        normalize(Normal);
        // "normalize" is a built-in GLSL function that takes any vector and returns a version of it with length 1,
        // while keeping the same direction. We apply it to the incoming interpolated normal.
        // (Even though the original normals were unit-length, interpolation between corners can change the length,
        // so we re-normalize to be safe.)

    // ======================================================================
    // PART 3: LIGHT DIRECTION
    // ======================================================================
    //
    // We want to know the DIRECTION "from this pixel toward the light" so we can compare it with the surface normal.
    // A direction is just the vector "from point A to point B", which we get by subtracting the two positions:
    //   target position minus source position.

    vec3 lightDir =
        // Declare a vec3 named "lightDir" (light direction).

        normalize(
        // We wrap the subtraction in normalize(...) to get a clean unit-length direction vector.

            light.position -
            // The light's world position (from the uniform) ...
            // We SUBTRACT FragPos from it to get a vector pointing from the pixel toward the light.
            // (Subtracting a smaller/earlier point from a later point gives "from the first to the second".)

            FragPos
            // This pixel's world position (from the interpolated vertex data).
        );
        // Closing ) for normalize and ; ending the statement. "lightDir" now points from this pixel toward the light.

    // ======================================================================
    // PART 4: DIFFUSE LIGHTING
    // ======================================================================
    //
    // Diffuse lighting models how a surface reflects direct light. The key idea: the more directly a surface faces the
    // light, the brighter it is. We measure this with the "dot product" between the surface normal and the light
    // direction.
    //   - If they point in the SAME direction (surface facing the light head-on), dot = 1.0  -> brightest.
    //   - If they are at 90 degrees (surface sideways to the light), dot = 0.0              -> no diffuse light.
    //   - If the surface faces AWAY, dot is negative; we clamp that to 0 so we don't get "negative light".

    float diff =
        // Declare a float (single number) named "diff" to hold how much diffuse brightness this pixel gets.

        max(
        // "max(a, b)" returns whichever of its two arguments is larger. This is our clamp: it makes sure the diffuse
        // value never drops below 0.0, so a surface facing away gets 0 rather than a negative (invalid) value.

            dot(lightDir, norm),
            // "dot(norm, lightDir)" computes the dot product of our normalized normal and the light direction.
            // The dot product of two unit vectors equals the cosine of the angle between them, which lands in the
            // range [-1, 1] and is exactly the measure of "how directly the surface faces the light" described above.

            0.0
            // The second argument to max: the minimum allowed value. So diff is forced to be >= 0.
        );
        // Closing ) for max, ; ends the statement. "diff" now holds the diffuse brightness (0.0 to 1.0).


    // Combine the pieces together: (brightness) x (light's diffuse color) x (material's diffuse color).

    vec3 diffuse =
        // Declare a vec3 named "diffuse" to hold the actual colored diffuse result.

        diff *
        // Multiply by the brightness factor we just computed (0.0 to 1.0).

        light.diffuse *
        // Multiply by the light's diffuse color (we animate this color over time in main.cpp).

        material.diffuse;
        // Multiply by the material's main color.
        // The final "diffuse" is a color: brighter parts are closer to the light, and it's tinted by both the light
        // and material colors.

    // ======================================================================
    // PART 5: VIEW DIRECTION
    // ======================================================================
    //
    // For the specular highlight we need to know the direction "from this pixel toward the camera" (the viewer).
    // Same subtraction pattern as the light direction, but using the camera position instead.

    vec3 viewDir =
        // Declare a vec3 named "viewDir" (view direction).

        normalize(
        // Normalize to get a clean unit-length direction.

            viewPos -
            // The camera's world position (from the uniform).
            // Subtract FragPos to get a vector pointing FROM this pixel TOWARD the camera.

            FragPos
            // This pixel's world position.
        );
        // "viewDir" now points from this pixel toward the camera.

    // ======================================================================
    // PART 6: REFLECTION DIRECTION
    // ======================================================================
    //
    // For the specular highlight, we pretend the incoming light bounces off the surface like a mirror. We need to know
    // the direction the light is REFLECTED toward, so we can compare it against the direction toward the camera.
    // If the reflected light points (roughly) at the camera, we see a highlight.

    vec3 reflectDir =
        // Declare a vec3 named "reflectDir" (reflection direction).

        reflect(
        // "reflect" is a built-in GLSL function that, given an incoming direction and a surface normal, computes the
        // mirror-reflection direction. It returns a vector.

            -lightDir,
            // First argument: the direction the light is coming IN. We negate lightDir (the minus sign flips it) because
            // lightDir points FROM the surface TOWARD the light, but the reflect() function expects the direction the
            // light is TRAVELING, which is the opposite (coming from the light TO the surface).

            norm
            // Second argument: the normalized surface normal that the light bounces off of.
        );
        // "reflectDir" now points in the direction that the incoming light is reflected off the surface.

    // ======================================================================
    // PART 7: SPECULAR LIGHTING
    // ======================================================================
    //
    // We compare the reflection direction with the view (camera) direction.
    //   - If they point the same way (reflected light heading toward the camera), we see a bright highlight.
    //   - The dot product again measures their alignment.
    //   - We then raise that number to the power of "shininess", which concentrates the highlight.

    float spec =
        // Declare a float named "spec" to hold the specular intensity.

        pow(
        // "pow(a, b)" raises the first argument (a) to the power of the second argument (b). A SHORT version of "x^y".
        // Raising our alignment value to the shininess power makes the highlight smaller and sharper: a value of 0.8
        // raised to the power 32 becomes tiny (only near-perfect alignment stays bright).

            max(
            // Clamp again so we never get negative values (surface facing away from reflected light gets 0).

                dot(viewDir, reflectDir),
                // Dot product of the view direction and the reflection direction.
                // High when the reflected light points straight at the camera, low when it points elsewhere.

                0.0
                // Minimum allowed value (0.0).
            ),
            // Closing ) for max.

            material.shininess
            // The exponent: "material.shininess". A big value = tight, bright, glossy highlight.
        );
        // "spec" now holds a value from 0.0 up to 1.0 representing how strong the highlight is.

    // Combine the pieces to get the actual colored specular result.

    vec3 specular =
        // Declare a vec3 named "specular" to hold the colored specular result.

        spec *
        // Multiply by the specular intensity (how strong the highlight is).

        light.specular *
        // Multiply by the light's specular color (we keep it white in main.cpp).

        material.specular;
        // Multiply by the material's specular color (also white in main.cpp).
        // The final "specular" is the glossy highlight color.

    // ======================================================================
    // PART 8: FINAL COLOR
    // ======================================================================
    //
    // We combine all three lighting contributions by simply ADDING them, because each one adds light on top of the
    // others (like stacking brightness). Result = ambient + diffuse + specular.

    vec3 result =
        // Declare a vec3 named "result" to hold the combined final color.

        ambient +
        // Add the ambient contribution (the constant base brightness/color).

        diffuse +
        // Add the diffuse contribution (the main lit color).

        specular;
        // Add the specular contribution (the glossy highlight).


    FragColor =
        // Assign our final color to the output variable, which OpenGL writes to the screen for this pixel.

        vec4(result, 1.0);
        // "vec4(result, 1.0)" builds the 4-component output color.
        // "result" fills the red, green, and blue channels (it's already a vec3 with three color components).
        // The second value "1.0" fills the ALPHA (transparency) channel, making the pixel fully opaque (solid).
    }
// Closing brace: end of main().
