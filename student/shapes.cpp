
#include "../rays/shapes.h"
#include "debug.h"

namespace PT {

const char *Shape_Type_Names[(int)Shape_Type::count] = {"None", "Sphere"};

BBox Sphere::bbox() const {

    BBox box;
    box.enclose(Vec3(-radius));
    box.enclose(Vec3(radius));
    return box;
}

Trace Sphere::hit(const Ray &ray) const {

    // TODO (PathTracer): Task 2
    // Intersect this ray with a sphere of radius Sphere::radius centered at the origin.

    // If the ray intersects the sphere twice, ret should
    // represent the first intersection, but remember to respect
    // ray.time_bounds! For example, if there are two intersections,
    // but only the _later_ one is within ray.time_bounds, you should
    // return that one!

    Trace ret;

    ret.hit = false;       // was there an intersection?
    ret.time = 0.0f;       // at what time did the intersection occur?
    ret.position = Vec3{}; // where was the intersection?
    ret.normal = Vec3{};   // what was the surface normal at the intersection?

    float a = dot(ray.dir, ray.dir);
    float b = 2.f*dot(ray.point, ray.dir);
    float c = dot(ray.point, ray.point)-radius*radius;

    float b2minus4ac = b*b-4.f*a*c;
    
    if(b2minus4ac>=0){
        float t1 = (- b - sqrt(b2minus4ac))/(2.f*a);
        float t2 = (- b + sqrt(b2minus4ac))/(2.f*a);
        bool t1True = (t1>ray.time_bounds[0])&&(t1<ray.time_bounds[1])&&(t1>0);
        bool t2True = (t2>ray.time_bounds[0])&&(t2<ray.time_bounds[1])&&(t2>0);
        
        if (t1True&&t2True){
            float t = std::min(t1, t2);
            ret.hit = true;
            ret.time = t;
            ret.position = ray.point+t*ray.dir;
            ret.normal = ret.position.normalize();
            return ret;
        }
        else if(t1True && !t2True){
            ret.hit = true;
            ret.time = t1;
            ret.position = ray.point+t1*ray.dir;
            ret.normal = ret.position.normalize();
            return ret;
        }
        else if(t2True && !t1True){
            ret.hit = true;
            ret.time = t1;
            ret.position = ray.point+t1*ray.dir;
            ret.normal = ret.position.normalize();
            return ret;
        }
    }

    return ret;
}
 
} // namespace PT
