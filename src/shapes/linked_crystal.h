//
// Created by clarkemaxw on 28/05/21.
//

#ifndef PBRT_V3_LINKED_CRYSTAL_H
#define PBRT_V3_LINKED_CRYSTAL_H

#include "shape.h"

namespace pbrt {

    class LinkedCrystal : public Shape {
    public:
        // Cylinder Public Methods
        LinkedCrystal(const Transform *ObjectToWorld, const Transform *WorldToObject,
                 bool reverseOrientation, Float radius, Float zMin, Float zMax,
                 Float phiMax)
                : Shape(ObjectToWorld, WorldToObject, reverseOrientation),
                  radius(radius),
                  zMin(std::min(zMin, zMax)),
                  zMax(std::max(zMin, zMax)),
                  phiMax(Radians(Clamp(phiMax, 0, 360))) {}
        Bounds3f ObjectBound() const;
        bool Intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect,
                       bool testAlphaTexture) const;
        bool IntersectP(const Ray &ray, bool testAlphaTexture) const;
        Float Area() const;
        Interaction Sample(const Point2f &u, Float *pdf) const;

    protected:
        // Cylinder Private Data
        const Float radius, zMin, zMax, phiMax;
    };

    std::shared_ptr<LinkedCrystal> CreateLinkedCrystalShape(std::vector<std::shared_ptr<Shape>> plyMesh1,
                                                            std::vector<std::shared_ptr<Shape>> plyMesh2,
                                                            const ParamSet &params);
}


#endif //PBRT_V3_LINKED_CRYSTAL_H
