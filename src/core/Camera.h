#ifndef CAMERA_H
#define CAMERA_H

#include "math/Vector3.h"
#include "Ray.h"
#include "Scene.h"
#include "Image.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <thread>
#include <vector>
#include <functional>

using namespace std;

class Camera
{
    public:
        Camera();
        Camera(Vector3 position, Vector3 forward, Vector3 up, Float v_fov, Float dist_to_plane, int pixel_width, int pixel_height);

        void CalculateScreenPlane();                // calculates screen up and right vectors
        Ray CreateCameraRay(Float x, Float y);      // creates a ray from camera pixel coordinates (x, y)

        int RenderScene(Scene& scene, Image& output); // renders the scene into Image output
        Vector3 GammaCorrect(Vector3 color);

        bool IsValid();                             // Returns true if all parameters are set

        #pragma region Getters and Setters

        void SetPosition(Vector3 position);
        void SetForward(Vector3 forward);
        void SetUp(Vector3 up);
        void SetVFOV(Float v_fov);
        void SetDistToPlane(Float dist_to_plane);
        void SetPixelWidth(int pixel_width);
        void SetPixelHeight(int pixel_height);
        void SetAspectRatio(Float aspect_ratio);
        void SetAspectRatio(int pixel_width, int pixel_height);
        void SetGamma(Float gamma);
        void SetIOR(Float ior);
        void SetThreads(unsigned int threads);
        void SetNumSamples(unsigned int numSamples);
        void SetNumBounces(unsigned int numBounces);

        Vector3 GetPosition();
        Vector3 GetForward();
        Vector3 GetUp();
        Float GetVFOV();
        Float GetDistToPlane();
        int GetPixelWidth();
        int GetPixelHeight();
        Float GetAspectRatio();
        Float GetGamma();
        Float GetIOR();
        unsigned int GetThreads();
        unsigned int GetNumSamples();
        unsigned int GetNumBounces();

        Vector3 GetScreenUp();
        Vector3 GetScreenRight();
        Vector3 GetScreenLowerLeft();
        Vector3 GetScreenUpperRight();

        #pragma endregion
    private:
        int parameters_set;                         // bitmask of parameters that have been set
        static const int numParameters = 8;         // number of parameters that need to be set
        Vector3 position;                           // param 0
        Vector3 forward;                            // param 1
        Vector3 up;                                 // param 2
        Float dist_to_plane;                        // param 3 (TODO: make this not a parameter)
        Float v_fov;                                // param 4
        int pixel_width;                            // param 5
        int pixel_height;                           // param 6
        Float aspect_ratio;                         // param 7
        unsigned int num_samples;                   // number of samples per pixel
        unsigned int num_bounces;                   // max number of bounces per raycast

        bool screen_plane_set;                      // true if screen plane has been calculated
        Vector3 screen_right;                       // normalized screen right vector
        Vector3 screen_up;                          // normalized screen up vector
        Vector3 screen_lower_left;                  // lower left world space coordinates of screen
        Vector3 screen_upper_right;                 // upper right world space coordinates of screen
        Float screen_width;                         // screen width in world space
        Float screen_height;                        // screen height in world space
        Float gamma;                                // gamma correction factor
        Float ior = 1;                              // index of refraction where the camera is located
        unsigned int threads = 1;                   // number of threads to use for rendering (1 default = no multithreading)

        void RenderScenePartial(Scene& scene, Image& output, int yStart, int yEnd);
};

#endif