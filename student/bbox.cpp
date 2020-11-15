
#include "../lib/mathlib.h"
#include "debug.h"

bool BBox::hit(const Ray &ray, Vec2 &times) const {

    // TODO (PathTracer):
    // Implement ray - bounding box intersection test
    // If the ray intersected the bounding box within the range given by
    // [times.x,times.y], update times with the new intersection times.

    Vec2 y_time;
    Vec3 z_time;

    // hit point in 3 dim
    times.x = (min.x-ray.point.x)*(1.0f/ray.dir.x);
    times.y = (max.x-ray.point.x)*(1.0f/ray.dir.x);
    if (times.x > times.y) std::swap(times.x,times.y);

    y_time.x = (min.y-ray.point.y)*(1.0f/ray.dir.y);
    y_time.y = (max.y-ray.point.y)*(1.0f/ray.dir.y);
    if (y_time.x > y_time.y) std::swap(y_time.x,y_time.y); 

    z_time.x = (min.z-ray.point.z)*(1.0f/ray.dir.z);
    z_time.y = (max.z-ray.point.z)*(1.0f/ray.dir.z);
    if (z_time.x > z_time.y) std::swap(z_time.x,z_time.y);

    // compare x y 
    if ((times.x > y_time.y) || (y_time.x > times.y)) 
        return false; 
    
    if (y_time.x > times.x){
        times.x = y_time.x;
    }
 
    if (y_time.y < times.y){
        times.y = y_time.y;
    }

    // compare x y z
    if ((times.x > z_time.y) || (z_time.x > times.y)){
        return false;
    }
 
    if (z_time.x > times.x) 
        times.x = z_time.x; 
 
    if (z_time.y < times.y) 
        times.y = z_time.y; 

    if (times.x < ray.time_bounds[0] || times.y>ray.time_bounds[1]){
        times.x = ray.time_bounds[0];
        times.y = ray.time_bounds[1];
        return false;
    }

    return true;
}
