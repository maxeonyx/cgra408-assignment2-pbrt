
#include "pbrt.h"
#include "material.h"

namespace pbrt {

class SolidWoodMaterial : public Material {
  public:
    SolidWoodMaterial(
        // const std::shared_ptr<Texture<Spectrum>> &Kd,
        // const std::shared_ptr<Texture<Spectrum>> &Ks,
        // const std::shared_ptr<Texture<Spectrum>> &Kr,
        // const std::shared_ptr<Texture<Spectrum>> &Kt,
        // const std::shared_ptr<Texture<Float>> &roughness,
        // const std::shared_ptr<Texture<Float>> &roughnessu,
        // const std::shared_ptr<Texture<Float>> &roughnessv,
        // const std::shared_ptr<Texture<Spectrum>> &opacity,
        // const std::shared_ptr<Texture<Float>> &eta,
        // const std::shared_ptr<Texture<Float>> &bumpMap,
        // bool remapRoughness
        )
        // :
        // Kd(Kd),
        //   Ks(Ks),
        //   Kr(Kr),
        //   Kt(Kt),
        //   opacity(opacity),
        //   roughness(roughness),
        //   roughnessu(roughnessu),
        //   roughnessv(roughnessv),
        //   eta(eta),
        //   bumpMap(bumpMap),
        //   remapRoughness(remapRoughness)
          {}


    void ComputeScatteringFunctions(SurfaceInteraction *si,
                                            MemoryArena &arena,
                                            TransportMode mode,
                                            bool allowMultipleLobes) const;

};

SolidWoodMaterial *CreateSolidWoodMaterial(const TextureParams &mp);

}