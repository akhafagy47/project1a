#include "Stars.h"

#include "DrawContext.h"
#include "GameContext.h"

Stars::Stars(int numStars, CMPUT350::Rect bounds) : mBounds(bounds), mGenerator(mRandomDevice()) {
    std::uniform_int_distribution<int> xRand(bounds.topLeft.x, bounds.topLeft.x + bounds.width);
    std::uniform_int_distribution<int> yRand(bounds.topLeft.y, bounds.topLeft.y + bounds.height);
    // Generate random star positions within the bounds
    for (int x = 0; x < numStars; x++) {
        mStarPositions.emplace_back(xRand(mGenerator), yRand(mGenerator));
    }
}

void Stars::RenderBackground(CMPUT350::GameContext* context) {
    CMPUT350::RGBColor c[4] = {CMPUT350::Colors::white, CMPUT350::Colors::cyan,
                               CMPUT350::Colors::magenta, CMPUT350::Colors::white};
    static int skip = 0;
    int curr = 0;
    skip++;
    for (auto& star : mStarPositions) {
        curr++;
        if ((curr + skip / 5) % (mStarPositions.size() / 20) != 0)
            context->ScreenContext->DrawCircle(star, 1.0f, c[(curr) % 4]);
        star.y += 3;
        if (star.y > mBounds.topLeft.y + mBounds.height) star.y -= mBounds.height;
    }
}
