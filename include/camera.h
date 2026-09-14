#ifndef CAMERA_H
// "ifndef" means "if not defined".
// We are asking the preprocessor:
// "Has CAMERA_H already been defined?"
// If it has NOT been defined, everything below this line is included.
//
// This is called a "header guard".
// Its purpose is to stop the contents of this file from being included multiple times,
// which could otherwise cause errors such as defining the Camera class more than once.

#define CAMERA_H
// Define CAMERA_H now.
// Because of this, if camera.h is included again later,
// the #ifndef CAMERA_H condition will be false,
// so the compiler will skip the contents of the file.

#include <glad/glad.h>
// Include GLAD.
// GLAD loads OpenGL functions so our program can actually use modern OpenGL.
//
// This camera class specifically needs GLboolean, which is an OpenGL boolean type.

#include <glm/glm.hpp>
// Include the main part of GLM (OpenGL Mathematics).
//
// GLM gives us useful mathematical types such as:
//     glm::vec3 -> a vector containing 3 values (x, y, z)
//     glm::mat4 -> a 4x4 matrix
//
// We use vec3 for positions and directions,
// and mat4 for the camera's view matrix.

#include <glm/gtc/matrix_transform.hpp>
// Include GLM's matrix-transformation functionality.
//
// This gives us useful functions such as glm::lookAt(),
// which constructs a view matrix from a camera's position,
// the point it is looking toward, and its up direction.


// -----------------------------------------------------------------------------
// CAMERA MOVEMENT ENUM
// -----------------------------------------------------------------------------

enum Camera_Movement {
// "enum" creates a special type containing a fixed set of named choices.
//
// Instead of writing mysterious numbers such as 0, 1, 2, and 3,
// we can use meaningful names.
//
// For example:
//     Camera_Movement direction = FORWARD;
//
// This makes the code much easier to understand.

    FORWARD,
// Represents moving forward in the direction the camera is facing.

    BACKWARD,
// Represents moving backward, opposite to the direction the camera is facing.

    LEFT,
// Represents moving toward the camera's left side.

    RIGHT
// Represents moving toward the camera's right side.
};


// -----------------------------------------------------------------------------
// DEFAULT CAMERA VALUES
// -----------------------------------------------------------------------------

const float YAW = -90.0f;
// "const" means this value cannot be changed after it has been initialized.
//
// Yaw describes horizontal rotation.
// Imagine standing in place and turning your head left or right:
// that rotation is Yaw.
//
// We start at -90 degrees because, with the formulas used later,
// this makes the initial Front direction:
//
//     (0, 0, -1)
//
// In other words, the camera initially looks down the negative Z axis.

const float PITCH = 0.0f;
// Pitch describes vertical rotation.
// It controls looking upward or downward.
//
// 0 degrees means the camera is looking horizontally.

const float SPEED = 2.5f;
// Default movement speed of the camera.
//
// This is not simply added to Position every frame.
// Later it gets multiplied by deltaTime so that the camera moves
// at approximately the same speed regardless of frame rate.

const float SENSITIVITY = 0.1f;
// Default mouse sensitivity.
//
// Mouse movement is multiplied by this value.
// For example, if the mouse offset is 10:
//
//     10 * 0.1 = 1
//
// So the camera's angle would change by 1 degree.

const float ZOOM = 45.0f;
// Default field of view (FOV).
//
// Despite the name "Zoom", this is really the camera's FOV.
//
// Smaller FOV  -> narrower visible area -> looks more zoomed in.
// Larger FOV   -> wider visible area -> looks more zoomed out.


// -----------------------------------------------------------------------------
// CAMERA CLASS
// -----------------------------------------------------------------------------

class Camera
{
// Create a class named Camera.
//
// A class lets us keep related data and functions together.
// This camera stores things like its position and orientation,
// and also contains functions that modify that data.


public:
// Everything below "public:" can be accessed from outside the class.
//
// For example:
//     camera.Position
// can be used by our game code because Position is public.


// -----------------------------------------------------------------------------
// CAMERA ATTRIBUTES
// -----------------------------------------------------------------------------

    glm::vec3 Position;
// Position tells us WHERE the camera physically is in the 3D world.
//
// A glm::vec3 contains three values:
//
//     x -> left/right
//     y -> up/down
//     z -> forward/backward in world space
//
// Example:
//
//     glm::vec3(0, 0, 3)
//
// means:
//
//     x = 0
//     y = 0
//     z = 3
//
// Position is a location, not a direction.

    glm::vec3 Front;
// Front tells us WHICH DIRECTION the camera is facing.
//
// Think of Front as an arrow coming out of the camera:
//
//         camera ● --------->
//
// The arrow is not another point in the world.
// It represents a direction.
//
// Example:
//
//     Front = (0, 0, -1)
//
// means the camera is looking toward negative Z.

    glm::vec3 Up;
// Up tells us which direction is "up" from the CAMERA'S point of view.
//
// This is the camera's local up direction.
//
// It is important not to confuse this with WorldUp below.
//
// WorldUp = the world's fixed idea of "up".
// Up      = the camera's current local idea of "up".

    glm::vec3 Right;
// Right tells us which direction is right relative to the camera.
//
// We use this when moving sideways:
//
//     Position += Right * distance
//
// means move to the camera's right.
//
// Similarly:
//
//     Position -= Right * distance
//
// means move to the camera's left.

    glm::vec3 WorldUp;
// WorldUp tells us which direction the WORLD considers to be up.
//
// Usually we use:
//
//     (0, 1, 0)
//
// because positive Y is vertical in our world.
//
// Unlike the camera's Up vector, WorldUp is usually kept fixed.
// It acts as a reference when calculating the camera's Right and Up vectors.


// -----------------------------------------------------------------------------
// EULER ANGLES
// -----------------------------------------------------------------------------

    float Yaw;
// Yaw controls horizontal rotation.
//
// Imagine looking straight ahead and turning your head:
//
//     left <----- camera -----> right
//
// That left/right rotation is Yaw.

    float Pitch;
// Pitch controls vertical rotation.
//
// Looking upward:
//
//             ↑
//
// Looking straight:
//
//             ●
//
// Looking downward:
//
//             ↓
//
// That up/down rotation is Pitch.


// -----------------------------------------------------------------------------
// CAMERA OPTIONS
// -----------------------------------------------------------------------------

    float MovementSpeed;
// How quickly the camera moves through the world.

    float MouseSensitivity;
// How much the camera rotates in response to mouse movement.

    float Zoom;
// The camera's current field of view.


    // -------------------------------------------------------------------------
    // CONSTRUCTOR #1
    // -------------------------------------------------------------------------

    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        // Constructor parameter named "position".
        //
        // Because it has a default value, we do not have to provide it.
        //
        // If we simply write:
        //
        //     Camera camera;
        //
        // position will automatically be:
        //
        //     (0, 0, 0)

        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        // Constructor parameter named "up".
        //
        // This becomes WorldUp.
        //
        // (0, 1, 0) means positive Y is the world's up direction.

        float yaw = YAW,
        // Starting Yaw.
        //
        // If we don't provide one, it uses the constant:
        //
        //     YAW = -90 degrees

        float pitch = PITCH
        // Starting Pitch.
        //
        // If we don't provide one, it uses:
        //
        //     PITCH = 0 degrees
    )

        // ---------------------------------------------------------------------
        // CONSTRUCTOR INITIALIZER LIST
        // ---------------------------------------------------------------------
        //
        // Everything after ":" is the constructor initializer list.
        //
        // It initializes these member variables before the constructor body
        // (the code between { and }) starts running.

        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        // Initialize Front to point along negative Z.
        //
        // This gives Front an initial value.
        // updateCameraVectors() will later calculate it again
        // from the actual Yaw and Pitch values.

        MovementSpeed(SPEED),
        // Initialize MovementSpeed using the SPEED constant.

        MouseSensitivity(SENSITIVITY),
        // Initialize MouseSensitivity using the SENSITIVITY constant.

        Zoom(ZOOM)
        // Initialize Zoom using the ZOOM constant.
    {
        Position = position;
        // Copy the constructor parameter into the camera's Position member.
        //
        // The parameter and member happen to have different scopes,
        // so:
        //
        //     position
        //
        // is the constructor's parameter,
        //
        // while:
        //
        //     Position
        //
        // is the Camera object's member variable.

        WorldUp = up;
        // Store the supplied world-up direction.

        Yaw = yaw;
        // Store the supplied starting Yaw.

        Pitch = pitch;
        // Store the supplied starting Pitch.

        updateCameraVectors();
        // IMPORTANT:
        //
        // We have now set:
        //
        //     Position
        //     WorldUp
        //     Yaw
        //     Pitch
        //
        // But Front, Right, and Up must be calculated from those values.
        //
        // So we call updateCameraVectors().
        //
        // This converts the camera's angles into its actual 3D direction vectors.
    }


    // -------------------------------------------------------------------------
    // CONSTRUCTOR #2
    // -------------------------------------------------------------------------

    Camera(
        float posX,
        // X coordinate of the camera's starting position.

        float posY,
        // Y coordinate of the camera's starting position.

        float posZ,
        // Z coordinate of the camera's starting position.

        float upX,
        // X component of the WorldUp vector.

        float upY,
        // Y component of the WorldUp vector.

        float upZ,
        // Z component of the WorldUp vector.

        float yaw,
        // Starting Yaw angle.

        float pitch
        // Starting Pitch angle.
    )

        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        // Initialize Front with the default starting direction.

        MovementSpeed(SPEED),
        // Initialize MovementSpeed.

        MouseSensitivity(SENSITIVITY),
        // Initialize MouseSensitivity.

        Zoom(ZOOM)
        // Initialize Zoom.
    {
        Position = glm::vec3(posX, posY, posZ);
        // Combine the three position numbers into one vec3.
        //
        // For example:
        //
        //     posX = 0
        //     posY = 0
        //     posZ = 3
        //
        // creates:
        //
        //     Position = (0, 0, 3)

        WorldUp = glm::vec3(upX, upY, upZ);
        // Combine upX, upY, and upZ into the WorldUp vector.

        Yaw = yaw;
        // Store the starting Yaw.

        Pitch = pitch;
        // Store the starting Pitch.

        updateCameraVectors();
        // Calculate Front, Right, and Up from the starting orientation.
    }


    // -------------------------------------------------------------------------
    // VIEW MATRIX
    // -------------------------------------------------------------------------

    glm::mat4 GetViewMatrix()
    // This function returns a glm::mat4.
    //
    // glm::mat4 is a 4x4 matrix.
    //
    // The view matrix represents the camera's point of view.
    //
    // OpenGL uses this matrix to transform the world
    // so that it appears from the camera's position and orientation.
    {
        return glm::lookAt(Position, Position + Front, Up);
        // glm::lookAt() creates the view matrix for us.
        //
        // It needs three important pieces of information:
        //
        // 1. Position
        // 2. The point we want to look toward
        // 3. Which direction should be considered "up"
        //
        // ---------------------------------------------------------------------
        // FIRST ARGUMENT: Position
        // ---------------------------------------------------------------------
        //
        // Position says WHERE the camera is.
        //
        // Example:
        //
        //     Position = (0, 0, 3)
        //
        // means the camera is located at (0, 0, 3).
        //
        // ---------------------------------------------------------------------
        // SECOND ARGUMENT: Position + Front
        // ---------------------------------------------------------------------
        //
        // Front is a DIRECTION, not a position.
        //
        // Suppose:
        //
        //     Position = (0, 0, 3)
        //     Front    = (0, 0, -1)
        //
        // Adding them gives:
        //
        //     (0, 0, 3) + (0, 0, -1)
        //     = (0, 0, 2)
        //
        // That gives us a point in front of the camera.
        //
        // Therefore:
        //
        //     Position + Front
        //
        // basically means:
        //
        //     "Give me a point in the direction I'm facing."
        //
        // ---------------------------------------------------------------------
        // THIRD ARGUMENT: Up
        // ---------------------------------------------------------------------
        //
        // Up tells lookAt which way is "up" from the camera's perspective.
        //
        // Finally, lookAt returns a 4x4 VIEW MATRIX.
    }


    // -------------------------------------------------------------------------
    // KEYBOARD MOVEMENT
    // -------------------------------------------------------------------------

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    // This function handles movement.
    //
    // direction tells us WHICH way to move:
    //
    //     FORWARD
    //     BACKWARD
    //     LEFT
    //     RIGHT
    //
    // deltaTime tells us how much time passed since the previous frame.
    {
        float velocity = MovementSpeed * deltaTime;
        // Calculate how far we should move during this frame.
        //
        // Why multiply by deltaTime?
        //
        // Imagine one computer renders 60 frames per second,
        // while another renders 120.
        //
        // If we moved a fixed amount every frame,
        // the faster computer would move the camera twice as fast.
        //
        // deltaTime fixes that.
        //
        // Example:
        //
        //     MovementSpeed = 2.5
        //     deltaTime    = 0.016
        //
        // gives approximately:
        //
        //     velocity = 0.04
        //
        // units of movement during this frame.

        if (direction == FORWARD)
        // Check whether direction is FORWARD.
        {
            // If yes, move in the direction the camera is facing.

            Position += Front * velocity;
            // Front tells us WHICH direction to move.
            // velocity tells us HOW FAR to move.
            //
            // Example:
            //
            //     Front = (0, 0, -1)
            //     velocity = 0.1
            //
            // then:
            //
            //     Front * velocity
            //     = (0, 0, -0.1)
            //
            // So the camera moves 0.1 units toward negative Z.
        }

        if (direction == BACKWARD)
        // Check whether direction is BACKWARD.
        {
            Position -= Front * velocity;
            // Subtracting Front moves opposite to the direction the camera faces.
            //
            // If Front points forward,
            // -Front points backward.
        }

        if (direction == LEFT)
        // Check whether direction is LEFT.
        {
            Position -= Right * velocity;
            // Right points toward the camera's right.
            //
            // Therefore subtracting Right moves toward the camera's left.
        }

        if (direction == RIGHT)
        // Check whether direction is RIGHT.
        {
            Position += Right * velocity;
            // Move in the direction of the camera's Right vector.
        }
    }


    // -------------------------------------------------------------------------
    // MOUSE MOVEMENT
    // -------------------------------------------------------------------------

    void ProcessMouseMovement(
        float xoffset,
        // How much the mouse moved horizontally.
        //
        // Positive usually means movement to the right.
        // Negative usually means movement to the left.

        float yoffset,
        // How much the mouse moved vertically.

        GLboolean constrainPitch = true
        // GLboolean is an OpenGL boolean type.
        //
        // constrainPitch determines whether Pitch should be restricted
        // to a safe range.
        //
        // It defaults to true,
        // so we normally prevent the camera from flipping upside down.
    )
    {
        xoffset *= MouseSensitivity;
        // Scale the horizontal mouse movement according to sensitivity.
        //
        // Example:
        //
        //     xoffset = 5
        //     sensitivity = 0.1
        //
        // gives:
        //
        //     5 * 0.1 = 0.5
        //
        // Therefore Yaw changes by only 0.5 degrees.

        yoffset *= MouseSensitivity;
        // Do the same thing for vertical mouse movement.

        Yaw += xoffset;
        // Horizontal mouse movement changes the Yaw angle.
        //
        // Moving the mouse left/right causes the camera to turn left/right.

        Pitch += yoffset;
        // Vertical mouse movement changes the Pitch angle.
        //
        // Moving the mouse up/down causes the camera to look up/down.


        // ---------------------------------------------------------------------
        // LIMIT PITCH
        // ---------------------------------------------------------------------

        if (constrainPitch)
        // Only perform these checks if pitch limiting is enabled.
        {
            if (Pitch > 89.0f)
            // If the camera tries to look above 89 degrees...

                Pitch = 89.0f;
                // Stop it at 89 degrees.
                //
                // We do not allow exactly 90 degrees because that creates
                // a problematic situation for calculating the camera's
                // Right vector.

            if (Pitch < -89.0f)
            // If the camera tries to look below -89 degrees...

                Pitch = -89.0f;
                // Stop it at -89 degrees.
                //
                // This prevents the camera from flipping over.
        }

        updateCameraVectors();
        // IMPORTANT:
        //
        // We changed Yaw and/or Pitch above.
        //
        // But changing an angle does NOT automatically change
        // the Front, Right, and Up vectors.
        //
        // So we recalculate those vectors now.
    }


    // -------------------------------------------------------------------------
    // MOUSE SCROLL / FOV
    // -------------------------------------------------------------------------

    void ProcessMouseScroll(float yoffset)
    // Handles vertical mouse-wheel movement.
    //
    // yoffset tells us how far the wheel moved.
    {
        Zoom -= (float)yoffset;
        // Change the camera's field of view.
        //
        // The (float) converts yoffset to a float.
        //
        // Usually, scrolling upward gives positive yoffset,
        // so subtracting it makes the FOV smaller.
        //
        // Smaller FOV:
        //     narrower visible area
        //     stronger zoom effect

        if (Zoom < 0.1f)
        // Prevent Zoom/FOV from becoming extremely small or negative.

            Zoom = 0.1f;
            // Clamp the minimum value to 0.1 degrees.

        if (Zoom > 180.0f)
        // Prevent the FOV from becoming larger than 180 degrees.

            Zoom = 180.0f;
            // Clamp the maximum value to 180 degrees.
    }


private:
// Everything below "private:" cannot normally be accessed directly
// from outside the Camera class.
//
// updateCameraVectors() is an internal helper function.
// Other code doesn't need to call it directly;
// ProcessMouseMovement() and the constructors call it when necessary.


// -----------------------------------------------------------------------------
// UPDATE CAMERA DIRECTION VECTORS
// -----------------------------------------------------------------------------

    void updateCameraVectors()
    // This function is the mathematical heart of the camera.
    //
    // Its job is to take:
    //
    //     Yaw
    //     Pitch
    //
    // and turn them into:
    //
    //     Front
    //     Right
    //     Up
    //
    // In other words:
    //
    //     angles
    //        ↓
    //     direction
    //        ↓
    //     camera coordinate system
    {
        // ---------------------------------------------------------------------
        // STEP 1: CALCULATE FRONT
        // ---------------------------------------------------------------------

        glm::vec3 front;
        // Create a temporary vec3 called front.
        //
        // It contains:
        //
        //     front.x
        //     front.y
        //     front.z
        //
        // We are going to calculate each of those components
        // using the camera's Yaw and Pitch.


        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        // Calculate the X component of the direction the camera is facing.
        //
        // There are two important functions here:
        //
        //     glm::radians()
        //     cos()
        //
        // ---------------------------------------------------------------------
        // glm::radians(Yaw)
        // ---------------------------------------------------------------------
        //
        // Our Yaw is stored in DEGREES because degrees are convenient for humans:
        //
        //     0
        //     45
        //     90
        //
        // But the trigonometric functions sin() and cos()
        // expect their angles in RADIANS.
        //
        // glm::radians() converts:
        //
        //     degrees -> radians
        //
        // ---------------------------------------------------------------------
        // cos(Yaw)
        // ---------------------------------------------------------------------
        //
        // Cosine helps determine the horizontal X component
        // of a direction based on the angle.
        //
        // Yaw controls horizontal rotation,
        // so cos(Yaw) helps determine how much of our direction points along X.
        //
        // ---------------------------------------------------------------------
        // cos(Pitch)
        // ---------------------------------------------------------------------
        //
        // Pitch controls looking up/down.
        //
        // When we look upward or downward,
        // less of our direction remains in the horizontal plane.
        //
        // cos(Pitch) accounts for that reduction.
        //
        // This is why BOTH X and Z contain cos(Pitch).


        front.y = sin(glm::radians(Pitch));
        // Calculate the Y component of the camera's direction.
        //
        // Pitch controls vertical direction,
        // so sine is used here to calculate how much the camera points upward/downward.
        //
        // Some useful examples:
        //
        // Pitch = 0 degrees
        // sin(0) = 0
        // -> front.y = 0
        // -> camera is looking horizontally
        //
        // Positive Pitch
        // -> front.y becomes positive
        // -> camera is looking upward
        //
        // Negative Pitch
        // -> front.y becomes negative
        // -> camera is looking downward.


        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        // Calculate the Z component of the direction.
        //
        // sin(Yaw) determines the horizontal Z component.
        //
        // cos(Pitch) reduces the horizontal part when we look upward/downward.
        //
        // Together, the three equations:
        //
        //     front.x
        //     front.y
        //     front.z
        //
        // convert the camera's two angles:
        //
        //     Yaw
        //     Pitch
        //
        // into one 3D direction:
        //
        //     (x, y, z)


        // ---------------------------------------------------------------------
        // EXAMPLE OF THE FORMULA
        // ---------------------------------------------------------------------
        //
        // Suppose:
        //
        //     Yaw   = -90 degrees
        //     Pitch = 0 degrees
        //
        // Then:
        //
        //     cos(-90) = 0
        //     sin(-90) = -1
        //     cos(0)   = 1
        //     sin(0)   = 0
        //
        // Therefore:
        //
        //     front.x = 0 * 1 = 0
        //     front.y = 0
        //     front.z = -1 * 1 = -1
        //
        // So:
        //
        //     front = (0, 0, -1)
        //
        // This means:
        //
        //     "The camera is looking directly down the negative Z axis."


        Front = glm::normalize(front);
        // Store the calculated direction in the camera's Front variable.
        //
        // normalize() changes the LENGTH of the vector,
        // but keeps its DIRECTION the same.
        //
        // Example:
        //
        //     (10, 0, 0)
        //
        // points in exactly the same direction as:
        //
        //     (1, 0, 0)
        //
        // but the first vector has length 10,
        // while the second has length 1.
        //
        // A vector with length 1 is called a "unit vector".
        //
        // Direction vectors such as Front are normally kept normalized
        // so that their length does not accidentally affect movement calculations.


        // ---------------------------------------------------------------------
        // STEP 2: CALCULATE RIGHT
        // ---------------------------------------------------------------------

        Right = glm::normalize(glm::cross(Front, WorldUp));
        // This calculates the camera's Right vector.
        //
        // The important function here is:
        //
        //     glm::cross()
        //
        // The cross product takes TWO 3D vectors and produces
        // a NEW vector that is perpendicular to both of them.
        //
        // We give it:
        //
        //     Front
        //     WorldUp
        //
        // because we want to find a direction that is perpendicular
        // to both the direction we are looking
        // and the world's up direction.
        //
        // That gives us the camera's Right direction.
        //
        //
        // ---------------------------------------------------------------------
        // WHY DOES THE ORDER MATTER?
        // ---------------------------------------------------------------------
        //
        // The cross product is NOT commutative.
        //
        // In general:
        //
        //     A × B != B × A
        //
        // Reversing the order reverses the resulting direction.
        //
        // So:
        //
        //     Front × WorldUp
        //
        // points one way,
        //
        // while:
        //
        //     WorldUp × Front
        //
        // points the opposite way.
        //
        // We use Front × WorldUp because we want the camera's RIGHT direction.
        //
        //
        // Finally, normalize() gives Right a length of 1.


        // When Front becomes almost parallel to WorldUp,
        // their cross product becomes very small.
        //
        // This happens when Pitch approaches +90 or -90 degrees.
        //
        // That's another reason the camera limits Pitch to:
        //
        //     +89 degrees
        //     -89 degrees
        //
        // rather than allowing exactly +90 or -90.


        // ---------------------------------------------------------------------
        // STEP 3: CALCULATE UP
        // ---------------------------------------------------------------------

        Up = glm::normalize(glm::cross(Right, Front));
        // Now we know two camera directions:
        //
        //     Right
        //     Front
        //
        // So we can calculate the camera's local Up direction.
        //
        // Again, the cross product gives us a vector perpendicular
        // to the two vectors supplied.
        //
        // Therefore:
        //
        //     Right × Front
        //
        // gives the direction that becomes the camera's Up.
        //
        // Then normalize() makes Up a unit vector.
        //
        //
        // IMPORTANT:
        //
        // WorldUp and Up are NOT the same thing.
        //
        // WorldUp:
        //     fixed reference direction for the world.
        //
        // Up:
        //     the camera's current local up direction.
        //
        // The cross products allow Front, Right, and Up
        // to form a proper perpendicular camera coordinate system.
    }
};


#endif
// End of the header guard.
//
// Because CAMERA_H was defined at the beginning,
// including this file again will not define the Camera class a second time.
