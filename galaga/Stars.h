#ifndef STARS_H
#define STARS_H

#include <random>

#include "GraphicsObject.h"

class Stars : public CMPUT350::GraphicsObject {
public:
    Stars(int numStars, CMPUT350::Rect bounds);
    void RenderBackground(CMPUT350::GameContext* context) override;

    CMPUT350::Point2D GetLocation() const override { return CMPUT350::Point2D(0, 0); }
    float GetRotation() const override { return 0.0f; }

private:
    std::vector<CMPUT350::Point2D> mStarPositions;
    CMPUT350::Rect mBounds;
    std::random_device mRandomDevice;
    std::mt19937 mGenerator;
};

#endif  // STARS_H
