#include "DrawContext.h"
#include <cmath>

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
    sfml_text.setPosition(sf::Vector2f(p.x, p.y));
    mWindow->draw(sfml_text);
}


void DrawContext::DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c)
{
    sf::Text sfml_text(*mFont, text);
    sfml_text.setCharacterSize(pixelSize);
    sfml_text.setPosition(sf::Vector2f(p.x, p.y));
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
    circle.setPosition(sf::Vector2f(p.x, p.y));
    mWindow->draw(circle);
}


void DrawContext::DrawRect(Rect r, RGBColor c)
{
    sf::RectangleShape rect(sf::Vector2f(r.width, r.height));
    rect.setFillColor(sf::Color(c.r, c.g, c.b));
    rect.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
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
    rect.setPosition(sf::Vector2f(r.topLeft.x, r.topLeft.y));
    mWindow->draw(rect);
}


void DrawContext::DrawLine(Point2D from, Point2D to, float width, RGBColor c)
{
    float x_diff = to.x - from.x;
    float y_diff = to.y - from.y;
    float length = std::sqrt((x_diff*x_diff) + (y_diff*y_diff));

    sf::RectangleShape line(sf::Vector2f(length, width));

    //Sets origin to the center of the line to allow for rotation
    line.setOrigin(sf::Vector2f(length/2.0f, width/2.0f));
    
    float mid_x = (from.x + to.x)/2.0f;
    float mid_y = (from.y + to.y)/2.0f;
    line.setPosition(sf::Vector2f(mid_x, mid_y));

    float radians = std::atan2(y_diff, x_diff);
    line.setRotation(sf::radians(radians));
    line.setFillColor(sf::Color(c.r, c.g, c.b));
    mWindow->draw(line);
}


int DrawContext::GetWindowWidth() { return mWindow->getSize().x; }

int DrawContext::GetWindowHeight() { return mWindow->getSize().y; }

}  // namespace CMPUT350
