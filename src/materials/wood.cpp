
#include <core/geometry.h>
#include <iostream>
#include "wood.h"
#include "materials/subsurface.h"
#include "textures/constant.h"
#include "spectrum.h"
#include "texture.h"
#include "interpolation.h"
#include "paramset.h"
#include "interaction.h"
#include "FastNoiseLite.h"

namespace pbrt {

    void SolidWoodMaterial::ComputeScatteringFunctions(
            SurfaceInteraction *si,
            MemoryArena &arena,
            TransportMode mode,
            bool allowMultipleLobes
    ) const {

        Point3f p = si->p;

        FastNoiseLite n = FastNoiseLite();

        double val1 = n.GetNoise(p.x*100,p.y*100,p.z*10)*0.5 + 0.5;
        double val2 = n.GetNoise(p.x*100 + 11,p.y*100 + 11,p.z*10 + 11)*0.5 + 0.5;
        double warp_x = p.x+val1*0.3;
        double warp_y = p.y+val2*0.3;
        double warp_dist = std::sqrt(warp_x*warp_x + warp_y*warp_y);
        double big_dist = warp_dist * 10;
        double fine_dist = warp_dist * 60;
        double small_rings = std::sin(fine_dist) * 0.5 + 0.5;
        double big_rings = std::sin(big_dist) * 0.5 + 0.5;

        double sawtooth = -std::cos(big_dist - std::cos(big_dist - std::cos(big_dist - std::cos(big_dist)))) * 0.5 + 0.5;
        double val = 0.3 * small_rings + 0.7 * sawtooth;

        float color1[3] = {(float)0xba/0xff, (float)0x3e/0xff, (float)0x12/0xff};
        RGBSpectrum color1_s = RGBSpectrum::FromRGB(color1);
        float color2[3] = {(float)0x63/0xff, (float)0x23/0xff, (float)0x0b/0xff};
        RGBSpectrum color2_s = RGBSpectrum::FromRGB(color2);

        Spectrum color = val * color1_s + (1-val)*color2_s;

        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

        si->bsdf->Add(ARENA_ALLOC(arena, LambertianReflection)(color));

        // 3d cylinder (with rotation angle?)

        // saw-tooth gradient function by distance

        // perlin noise

        // use perlin noise for coordinate distortion

        // fractal perlin distortion

        // multiple wood ring levels (large and small)

        // material differences by part of the wood ring.

    }

    SolidWoodMaterial *CreateSolidWoodMaterial(const TextureParams &mp) {
        return new SolidWoodMaterial();
    }

}