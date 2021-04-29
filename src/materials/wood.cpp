
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

    void SolidWoodMaterial::WoodBump(
            SurfaceInteraction *si) const {
        // Compute offset positions and evaluate displacement texture
        SurfaceInteraction siEval = *si;

        // Shift _siEval_ _du_ in the $u$ direction
        Float du = .5f * (std::abs(si->dudx) + std::abs(si->dudy));
        // The most common reason for du to be zero is for ray that start from
        // light sources, where no differentials are available. In this case,
        // we try to choose a small enough du so that we still get a decently
        // accurate bump value.
        if (du == 0) du = .0005f;
        siEval.p = si->p + du * si->shading.dpdu;
        siEval.uv = si->uv + Vector2f(du, 0.f);
        siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
                             du * si->dndu);
        Float uDisplace = SolidWoodValAt(siEval.p);

        // Shift _siEval_ _dv_ in the $v$ direction
        Float dv = .5f * (std::abs(si->dvdx) + std::abs(si->dvdy));
        if (dv == 0) dv = .0005f;
        siEval.p = si->p + dv * si->shading.dpdv;
        siEval.uv = si->uv + Vector2f(0.f, dv);
        siEval.n = Normalize((Normal3f)Cross(si->shading.dpdu, si->shading.dpdv) +
                             dv * si->dndv);
        Float vDisplace = SolidWoodValAt(siEval.p);
        Float displace = SolidWoodValAt(si->p);

        // Compute bump-mapped differential geometry
        Vector3f dpdu = si->shading.dpdu +
                        (uDisplace - displace) / du * Vector3f(si->shading.n) +
                        displace * Vector3f(si->shading.dndu);
        Vector3f dpdv = si->shading.dpdv +
                        (vDisplace - displace) / dv * Vector3f(si->shading.n) +
                        displace * Vector3f(si->shading.dndv);
        si->SetShadingGeometry(dpdu, dpdv, si->shading.dndu, si->shading.dndv,
                               false);
    }

    Float SolidWoodMaterial::SolidWoodValAt(Point3f p) const {

        FastNoiseLite n = FastNoiseLite();

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

        return val;
    }

    void SolidWoodMaterial::ComputeScatteringFunctions(
            SurfaceInteraction *si,
            MemoryArena &arena,
            TransportMode mode,
            bool allowMultipleLobes
    ) const {

        WoodBump(si);

        Point3f p = si->p * (1/scale);

        double val = SolidWoodValAt(p);

        Spectrum diffuse = val * color1 + (1-val)*color2;



        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si);

        double roughness_sigma = (1-val) * 30 + 10;

        si->bsdf->Add(ARENA_ALLOC(arena, OrenNayar)(diffuse, roughness_sigma));

        return;

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