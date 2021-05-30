
/*
    pbrt source code is Copyright(c) 1998-2016
                        Matt Pharr, Greg Humphreys, and Wenzel Jakob.

    This file is part of pbrt.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */


// materials/portalcrystal.cpp*
#include <shapes/linked_crystal.h>
#include "materials/portalcrystal.h"
#include "spectrum.h"
#include "reflection.h"
#include "paramset.h"
#include "texture.h"
#include "interaction.h"

namespace pbrt {

    // GlassMaterial Method Definitions
    void PortalCrystalMaterial::ComputeScatteringFunctions(SurfaceInteraction *si,
                                                   MemoryArena &arena,
                                                   TransportMode mode,
                                                   bool allowMultipleLobes) const {
    //    if (dynamic_cast<LinkedCrystal*>(si->shape)) {
    //
    //    }

        // bumpmapping removed
        // roughness removed

        Float eta = index->Evaluate(*si);

        Spectrum R = Kr->Evaluate(*si).Clamp();
        Spectrum T = Kt->Evaluate(*si).Clamp();
        // Initialize _bsdf_ for smooth or rough dielectric
        si->bsdf = ARENA_ALLOC(arena, BSDF)(*si, eta);

        if (R.IsBlack() && T.IsBlack()) return;


        // always specular, i got rid of roughness
        si->bsdf->Add(
            ARENA_ALLOC(arena, PortalFresnelSpecular)(R, T, 1.f, eta, mode));


    }

    void PortalCrystalMaterial::AddShape(const std::shared_ptr<Shape> shape) {
        if (shape1 == nullptr) {
            shape1 = shape;
        }
        else if (shape2 == nullptr) {
            shape2 = shape;
        }
    }

    bool PortalCrystalMaterial::IsAbsorby(const SurfaceInteraction *si) const {
        return si->shape == shape1.get();
    }

    bool PortalCrystalMaterial::IsEmitty(const SurfaceInteraction *si) const {
        return si->shape == shape2.get();
    }

    float PortalCrystalMaterial::Attenuation(float d) const {
        return exp(-d / attenuation_length);
    }

    void PortalCrystalMaterial::CameraFirstTransform(RayDifferential *ray) const {
        Ray worldRay = (*shape2->ObjectToWorld)(*ray);
        Ray destRay = (*shape1->WorldToObject)(worldRay);
        *ray = destRay;
    }

    void PortalCrystalMaterial::LightFirstTransform(RayDifferential *ray) const {
        Ray worldRay = (*shape1->ObjectToWorld)(*ray);
        Ray destRay = (*shape2->WorldToObject)(worldRay);
        *ray = destRay;
    }

    PortalCrystalMaterial *CreatePortalCrystalMaterial(const TextureParams &mp) {
        std::shared_ptr<Texture<Spectrum>> Kr =
            mp.GetSpectrumTexture("Kr", Spectrum(1.f));
        std::shared_ptr<Texture<Spectrum>> Kt =
            mp.GetSpectrumTexture("Kt", Spectrum(1.f));

        std::shared_ptr<Texture<Float>> index = mp.GetFloatTexture("index", 1.5f);

        Float attenuation_length = mp.FindFloat("attenuation_length", 5.0f);

        return new PortalCrystalMaterial(Kr, Kt, index, attenuation_length);
    }

}  // namespace pbrt
