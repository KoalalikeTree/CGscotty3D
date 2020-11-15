
#include "../rays/tri_mesh.h"
#include "debug.h"

namespace PT {

BBox Triangle::bbox() const {

    // TODO (PathTracer): Task 2
    // compute the bounding box of the triangle
      
    BBox box;

    Vec3 pos0 = vertex_list[v0].position;
    Vec3 pos1 = vertex_list[v1].position;
    Vec3 pos2 = vertex_list[v2].position;

    //expand the box to include the points
    box.enclose(pos0);
    box.enclose(pos1);
    box.enclose(pos2);

    return box;
}

Trace Triangle::hit(const Ray &ray) const {

    // Vertices of triangle - has postion and surface normal
    Tri_Mesh_Vert v_0 = vertex_list[v0];
    Tri_Mesh_Vert v_1 = vertex_list[v1];
    Tri_Mesh_Vert v_2 = vertex_list[v2];
    (void)v_0;
    (void)v_1;
    (void)v_2;
    
    Vec3 e1 = v_1.position - v_0.position;
    Vec3 e2 = v_2.position - v_0.position;
    Vec3 s = ray.point-v_0.position;
    
    double denominator = dot(cross(e1, ray.dir), e2);
    
    
    // TODO (PathTracer): Task 2
    // Intersect this ray with a triangle defined by the three above points.

    Trace ret;
    ret.hit = false;       // was there an intersection?
    ret.time = 0.0f;       // at what time did the intersection occur?
    ret.position = Vec3{}; // where was the intersection?
    ret.normal = Vec3{};   // what was the surface normal at the intersection?
                           // (this should be interpolated between the three vertex normals)

    

    // if the demonimator is zero, then there is no intersection
    if (denominator != 0){
        float scale = (float)(1.0/denominator);
        // u = -(S X e2) `d
        float u = scale * dot(cross(s,e2) *(-1.0), ray.dir);

        // v = (e1 X d) `s
        float v  = scale *  dot(cross(e1,ray.dir), s);

        //t = -(s X e2) `e1
        float t = scale * dot(cross(s,e2)*(-1.0), e1);

        //three number is between[0,1], is meaningful barycentric coordinate
        bool uTrue = (u >= 0) && (u <= 1);
        bool vTrue = (v >= 0) && (v <= 1);
        bool uvTrue = ((1-u-v) >= 0) && ((1-u-v) <= 1);
        bool tTrue =  (t>ray.time_bounds[0]) && (t<ray.time_bounds[1]);

        if (uTrue && vTrue && uvTrue && tTrue){
            ret.hit = true;
            ret.time = t;
            ret.position = (1-u-v)*v_0.position + u*v_1.position + v*v_2.position;
            ret.normal = (1-u-v)*v_0.normal + u*v_1.normal + v*v_2.normal;
            
            // is the normal face the ray?
            float cross_product_ori_norm = dot(ret.normal, ray.dir);
            float cross_product_inv_norm = dot(-ret.normal, ray.dir);
            // when the cross product is smaller, it means the angle between the ray direction and norm is bigger
            // which means the norm is facing the ray
            if (cross_product_inv_norm < cross_product_ori_norm){
                ret.normal = -ret.normal;
            }

            
        }
    }

    return ret;
}

Triangle::Triangle(Tri_Mesh_Vert *verts, unsigned int v0, unsigned int v1, unsigned int v2)
    : vertex_list(verts), v0(v0), v1(v1), v2(v2) {}

void Tri_Mesh::build(const GL::Mesh &mesh) {

    verts.clear();
    triangles.clear();

    for (const auto &v : mesh.verts()) {
        verts.push_back({v.pos, v.norm});
    }

    const auto &idxs = mesh.indices();

    std::vector<Triangle> tris;
    for (size_t i = 0; i < idxs.size(); i += 3) {
        tris.push_back(Triangle(verts.data(), idxs[i], idxs[i + 1], idxs[i + 2]));
    }

    triangles.build(std::move(tris), 4);
}

Tri_Mesh::Tri_Mesh(const GL::Mesh &mesh) { build(mesh); }

BBox Tri_Mesh::bbox() const { return triangles.bbox(); }

Trace Tri_Mesh::hit(const Ray &ray) const { return triangles.hit(ray); }

size_t Tri_Mesh::visualize(GL::Lines &lines, GL::Lines &active, size_t level,
                           const Mat4 &trans) const {
    return triangles.visualize(lines, active, level, trans);
}

} // namespace PT
