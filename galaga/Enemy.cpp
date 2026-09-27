#include "Enemy.h"
#include "Bullet.h"

Enemy::Enemy(CMPUT350::Point2D loc)
:enemy_loc(loc), is_enemy_alive(true), enemy_boundary(loc.x-20.f, loc.y-20.f, 40.0f, 40.f)
{
    // TODO: Update code
    //Based on what we saw online, the size of both enemy and player are 16x16,
    //But because the player is set to 40x40, we have set the player to 40x40 as well
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
}

void Enemy::Update(CMPUT350::GameContext* context)
{
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Enemy::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    return false;
    //being ignored for now;
}

void Enemy::RenderBackground(CMPUT350::GameContext* context)
{
}

void Enemy::RenderForeground(CMPUT350::GameContext* context)
{
    if (IsAlive())
    {
        context->ScreenContext->DrawRect(enemy_boundary, CMPUT350::Colors::blue);
    }
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && !bullet->IsPlayerBullet())
    {
        return; 
    }
    Kill();
}

void Enemy::Kill()
{
    is_enemy_alive = false;
}

bool Enemy::IsAlive() const
{
    // TODO: Update code
    return is_enemy_alive;
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    // TODO: Update code
    return enemy_boundary;
}
