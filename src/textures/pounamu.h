/*
   A Custom PBRT Texture to represent Pounamu or New Zealand Greenstone in
   PBRT For CGRA408 Assignment 2 Victoria University of Wellington Author -
   James Drown
*/

#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_TEXTURES_POUNAMU_H
#define PBRT_TEXTURES_POUNAMU_H

#include "paramset.h"
#include "pbrt.h"
#include "texture.h"

namespace pbrt {

class PounamuTexture : public Texture<Spectrum> {
  public:
    // ConstantTexture Public Methods
    PounamuTexture(const Spectrum &bColour,
                   std::unique_ptr<TextureMapping3D> mapping, int octaves,
                   Float omega, Float scale, Float variation)
        : baseColour(bColour),
          mapping(std::move(mapping)),
          octaves(octaves),
          omega(omega),
          scale(scale),
          variation(variation){}

    Spectrum Evaluate(const SurfaceInteraction &si) const {
        // Set the base colour
        //Float baseRGB[3] = {baseColour.x, baseColour.y, baseColour.z};

        // Follow marble concept to get vain
        Vector3f dpdx, dpdy;
        Point3f p = mapping->Map(si, &dpdx, &dpdy);
        p *= scale;

        // Fractional brownian motion function
        Float veinY =  p.y + variation * FBm(p, scale * dpdx, scale * dpdy,
                                             omega, octaves);
		Float veinX =  p.x + variation * FBm(p, scale * dpdx, scale * dpdy,
                                             omega, octaves);
        Float t = .5f + std::min(.5f * std::sin(veinY), .5f * std::sin(veinX));
        //Float t = .5f + std::sin(veinY);
        //Float t = .5f + std::sin(veinX);

        // Evaluate vein spline at _t_ (magic numbers from marble function)
        static Float c[][3] = {
            {.58f, .58f, .6f}, {.58f, .58f, .6f}, {.58f, .58f, .6f},
            {.5f, .5f, .5f},   {.6f, .59f, .58f}, {.58f, .58f, .6f},
            {.58f, .58f, .6f}, {.2f, .2f, .33f},  {.58f, .58f, .6f},
        };

#define NC sizeof(c) / sizeof(c[0])
#define NSEG (NC -3)
        int first = std::min(1, int(std::floor(t * NSEG)));
        t = (t * NSEG - first);
        Spectrum c0 = Spectrum::FromRGB(c[first]);
        Spectrum c1 = Spectrum::FromRGB(c[first + 1]);
        Spectrum c2 = Spectrum::FromRGB(c[first + 2]);
        Spectrum c3 = Spectrum::FromRGB(c[first + 3]);
        // Bezier spline evaluated with de Castilejau's algorithm
        Spectrum s0 = (1.f - t) * c0 + t * c1;
        Spectrum s1 = (1.f - t) * c1 + t * c2;
        Spectrum s2 = (1.f - t) * c2 + t * c3;
        s0 = (1.f - t) * s0 + t * s1;
        s1 = (1.f - t) * s1 + t * s2;
        return baseColour * ((1.f - t) * s0 + t * s1);
    }

  private:
    Spectrum baseColour;
    std::unique_ptr<TextureMapping3D> mapping;
    const int octaves;
    const Float omega, scale, variation;
};

Texture<Float> *CreatePounamuFloatTexture(const Transform &tex2world,
                                          const TextureParams &tp);
PounamuTexture *CreatePounamuSpectrumTexture(const Transform &tex2world,
                                             const TextureParams &tp);

}  // namespace pbrt

#endif