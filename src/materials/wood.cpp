
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

        Point3f p = si->p * (1/scale);

        FastNoiseLite n = FastNoiseLite();

        // TODO: random darkness amount by distance

        double val1 = n.GetNoise(p.x*100,p.y*100,p.z*10)*0.5 + 0.5;
        double val2 = n.GetNoise(p.x*100 + 11,p.y*100 + 11,p.z*10 + 11)*0.5 + 0.5;
        double warp_x = p.x+val1*0.3;
        double warp_y = p.y+val2*0.3;
        double warp_dist = std::sqrt(warp_x*warp_x + warp_y*warp_y);
        double big_dist = warp_dist * 10;
        double fine_dist = warp_dist * 60;
        double small_rings = std::sin(fine_dist) * 0.5 + 0.5;

        double sawtooth = -std::cos(big_dist - std::cos(big_dist - std::cos(big_dist - std::cos(big_dist)))) * 0.5 + 0.5;
        double val = 0.3 * small_rings + 0.7 * sawtooth;

        Spectrum diffuse = val * color1 + (1-val)*color2;

        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

        Spectrum specular = Ks->Evaluate(*si).Clamp();
        Float roughu = nu->Evaluate(*si);
        Float roughv = nv->Evaluate(*si);

        if (!diffuse.IsBlack() || !specular.IsBlack()) {
            if (remapRoughness) {
                roughu = TrowbridgeReitzDistribution::RoughnessToAlpha(roughu);
                roughv = TrowbridgeReitzDistribution::RoughnessToAlpha(roughv);
            }
            MicrofacetDistribution *distrib =
                    ARENA_ALLOC(arena, TrowbridgeReitzDistribution)(roughu, roughv);
            si->bsdf->Add(ARENA_ALLOC(arena, FresnelBlend)(diffuse, specular, distrib));
        }

        // 3d cylinder (with rotation angle?)

        // saw-tooth gradient function by distance

        // perlin noise

        // use perlin noise for coordinate distortion

        // fractal perlin distortion

        // multiple wood ring levels (large and small)

        // material differences by part of the wood ring.

    }

    SolidWoodMaterial *CreateSolidWoodMaterial(const TextureParams &mp) {
        Float scale = mp.FindFloat("scale", 1);
        float color1array[] = {0.729, 0.243, 0.070};
        float color2array[] = {0.2716, 0.096, 0.03};
        Spectrum color1 = mp.FindSpectrum("color1", RGBSpectrum::FromRGB(color1array));
        Spectrum color2 = mp.FindSpectrum("color2", RGBSpectrum::FromRGB(color2array));
        std::shared_ptr<Texture<Spectrum>> Ks =
                                                   mp.GetSpectrumTexture("Ks", Spectrum(.5f));
        std::shared_ptr<Texture<Float>> uroughness =
                                                mp.GetFloatTexture("uroughness", .1f);
        std::shared_ptr<Texture<Float>> vroughness =
                                                mp.GetFloatTexture("vroughness", .1f);
        std::shared_ptr<Texture<Float>> bumpMap =
                                                mp.GetFloatTextureOrNull("bumpmap");
        bool remapRoughness = mp.FindBool("remaproughness", true);
        return new SolidWoodMaterial(scale, color1, color2, Ks, uroughness, vroughness, bumpMap,
                                     remapRoughness);
    }

}