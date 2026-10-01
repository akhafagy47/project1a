#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include <cstdint>

#include "MathUtil.h"
#include <SFML/Graphics.hpp>

namespace CMPUT350 {

struct RGBColor {
    uint8_t r, g, b;
    RGBColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

namespace Colors {

const RGBColor red(255, 0, 0);
const RGBColor green(0, 255, 0);
const RGBColor blue(0, 0, 255);
const RGBColor yellow(255, 255, 0);
const RGBColor cyan(0, 255, 255);
const RGBColor magenta(255, 0, 255);
const RGBColor white(255, 255, 255);
const RGBColor black(0, 0, 0);
const RGBColor gray(100, 100, 100);
const RGBColor grey(200, 200, 200);

}  // namespace Colors

class DrawContext {
public:
    DrawContext(std::shared_ptr<sf::RenderWindow> mWindow, std::shared_ptr<sf::Font> font);

    /**
     * @brief Draws text aligned to the top left of the specified point.
     * @param text The string to be drawn.
     * @param pixelSize The size of characters of text in pixels.
     * @param p The top-left 2D starting coordinate for the text.
     * @param c The color of the text.
     */
    void DrawText(const std::string &text, int pixelSize, Point2D p, RGBColor c);

    /**
     * @brief Draws text centered at the specified point.
     * @param text The string that is to be drawn.
     * @param pixelSize The size of characters of text in pixels.
     * @param p The center coordinate for the text.
     * @param c The color of the text.
     */
    void DrawCenteredText(const std::string &text, int pixelSize, Point2D p, RGBColor c);

    /**
     * @brief Draws a filled circle.
     * @param p The center coordinate of the circle.
     * @param radius The radius of the circle in pixels.
     * @param c The fill color of the circle.
     */
    void DrawCircle(Point2D p, float radius, RGBColor c);

    /**
     * @brief Draws a filled axis aligned rectangle.
     * @param r The Rect bounding box defining the rectangle's position and size.
     * @param c The fill color of the rectangle.
     */
    void DrawRect(Rect r, RGBColor c);

    /**
     * @brief Draws an outlined rectangle frame.
     * @param r The Rect bounding box defining the rectangle's position and size.
     * @param width The thickness of the outline border in pixels.
     * @param c The outline color of the rectangle.
     */
    void FrameRect(Rect r, float width, RGBColor c);

    /**
     * @brief Draws a line between two points with a specified width and color.
     *
     * @param from The starting point of the line (Point2D).
     * @param to The ending point of the line (Point2D).
     * @param width The width of the line in pixels.
     * @param c The color of the line, specified as an RGBColor object.
     *
     * This  calculates the distance and angle between the two points
     * and uses a rectangle to represent the line. The line is drawn
     * relative to the world offset and rendered onto the associated window.
     */
    void DrawLine(Point2D from, Point2D to, float width, RGBColor c);
    
    /**
     * @brief Gets the width of the rendering window.
     * @return The window width in pixels.
     */
    int GetWindowWidth();

    /**
     * @brief Gets the height of the rendering window.
     * @return The window height in pixels.
     */
    int GetWindowHeight();

private:
    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
};

}  // namespace CMPUT350

#endif  // RENDERTARGET_H
