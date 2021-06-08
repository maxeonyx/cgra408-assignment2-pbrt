#include "textures/pounamu.h"

namespace pbrt {

Texture<Float> *CreatePounamuFloatTexture(const Transform &tex2world,
                                                 const TextureParams &tp) {
    return nullptr;
}

PounamuTexture *CreatePounamuSpectrumTexture(
    const Transform &tex2world, const TextureParams &tp) {
	    
	std::unique_ptr<TextureMapping3D> map(new IdentityMapping3D(tex2world));

	return new PounamuTexture(
            tp.FindSpectrum("baseColour", Spectrum(1)),
						std::move(map),
						tp.FindInt("octaves", 8),
						tp.FindFloat("roughness", .5f),
						tp.FindFloat("scale", 1.f),
						tp.FindFloat("variation", .2f));
}

}  // namespace pbrt
