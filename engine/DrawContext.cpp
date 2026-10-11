#include "DrawContext.h"
#include <cmath>
#include <algorithm>

namespace CMPUT350 {

DrawContext::DrawContext(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<sf::Font> font)
    : mWindow(window), mFont(font) {}


void DrawContext::DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c)
{
    sf::Text sfml_text(*mFont, text);
    sfml_text.setCharacterSize(pixelSize);
    sfml_text.setFillColor(sf::Color(c.r, c.g, c.b));

    sf::FloatRect text_outline = sfml_text.getLocalBounds();

    float center_x = text_outline.position.x + (text_outline.size.x/2.0f);
    float center_y = text_outline.position.y + (text_outline.size.y/2.0f);
    //Shifts the text origin to its calculated center so it aligns at point p
    sfml_text.setOrigin(sf::Vector2f(center_x, center_y));
    Point2D localToWorldPoint = Transform(p);
    sfml_text.setPosition(sf::Vector2f(localToWorldPoint.x, localToWorldPoint.y));
    sfml_text.setRotation(sf::radians(mRotation));
    mWindow->draw(sfml_text);
}


void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c)
{
    sf::Text sfml_text(*mFont, text);
    sfml_text.setCharacterSize(pixelSize);
    Point2D localToWorldPoint = Transform(p);
    sfml_text.setPosition(sf::Vector2f(localToWorldPoint.x, localToWorldPoint.y));
    sfml_text.setRotation(sf::radians(mRotation));
    sfml_text.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(sfml_text);

}


void DrawContext::DrawCircle(Point2D p, float radius, RGBColor c)
{
    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setFillColor(sf::Color(c.r, c.g, c.b));
    //Sets the  distance to draw from the center instead of top left
    circle.setOrigin(sf::Vector2f(radius, radius));
    Point2D worldPoint = Transform(p);
    circle.setPosition(sf::Vector2f(worldPoint.x, worldPoint.y));
    mWindow->draw(circle);
}


void DrawContext::DrawRect(Rect r, RGBColor c)
{
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    rect.setFillColor(sf::Color(c.r, c.g, c.b));
    Point2D worldPoint = Transform(r.topLeft);
    rect.setPosition(sf::Vector2f(worldPoint.x, worldPoint.y));
    rect.setRotation(sf::radians(mRotation));
    mWindow->draw(rect);
}


void DrawContext::FrameRect(Rect r, float width, RGBColor c)
{
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color(c.r, c.g, c.b));

    //Setting outline thickness to a negative num causes the border to draw inward from the boundary instead of outward.
    //used this as reference: https://www.sfml-dev.org/documentation/3.1.0/classsf_1_1Shape.html
    rect.setOutlineThickness(-width);
    Point2D worldPoint = Transform(r.topLeft);
    rect.setPosition(sf::Vector2f(worldPoint.x, worldPoint.y));
    rect.setRotation(sf::radians(mRotation));
    mWindow->draw(rect);
}


void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c)
{
    Point2D worldPointFrom = Transform(from);
    Point2D worldPointTo = Transform(to);
    
    float x_diff = worldPointTo.x - worldPointFrom.x;
    float y_diff = worldPointTo.y - worldPointFrom.y;
    float length = std::sqrt((x_diff*x_diff) + (y_diff*y_diff));

    sf::RectangleShape line(sf::Vector2f(length, width));

    //Sets origin to the center of the line to allow for rotation
    line.setOrigin(sf::Vector2f(length/2.0f, width/2.0f));
    
    float mid_x = (worldPointFrom.x + worldPointTo.x)/2.0f;
    float mid_y = (worldPointFrom.y + worldPointTo.y)/2.0f;
    line.setPosition(sf::Vector2f(mid_x, mid_y));

    float radians = std::atan2(y_diff, x_diff);
    line.setRotation(sf::radians(radians));
    line.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(line);
}


int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }


void DrawContext::SetContextOffset(Point2D p) {
    mOffset = p;
}

void DrawContext::SetContextRotation(float rotation) {
    mCos = std::cos(rotation);
    mSin = std::sin(rotation);
    mRotation = rotation;
}

Point2D DrawContext::Transform(Point2D p) {
    float rotX = p.x * mCos - p.y * mSin;
    float rotY = p.x * mSin + p.y * mCos;
    return Point2D(rotX + mOffset.x, rotY + mOffset.y);
}

Point2D DrawContext::ReverseTransform(Point2D p) {
    // 1. Reverse translation
    float reverseOffsetX = p.x - mOffset.x;
    float reverseOffsetY = p.y - mOffset.y;
    // 2. Reverse rotation (transpose/inverse of rotation matrix)
    float reverseRotationX = reverseOffsetX * mCos + reverseOffsetY * mSin;
    float reverseRotationY = -reverseOffsetX * mSin + reverseOffsetY * mCos;
    return Point2D(reverseRotationX, reverseRotationY);
}

Rect DrawContext::Transform(Rect r) {
    // Transform all 4 corners of the rect to find the new axis-aligned bounding box
    Point2D rectTopLeft = Transform(r.topLeft);
    Point2D rectTopRight = Transform(Point2D(r.topLeft.x + r.width, r.topLeft.y));
    Point2D rectBottomLeft = Transform(Point2D(r.topLeft.x, r.topLeft.y + r.height));
    Point2D rectBottomRight = Transform(Point2D(r.topLeft.x + r.width, r.topLeft.y + r.height));

    float minX = std::min({rectTopLeft.x, rectTopRight.x, rectBottomLeft.x, rectBottomRight.x});
    float minY = std::min({rectTopLeft.y, rectTopRight.y, rectBottomLeft.y, rectBottomRight.y});
    float maxX = std::max({rectTopLeft.x, rectTopRight.x, rectBottomLeft.x, rectBottomRight.x});
    float maxY = std::max({rectTopLeft.y, rectTopRight.y, rectBottomLeft.y, rectBottomRight.y});

    return Rect(minX, minY, maxX - minX, maxY - minY);
}
}  // namespace CMPUT350

