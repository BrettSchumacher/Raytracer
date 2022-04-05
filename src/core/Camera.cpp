#include "Camera.h"

using namespace std;

Camera::Camera()
{
    parameters_set = 0;
    gamma = 1.0f;
    num_samples = 1;
    num_bounces = 1;
    screen_plane_set = false;
}

Camera::Camera(Vector3 position, Vector3 forward, Vector3 up, Float v_fov, Float dist_to_plane, int pixel_width, int pixel_height)
{
    SetPosition(position);
    SetForward(forward);
    SetUp(up);
    SetVFOV(v_fov);
    SetDistToPlane(dist_to_plane);
    SetAspectRatio(pixel_width, pixel_height);
    gamma = 1.0f;
    num_samples = 1;
    num_bounces = 1;
    screen_plane_set = false;
}

#pragma region Setters and Getters

void Camera::SetPosition(Vector3 position)
{
    this->position = position;
    parameters_set |= (1 << 0);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetForward(Vector3 forward)
{
    this->forward = forward.normalized();
    parameters_set |= (1 << 1);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetUp(Vector3 up)
{
    this->up = up.normalized();
    parameters_set |= (1 << 2);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetVFOV(Float v_fov)
{
    this->v_fov = v_fov;
    parameters_set |= (1 << 3);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetDistToPlane(Float dist_to_plane)
{
    this->dist_to_plane = dist_to_plane;
    parameters_set |= (1 << 4);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetPixelWidth(int pixel_width)
{
    this->pixel_width = pixel_width;
    parameters_set |= (1 << 5);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetPixelHeight(int pixel_height)
{
    this->pixel_height = pixel_height;
    parameters_set |= (1 << 6);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetAspectRatio(Float aspect_ratio)
{
    this->aspect_ratio = aspect_ratio;
    parameters_set |= (1 << 7);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetAspectRatio(int pixel_width, int pixel_height)
{
    this->pixel_width = pixel_width;
    this->pixel_height = pixel_height;
    this->aspect_ratio = (float)pixel_width / (float)pixel_height;
    parameters_set |= (1 << 5) | (1 << 6) | (1 << 7);
    if (screen_plane_set)
    {
        screen_plane_set = false;
    }
}

void Camera::SetGamma(Float gamma)
{
    this->gamma = gamma;
}

void Camera::SetIOR(Float ior)
{
    this->ior = ior;
}

void Camera::SetThreads(unsigned int threads)
{
    this->threads = threads;
}

void Camera::SetNumSamples(unsigned int num_samples)
{
    this->num_samples = num_samples;
}

void Camera::SetNumBounces(unsigned int num_bounces)
{
    this->num_bounces = num_bounces;
}

Vector3 Camera::GetPosition()
{
    return position;
}

Vector3 Camera::GetForward()
{
    return forward;
}

Vector3 Camera::GetUp()
{
    return up;
}

Float Camera::GetVFOV()
{
    return v_fov;
}

Float Camera::GetDistToPlane()
{
    return dist_to_plane;
}

int Camera::GetPixelWidth()
{
    return pixel_width;
}

int Camera::GetPixelHeight()
{
    return pixel_height;
}

Float Camera::GetAspectRatio()
{
    return aspect_ratio;
}

Float Camera::GetGamma()
{
    return gamma;
}

Float Camera::GetIOR()
{
    return ior;
}

Vector3 Camera::GetScreenUp()
{
    if (!screen_plane_set)
    {
        CalculateScreenPlane();
    }
    return screen_up;
}

Vector3 Camera::GetScreenRight()
{
    if (!screen_plane_set)
    {
        CalculateScreenPlane();
    }
    return screen_right;
}

Vector3 Camera::GetScreenLowerLeft()
{
    if (!screen_plane_set)
    {
        CalculateScreenPlane();
    }
    return screen_lower_left;
}

Vector3 Camera::GetScreenUpperRight()
{
    if (!screen_plane_set)
    {
        CalculateScreenPlane();
    }
    return screen_upper_right;
}

unsigned int Camera::GetThreads()
{
    return threads;
}

unsigned int Camera::GetNumSamples()
{
    return num_samples;
}

unsigned int Camera::GetNumBounces()
{
    return num_bounces;
}

#pragma endregion

bool Camera::IsValid()
{
    // first check that the user has set all the necessary parameters
    if (parameters_set != ((1 << numParameters) - 1))
    {
        std::cout << "ERROR: Insufficient scene data provided\n";
        return false;
    }

    // check that forward and up are not parallel
    // since they are both normalized, we can just check that the dot product is not 1
    if (abs(forward.dot(up)) > 0.99f)
    {
        std::cout << "ERROR: Forward and up vectors are co-linear\n";
        return false;
    }

    // can add more tests here if necessary

    return true;
}

void Camera::CalculateScreenPlane()
{
    screen_right = forward.cross(up).normalized();
    screen_up = screen_right.cross(forward).normalized();

    Vector3 center = position + forward * dist_to_plane;
    screen_height = 2.0f * tan(v_fov * 0.5f * M_PI / 180.0f) * dist_to_plane;
    screen_width = screen_height * aspect_ratio;

    screen_lower_left = center - screen_up * screen_height * 0.5f - screen_right * screen_width * 0.5f;
    screen_upper_right = center + screen_up * screen_height * 0.5f + screen_right * screen_width * 0.5f; // I never actually use this

    screen_plane_set = true;
}

Ray Camera::CreateCameraRay(Float x, Float y)
{
    if (!screen_plane_set)
    {
        CalculateScreenPlane();
    }

    // get u and v coordinates in range [0, 1] from x and y, where x [0, pixel_width - 1] and y [0, pixel_height - 1]
    Float u = x / pixel_width;
    Float v = y / pixel_height;

    // calculate world space coordinates and generate ray
    // used different method than the one shown in class since this one makes more sense to me
    Vector3 screen_point = screen_lower_left + screen_up * screen_height * v + screen_right * screen_width * u;
    Vector3 ray_direction = screen_point - position;

    Ray ray = Ray(position, ray_direction, ior);
    return ray;
}

int Camera::RenderScene(Scene& scene, Image& output)
{
    this->CalculateScreenPlane();

    // first initialize output image
    output.SetDimensions(pixel_width, pixel_height);

    // first see if we need to use multithreading
    if (threads == 1)
    {
        RenderScenePartial(scene, output, 0, pixel_height);
        return 0;
    }

    unsigned int numThreads = min(threads, thread::hardware_concurrency()); // don't use more threads than available
    numThreads = min(threads, (unsigned int) pixel_height); // don't need more threads than there are rows

    // create threads
    vector<thread> threads;
    int startRow, endRow;

    for (unsigned int i = 0; i < numThreads; i++)
    {
        startRow = i * pixel_height / numThreads;
        endRow = (i + 1) * pixel_height / numThreads;
        threads.push_back(thread(&Camera::RenderScenePartial, this, ref(scene), ref(output), startRow, endRow));
    }

    // wait for threads to finish
    for (unsigned int i = 0; i < numThreads; i++)
    {
        threads[i].join();
    }

    return 0;
}

// assume setup from RenderScene has already been done
// renders rows from yStart (inclusive) to yEnd (exclusive)
void Camera::RenderScenePartial(Scene& scene, Image& output, int yStart, int yEnd)
{
    Ray ray;
    Vector3 color;
    int pixel_index = yStart * pixel_width;
    Float x_offset;
    Float y_offset;

    // for each pixel, generate ray and use scene to trace it
    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = 0; x < pixel_width; x++)
        {
            color = Vector3(0.0f, 0.0f, 0.0f);

            // trace each sample with random offset inside pixel
            for (int i = 0; i < num_samples; i++)
            {
                if (num_samples == 1)
                {
                    x_offset = 0.5;
                    y_offset = 0.5;
                }
                else
                {
                    x_offset = (Float) rand() / (Float) RAND_MAX;
                    y_offset = (Float) rand() / (Float) RAND_MAX;
                }
                ray = CreateCameraRay(x + x_offset, y + y_offset);
                Float dist;
                color += scene.TraceRay(ray, num_bounces, dist);
            }

            color /= num_samples;
            output[pixel_index] = GammaCorrect(color);
            pixel_index++;
        }
    }
}

Vector3 Camera::GammaCorrect(Vector3 color)
{
    if (gamma == 1.0) // don't waste time if gamma is 1
    {
        return color;
    }
    else if (abs(gamma - 2.0) < 0.01) // use sqrt if we can (I think it's faster that pow(,0.5)???)
    {
        return Vector3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
    }
    else
    {
        return Vector3(pow(color.x, 1.0 / gamma), pow(color.y, 1.0 / gamma), pow(color.z, 1.0 / gamma));
    }
}
