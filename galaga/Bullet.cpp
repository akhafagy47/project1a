#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Vector2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent)
    : is_bullet_alive(true), shot_by_player(player), has_hit(false),
      current_bullet_loc(location), previous_bullet_loc(location), bullet_heading(heading),
      hit_point(location), bullet_parent(parent), bullet_boundary(location.x, location.y, 2.0f, 2.0f), bullet_shapes()
{
    UpdateCollisionData();
}

bool Bullet::IsPlayerBullet()
{
    return shot_by_player;
}

void Bullet::Initialize(CMPUT350::GameContext* context)
{
}

void Bullet::Update(CMPUT350::GameContext* context)
{

    if (has_hit)
    {
        return;
    }
    
    previous_bullet_loc = current_bullet_loc;
    current_bullet_loc += bullet_heading;

    std::shared_ptr<CMPUT350::GameObject> parent = bullet_parent.lock();
    if (parent == nullptr || !parent->IsAlive())
    {
        Kill();
    }

    int screenHeight = context->ScreenContext->GetWindowHeight();
    int screenWidth = context->ScreenContext->GetWindowWidth();

    //if bullet is out of screen kill it;
    if (current_bullet_loc.x<0.0f || current_bullet_loc.x>screenWidth || current_bullet_loc.y<0.0f || current_bullet_loc.y>screenHeight) 
    {
        Kill();
    }

    UpdateCollisionData();
}

void Bullet::LateUpdate(CMPUT350::GameContext* context)
{
}

void Bullet::RenderBackground(CMPUT350::GameContext* context)
{
}

void Bullet::RenderForeground(CMPUT350::GameContext* context)
{
    
    CMPUT350::RGBColor bullet_color = CMPUT350::Colors::red;
    if (!shot_by_player)
    {
        bullet_color = CMPUT350::Colors::yellow;
    }

    if (has_hit)
    {
        context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.0f, 0.0f), 5.0f, CMPUT350::Colors::magenta);
    }

    else if (IsAlive())
    {
        context->ScreenContext->DrawLine(previous_bullet_loc - current_bullet_loc,CMPUT350::Point2D(0.0f,0.0f) ,2.0f, bullet_color);
    }
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    CollisionEnter(obj, CMPUT350::Point2D(0.0f, 0.0f));
}


void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj, const CMPUT350::Point2D& collisionPoint)
{
    // Bullets pass through other bullets and dont kill each other.
    if (std::dynamic_pointer_cast<Bullet>(obj) != nullptr)
    {
        return; 
    }

    // Makes sure player's own bullets dont kill it when created.
    if (shot_by_player && std::dynamic_pointer_cast<Player>(obj) != nullptr)
    {
        return; 
    }

    if (!shot_by_player && std::dynamic_pointer_cast<Enemy>(obj) != nullptr)
    {
        return; 
    }

    CMPUT350::Point2D contact = current_bullet_loc + collisionPoint;

    if (!has_hit || contact.y > hit_point.y)
    {
        hit_point = contact;
    }
    
    has_hit = true;
    current_bullet_loc = hit_point;
    Kill();
}

void Bullet::Kill()
{
    is_bullet_alive = false;
}

bool Bullet::IsAlive() const
{
    
    return is_bullet_alive;
}

const CMPUT350::Rect& Bullet::GetBounds()
{
    return bullet_boundary;
}
CMPUT350::Point2D Bullet::GetLocation() const { return current_bullet_loc; }
float Bullet::GetRotation() const
{
    return 0.0f;
}

const std::vector<CMPUT350::Shape>& Bullet::GetShapes() {
    return bullet_shapes;
}


void Bullet::UpdateCollisionData()
{
    bullet_shapes.clear();
    bullet_shapes.emplace_back(CMPUT350::Line(previous_bullet_loc - current_bullet_loc, CMPUT350::Point2D(0.0f,0.0f)));
    bullet_boundary = CMPUT350::Rect(previous_bullet_loc - current_bullet_loc, CMPUT350::Point2D(0.0f,0.0f));
}