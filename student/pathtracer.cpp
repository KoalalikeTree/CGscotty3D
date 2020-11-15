
#include "../rays/pathtracer.h"
#include "../rays/samplers.h"
#include "../util/rand.h"
#include "debug.h"
#include <iostream>

namespace PT {

Spectrum Pathtracer::trace_pixel(size_t x, size_t y) {

    Vec2 xy((float)x, (float)y);
    Vec2 wh((float)out_w, (float)out_h);

    // TODO (PathTracer): Task 1

    // Generate a sample within the pixel with coordinates xy and return the
    // incoming light using trace_ray.

    // Tip: Samplers::Rect::Uniform
    // Tip: you may want to use log_ray for debugging

    // This currently generates a ray at the bottom left of the pixel every time.
    Spectrum s;

    float pdf = 1.0f;
    Samplers::Rect::Uniform myUni = Samplers::Rect::Uniform();
    

    for (int i=0; i<n_samples; i++){
        auto sample = myUni.sample(pdf); 

        float x_screen = (x+sample.x)/ out_w;
        float y_screen = (y+sample.y)/ out_h;

        Ray out = camera.generate_ray(Vec2(x_screen, y_screen));
        out.depth = max_depth;
        s += trace_ray(out);
        log_ray(out, 10.0f);
    }
    
    s *= (float)(1.0f / n_samples);
    return s;
}

Spectrum Pathtracer::trace_ray(const Ray &ray) {
    // Trace ray into scene. If nothing is hit, sample the environment
    Trace hit = scene.hit(ray);
    if (!hit.hit) {
        if (env_light.has_value()) {
            return env_light.value().sample_direction(ray.dir);
        }
        return {};
    }

    // Set up a coordinate frame at the hit point, where the surface normal becomes {0, 1, 0}
    // This gives us out_dir and later in_dir in object space, where computations involving the
    // normal become much easier. For example, cos(theta) = dot(N,dir) = dir.y!
    Mat4 object_to_world = Mat4::rotate_to(hit.normal);
    Mat4 world_to_object = object_to_world.T();
    Vec3 out_dir = world_to_object.rotate(ray.point - hit.position).unit();
    const BSDF &bsdf = materials[hit.material];

    // Now we can compute the rendering equation at this point.
    // We split it into two stages: sampling lighting (i.e. directly connecting
    // the current path to each light in the scene), then sampling the BSDF
    // to create a new path segment.

    // TODO (PathTracer): Task 5
    // Instead of initializing this value to a constant color, use the direct,
    // indirect lighting components calculated in the code below. The starter
    // code sets radiance_out to (0.5,0.5,0.5) so that you can test your geometry
    // queries before you implement path tracing.
    Spectrum radiance_out =
        debug_data.normal_colors ? Spectrum(0.1f) : Spectrum::direction(hit.normal);
    {
        auto sample_light = [&](const auto &light) {
            // If the light is discrete (e.g. a point light), then we only need
            // one sample, as all samples will be equivalent
            int samples = light.is_discrete() ? 1 : (int)n_area_samples;
            for (int i = 0; i < samples; i++) {

                Light_Sample sample = light.sample(hit.position);
                Vec3 in_dir = world_to_object.rotate(sample.direction);

                // If the light is below the horizon, ignore it
                float cos_theta = in_dir.y;
                if (cos_theta <= 0.0f)
                    continue;

                // If the BSDF has 0 throughput in this direction, ignore it
                // This is another oppritunity to do Russian roulette on low-throughput rays,
                // which would allow us to skip the shadow ray cast, increasing efficiency.
                Spectrum absorbsion = bsdf.evaluate(out_dir, in_dir);
                if (absorbsion.luma() == 0.0f)
                    continue;

                // TODO (PathTracer): Task 4
                // Construct a shadow ray and compute whether the intersected surface is
                // in shadow. Only accumulate light if not in shadow.

                // Tip: when making your ray, you will want to slightly offset it from the
                // surface it starts on, lest it intersect at time=0. Similarly, you may want
                // to limit the ray slightly before it would hit the light itself.

                // Note: that along with the typical cos_theta, pdf factors, we divide by samples.
                // This is because we're  doing another monte-carlo estimate of the lighting from
                // area lights.
                Ray shadowRay(hit.position + EPS_F * sample.direction, sample.direction);
				shadowRay.time_bounds[1] = sample.distance / sample.direction.norm() - EPS_F;
				if (!scene.hit(shadowRay).hit){
                    radiance_out += (cos_theta / (samples * sample.pdf)) * sample.radiance * absorbsion;
                }
                    
            }
        };

        // If the BSDF is discrete (i.e. uses dirac deltas/if statements), then we are never
        // going to hit the exact right direction by sampling lights, so ignore them.
        if (!bsdf.is_discrete()) {
            for (const auto &light : lights)
                sample_light(light);
            if (env_light.has_value())
                sample_light(env_light.value());
        }
    }


    return radiance_out;
    //// TODO (PathTracer): Task 5
    //// Compute an indirect lighting estimate using pathtracing with Monte Carlo.

   
    //// (1) Ray objects have a depth field; you should use this to avoid
    //// traveling down one path forever.
    //if (ray.depth > max_depth){
    //    return radiance_out;
    //}

    //// (2) randomly select a new ray direction (it may be reflection or transmittence
    //// ray depending on surface type) using bsdf.sample()
    //
    //BSDF_Sample f = bsdf.sample(out_dir);


    //// (3) potentially terminate path (using Russian roulette). You can make this
    //// a function of the bsdf attenuation or track overall ray throughput
    //float termProb = 1.0f - f.attenuation.luma();

    //if ((double)rand() / RAND_MAX < termProb){
    //    return radiance_out;
    //}

    //// (4) create new scene-space ray and cast it to get incoming light
    //Vec3 newray_dir = object_to_world * f.direction;
    //newray_dir.normalize();
    //Ray newRay(hit.position + EPS_F * newray_dir, newray_dir);
    //double cos_theta = f.direction.z;

    //// (5) add contribution due to incoming light with proper weighting

    //float pdf_term = 1/(f.pdf * (1-termProb));

    //return radiance_out+ f.attenuation * trace_ray(newRay) * (float)abs(cos_theta) * pdf_term;
    
}

} // namespace PT
