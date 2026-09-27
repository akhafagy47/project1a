#include "Bullet.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Point2D heading, bool player)
    :current_bullet_loc(location), bullet_heading(heading), shot_by_player(player),
    is_bullet_alive(true), bullet_boundary(location.x, location.y, 2.0f, 2.0f), previous_bullet_loc(location)
{
}

bool Bullet::IsPlayerBullet()
{
    // TODO: Update
    return shot_by_player;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{

    
    previous_bullet_loc = current_bullet_loc;
    current_bullet_loc += bullet_heading;

    float bullet_boundary_top = std::min(current_bullet_loc.y, previous_bullet_loc.y);
    float bullet_boundary_left = std::min(current_bullet_loc.x, previous_bullet_loc.x);
    float bullet_boundary_width = std::max(2.0f, std::abs(current_bullet_loc.x - previous_bullet_loc.x));
    float bullet_boundary_height = std::max(2.0f, std::abs(current_bullet_loc.y - previous_bullet_loc.y));


    int screenHeight = context->ScreenContext->GetWindowHeight();
    int screenWidth = context->ScreenContext->GetWindowWidth();

    //if bullet is out of screen;
    if (current_bullet_loc.x<0.0f || current_bullet_loc.x>screenWidth || current_bullet_loc.y<0.0f || current_bullet_loc.y>screenHeight) 
    {
        Kill();
    }

    bullet_boundary = CMPUT350::Rect(bullet_boundary_left, bullet_boundary_top, bullet_boundary_width, bullet_boundary_height);
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Bullet::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    if (IsAlive())
    {
        context->ScreenContext->DrawLine(previous_bullet_loc, current_bullet_loc, 2.0f, CMPUT350::Colors::red);
    }
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    Kill();
}

void Bullet::Kill()
{
    is_bullet_alive = false;
}

bool Bullet::IsAlive() const
{
    // TODO: Update code
    
    return is_bullet_alive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    // TODO: Update code
    return bullet_boundary;
}
