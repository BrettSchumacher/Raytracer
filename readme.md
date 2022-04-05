# CSCI 5607 Raytracer
![](/oldInputs/demo.png)

The code for HW1 (parts a-d) of CSCI 5607 in Spring 2022.

## Important Notes

Renders for the given hw1d scenes are inside the hw1d folder, with a "_mine" suffix to distinguish them from the official renders. I also added a shadow_test.txt scene to show off the partial shadows from transparent objects, additionally it showcases the fresnel reflections working as expected on the opaque sphere in the scene.

My demo scene is demo.png, it showcases a refractive lens with 2 spheres with normal and texture maps as well as a milkyway background. You can see how the refractive lens leads to magnification of the background (to where you can notice the pixelation), and you can see the milkyway also reflected on the surface of the right sphere.

# Overview
## Compilation
First make sure your working directory is inside the Raytracer folder, then run:
```
make clean
make
``` 
Alternatively, you can replace the second line with `make double` which will compile the program to use `doubles` instead of `floats`, helping to avoid artifacts that can appear due to floating point imprecision in some renders.

## Running the program
After you have built the program, you can render a scene with the following command:
```
./raytracer <input_file> [<output_file>]
```
where `input_file` is the relative path to the description file of the scene you are trying to render, and `output_file` is the relative path to the file you would like the output generated in.  
If there is no output file specified, it will save the image to the name of the input file appended with ".ppm".

## Functionality  
The program implements the following extra credit features:
### Part a
- Cylinder rendering (with end caps)  
### Part b
- Soft shadows
- Light attenuation
- Depthcueing
### Part c
- Normal mapping
### Pard d
- None :( I ran out of time

### Additional
Beyond the basic requirements for the assigment, the program also supports the following:
- Multithreading
- Antialiasing
- Gamma correction
- Spotlights
- Bounding Volume Hierarchy (BVH) acceleration, disabled by default.
- OBJ file loading.
- Quad meshes (to better support OBJ files)
- glTF file loading.
    - Specifically developed for Blender's glTF separate format. (.gltf + .bin + textures)
- hdri environment map loading

### Known Issues
- When referencing files (such as for textures or models), the program searches from the raytracer executable's directory, rather than the scene file's directory. So if you have a scene file and its textures in a different directory, it will likely not find them.
- Texture mapping is different than in the examples shown. This is because I followed Blender's UV mapping system so that I could use scenes exported from Blender.

## External Sources
This program uses the following external sources:
- For loading in png/jpg images for the glTF files: https://github.com/nothings/stb
- For parsing the json format of the glTF files: https://github.com/nlohmann/json 
- For reference when making the BVH: https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies
- glTF loader adapted from: https://github.com/VictorGordan/opengl-tutorials/tree/main/YoutubeOpenGL%2013%20-%20Model%20Loading
    - More details in a comment at the top of InputReader.cpp
- PBR textures for demos downloaded from: https://www.cgbookcase.com/
- Milkyway background for demo1d found in thread: https://blenderartists.org/t/high-resolution-milky-way-hdri-or-background-image-willing-to-pay/1176021/2 

## Planned Features
- Per object transforms
    - Since I created a transform class for reading in glTF models, I want to also use these for the sphere/cylinder objects to allow for easier texture mapping.
- glTF scene loading
    - I want to be able to read in light and camera information from glTF files and allow the user to render the scene directly from the glTF file rather than having to reference it in a separate scene file.
- Alternate output image types.
    - Since I already use stb_image to load in jpg/png textures, I want to add functionality to output to those formats as well.

# Scene file format
To render a scene, you need to provide the raytracer with a properly formatted input file. If there are any errors parsing the file, the program will tell you the line in which the error ocurred as well as a (hopefully) helpful error message.

## Required arguments
In order for the scene to be able to render, at least basic camera properties must be defined inside the input file. These commands are the following:

---
### eye
Eye specifies the camera location inside the scene, a proper use of it looks like this:
```
eye <x> <y> <z>
```
which would place the camera at the location (`x`,`y`,`z`).

---
### viewdir
Viewdir tells the camera which direction to point, defining the normal of the screen plane, it should be invoked like this:
```
viewdir <dir_x> <dir_y> <dir_z>
```
which would set the camera's forward vector as (`dir_x`, `dir_y`, `dir_z`).

---
### updir
Updir sets the camera's roll rotation, orienting the scene plane so that it's vertically aligned with the updir.
```
updir <dir_x> <dir_y> <dir_z>
```
This sets the camera's up vector to (`dir_x`, `dir_y`, `dir_z`).

---
### vfov
The vertical field of view, determines the range of vertical angles in which rays are cast out from the camera.
```
vfov <degrees>
```
This sets the vfov of the camera to the `degrees` (**note**: this *is* degrees, not radians).

---
### imsize
Imsize specifies the pixel dimensions of the image to output. This also determine's the image's aspect ratio as well as horizontal fov.
```
imsize <pixel_width> <pixel_height>
```
This would set the camera to render an image with pixel dimensions (`pixel_width`, `pixel_height`). (**note**: both should be integers)

---
## Optional Arguments
All of the following are optional arguments, though if none of them are used, you're gonna get a pretty boring image.

---
### bkgcolor
Used to set the background color of the image, by default this is set to black.
```
bkgcolor <r> <g> <b>
```
This would set the background color as defined by (`r`, `g`, `b`) in rgb colorspace. (**note**: these should be passed in as floating point numbers in the range [0,1]).

---
### hdri
Used to load in an environment map, can be any image format recognized by stb_image.
```
hdri <path_to_image>
```

---
### bvh
Used to enable BVH acceleration. It takes in a single argument, which is the maximum depth of the BVH (I usually set this to around 20 for large scenes).
```
bvh <max_depth>
```

---
### mtlcolor
Used to define a material used in the Phong Illumination Model. This will be applied to any objects declared after, until a new material is made. The default material is black.
```
mtlcolor <Odr> <Odg> <Odb> <Osr> <Osg> <Osb> <ka> <kd> <ks> <n> <alpha> <ior>
```
This would set the material's diffuse color to (`Odr`, `Odg`, `Odb`) in rgb colorspace; specular color to (`Osr`, `Osg`, `Osb`) in rgb colorspace; the ambient, diffuse, and specular coefficients to (`ka`, `kd`, `ks`) respectively; and the specular exponent to `n`. `alpha` is the transparency of the material (with 0 being fully transparent), and `ior` is the index of refraction of the material. 

**note**: these should be passed in as floating point numbers in the range [0,1], except for `n` which should be a positive integer. As well as `alpha` which can be any floating point number, since transparency is implemented using Beer's Law.

---
### texture
Used to add an image to the most recently declared material. If it's a ppm it will use the ppm loader I wrote, otherwise my program will attempt to load the image using the stb_image library. 
```
texture <filename>
```  
One current issue with this command is that it only searches for the file from the relative path of the raytracer program, so if your scene file and textures are in a different folder it will likely break.  
(*note:* I did not see that we could assume the entire ppm header would be on 1 line, so my loader should work for any ppm with a valid header)

---
### bump
Used to enable normal mapping. This will be applied to any objects declared after, until a new material is made.
```
bump <filename>
```

---
### sphere
Used to place a sphere inside the scene.
```
sphere <x> <y> <z> <r>
```
This would place a sphere at the location (`x`, `y`, `z`) with radius `r`.

---
### cylinder
Used to place a finite cylinder with end caps inside the scene.
```
cylinder <x> <y> <z> <dir_x> <dir_y> <dir_z> <r> <h>
```
This would place a cylinder centered at (`x`, `y`, `z`) with radius `r` and height `h`, oriented along the direction vector (`dir_x`, `dir_y`, `dir_z`).

---
### obj
Used to load in an OBJ file. It will only load in the vertex and face data, ignoring all other lines.
```
obj <filename>
```
If you want do want to load in textures and materials with the object data, you should convert it to a glTF file (probably easiest by importing it into Blender) and use the glTF command instead.

---
### glft
Used to load in a glTF file. Only works for separated glTF files with a .gltf and .bin file. I've only tested it with Blender's glTF separate format, so it very likely won't work with other glTF formats or exporters. Unlike obj, it will read in materials and textures.
```
gltf <filename>
```
**Note:** Getting the glTF PBR materials to work with the raytracer was a bit tricky, and the roughness maps may not work entirely correctly yet. Hopefully I will have it working before part d is due.

---
### light
Used to place a light source inside the scene. There are 3 possible light sources: directional, point, and spotlight.  
To generate a directional light, use:  
```
light <dir_x> <dir_y> <dir_z> 0 <r> <g> <b>
```
This will create a directional light with direction (`dir_x`, `dir_y`, `dir_z`) and color (`r`, `g`, `b`).  

To generate a point light, use:
```
light <x> <y> <z> 1 <r> <g> <b>
```
This will create a point light at the location (`x`, `y`, `z`) with color (`r`, `g`, `b`).  

To generate a spotlight, use:
```
light <x> <y> <z> 2 <dir_x> <dir_y> <dir_z> <inner_angle> <outer_angle> <r> <g> <b>
```
This will create a spotlight at the location (`x`, `y`, `z`) with direction (`dir_x`, `dir_y`, `dir_z`), inner and outer cone angles (`inner_angle`, `outer_angle`), and color (`r`, `g`, `b`). (**note**: `inner_angle` and `outer_angle` are in degrees).

---
### attlight
Used to place an attenuated light source inside the scene. There are 2 possible attenuated light sources: point and spotlight. (A directional attenuated light would not make sense, so it is not supported.)  
To generate an attenuated point light, use:
```
attlight <x> <y> <z> 1 <r> <g> <b> <c1> <c2> <c3>
```
This will create an attenuated point light at the location (`x`, `y`, `z`) with color (`r`, `g`, `b`), and attenuation coefficients (`c1`, `c2`, `c3`) with c1 as the constant term, c2 as the linear term, and c3 as the quadratic term.  

To generate an attenuated spotlight, use:
```
attlight <x> <y> <z> 2 <dir_x> <dir_y> <dir_z> <inner_angle> <outer_angle> <r> <g> <b> <c1> <c2> <c3>
```
This will create an attenuated spotlight at the location (`x`, `y`, `z`) with direction (`dir_x`, `dir_y`, `dir_z`), inner and outer cone angles (`inner_angle`, `outer_angle`), and color (`r`, `g`, `b`), and attenuation coefficients (`c1`, `c2`, `c3`) with c1 as the constant term, c2 as the linear term, and c3 as the quadratic term. (**note**: `inner_angle` and `outer_angle` are in degrees).

---
### depthcueing
Used to set the depthcueing. By default, depthcueing is turned off.
``` 
depthcueing <r> <g> <b> <alpha_max> <alpha_min> <dist_max> <dist_min>
```
This will set the depthcueing to be on, with the color (`r`, `g`, `b`) in rgb colorspace. Between distances `dist_min` and `dist_max` from the camera, the alpha value of the color will be linearly interpolated between `alpha_min` and `alpha_max` and added to the pixel's color.

---
### shadowSamples
Used to set number of shadow rays cast per hit fir soft shadows. By default, this is set to 1, resulting in hard shadows.
```
shadowSamples <num_samples>
```
This will enable soft shadows and set the number of shadow rays cast per hit to `num_samples`.

---
### unlit
Used to set the unlit flag. By default, this is set to false, resulting in all objects all objects using the Phong illumination model.
```
unlit
```

---
### gamma
Used to set the gamma correction. By default, no gamma correction is applied.
```
gamma <gamma>
```
This will raise all pixel rgb values to the power of 1 / `gamma`, which should be a positive floating point number (ideally in the range [1.8,2.2]).

---
### threads
Used to set the number of threads to use. By default, this is set to 1, resulting in no multithreading. If the number is set above the number of cores available, it will be set to the number of cores available.
```
threads <num_threads>
```
This will set the number of threads to use to `num_threads`.

---
### samples
Used to set the number of samples to take per pixel. By default, this is set to 1, and the image will be rendered with no anti-aliasing.
```
samples <num_samples>
```
This will set the number of samples to take per pixel to `num_samples`.

---
### bounces
Used to set max number of bounces a ray can take. By default, this is set to 1, and the image will be rendered with no reflections or refractions.
```
bounces <num_bounces>
```
This will set the max number of bounces a ray can take to `num_bounces`.

---
### ior
Used to set the initial index of refraction. By default, this is set to 1. 
```
ior <ior>
```
This would make all rays start with an index of refraction of `ior`.

---
## Comments
If you add "#" anywhere on a line in the scene file, the rest of the line will be ignored. Like this:
```
eye 0 0 10  # this is a comment
```
This would still set the eye location to (0, 0, 10), and the rest of the line would be ignored.

---
## Example Scene
Here is an example of a scene file that would render the demo scene (if you moved the demo files from oldInputs):
```
eye 0 15 -30
viewdir 0 -1 2
updir 0 1 0
vfov 45
bkgcolor 0.1 0.3 0.5 
imsize 512 512
threads 12
bvh 20

light 0 10 -2  1  1 1 1

gltf demo.gltf
```
