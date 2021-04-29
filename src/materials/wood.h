
#include <core/spectrum.h>
#include "pbrt.h"
#include "material.h"
#include "FastNoiseLite.h"

namespace pbrt {

class SolidWoodMaterial : public Material {
  public:
    SolidWoodMaterial(
            Float scale,
            Spectrum color1,
            Spectrum color2,
            const std::shared_ptr<Texture<Spectrum>> &Ks,
         const std::shared_ptr<Texture<Float>> &nu,
         const std::shared_ptr<Texture<Float>> &nv,
         const std::shared_ptr<Texture<Float>> &bumpMap,
            bool remapRoughness
        )
         :
        color1(color1),
        color2(color2),
         scale(scale),
           Ks(Ks),
           nu(nu),
           nv(nv),
           bumpMap(bumpMap),
           remapRoughness(remapRoughness)
          {
          }

    virtual Float SolidWoodValAt(Point3f p) const;
    virtual void WoodBump(
                          SurfaceInteraction *si) const;

    void ComputeScatteringFunctions(SurfaceInteraction *si,
                                            MemoryArena &arena,
                                            TransportMode mode,
                                            bool allowMultipleLobes) const override;
private:
    Float scale;
    Spectrum color1, color2;
    // SubstrateMaterial Private Data
    std::shared_ptr<Texture<Spectrum>> Ks;
    std::shared_ptr<Texture<Float>> nu, nv;
    std::shared_ptr<Texture<Float>> bumpMap;
    bool remapRoughness;
};

SolidWoodMaterial *CreateSolidWoodMaterial(const TextureParams &mp);

}