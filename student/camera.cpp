
#include "../util/camera.h"
#include "debug.h"
#include <iostream>

Ray Camera::generate_ray(Vec2 screen_coord) const {

    // TODO (PathTracer): Task 1
    // compute position of the input sensor sample coordinate on the
    // canonical sensor plane one unit away from the pinhole.
    // Tip: compute the ray direction in view space and use
    // the camera transform to transform it back into world space.
    float pixelScreenX  = screen_coord[0];
    float pixelScreenY = screen_coord[1];

    float v_fov = get_fov();
    
    //std::cout<<"v_fov = "<< v_fov <<std::endl;
    //std::cout<<"aspect ratio = "<< get_ar() <<std::endl;

    float height = tan(Radians(v_fov/2));
    float width = height*get_ar();

    float pixelCameraY = (1-2*pixelScreenY)*height;
    float pixelCameraX = (2*pixelScreenX-1)*width;

     //std::cout<<"height = "<< height <<std::endl; 
    //std::cout<<"width = "<< width <<std::endl;

    // the parameter screen coordinate is start from the left up corner
    // but the camera coordinate is start from the center of the image, so we need -width/2 -height/2
    Vec3 camera_dir(pixelCameraX, -pixelCameraY, -1.0f);
    camera_dir = camera_dir.normalize();

    Vec3 world_dir = iview * camera_dir-pos();
    world_dir = world_dir.normalize();

    return Ray(pos(), world_dir);
}
 