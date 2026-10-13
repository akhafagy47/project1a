#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include <cctype>

namespace
{
const float player_half_width = 20.0f;
const float player_speed = 8.0f;  // Pixels per frame while a move key is held
}

Player::Player(CMPUT350::Point2D loc)
    :is_player_alive(true), shifting_left(false), shifting_right(false), requesting_fire(false), player_boundary(-20.0f, - 20.0f, 40.0f, 40.0f), player_loc(loc) 
{
    // main_body and wings
    player_shapes.emplace_back(CMPUT350::Rect(-5.0f, -20.0f, 10.0f, 40.0f));
    player_shapes.emplace_back(CMPUT350::Rect(-20.0f, -3.0f, 40.0f, 18.0f));
}

void Player::Initialize(CMPUT350::GameContext* context)
{

    if (context->mNotificationManager == nullptr)
    {
        return;
    }

    context->mNotificationManager->Register(context->CurrObject, shift_left_key);
    context->mNotificationManager->Register(context->CurrObject, shift_right_key);
    context->mNotificationManager->Register(context->CurrObject, stop_left_key);
    context->mNotificationManager->Register(context->CurrObject, stop_right_key);
    context->mNotificationManager->Register(context->CurrObject, fire_key);

}

void Player::Update(CMPUT350::GameContext* context)
{
    if (!IsAlive())
    {
        return;
    }

    if (shifting_left)
    {
        player_loc.x -= player_speed;
    }

    if (shifting_right)
    {
        player_loc.x += player_speed;
    }

    int screenWidth = context->ScreenContext->GetWindowWidth();

    //Prevents the player from exiting the left bound of the screen.
    if (player_loc.x -20.0f < 0.0f) 
    {
        player_loc.x = 20.0f;
    }

    //Prevents the player from exiting the right bound of the screen.
    if (player_loc.x + 20.0f > screenWidth)
    {
        player_loc.x = screenWidth - 20.0f;
    }


    if (requesting_fire)
    {
        requesting_fire = false;
        FireBullet(context);
    }


}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    if (is_player_alive)
    {
        CMPUT350::Rect main_body(- 5.0f, -15.0f, 10.0f, 35.00f);
        CMPUT350::Rect wings(- 20.0f, 5.0f, 40.0f, 10.0f);
        CMPUT350::Rect left_cannon(- 20.0f, - 3.0f, 8.0f, 8.0f);
        CMPUT350::Rect right_cannon(12.0f, - 3.0f, 8.0f, 8.0f);
        CMPUT350::Rect gun(- 2.0f, - 20.0f, 4.0f, 5.0f);

        context->ScreenContext->DrawRect(main_body, CMPUT350::Colors::white);
        context->ScreenContext->DrawRect(wings, CMPUT350::Colors::green);
        context->ScreenContext->DrawRect(left_cannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(right_cannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(gun, CMPUT350::Colors::red);
    }
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{

    //Makes sure bullet created by player doesnt destroy player.
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet!=nullptr && bullet->IsPlayerBullet())
    {
        return; 
    }
    Kill();
}

void Player::Kill()
{
    is_player_alive = false;
}

bool Player::IsAlive() const
{
    // TODO: Update code
    return is_player_alive;
}

const CMPUT350::Rect& Player::GetBounds()
{
    // TODO: Update code
    return player_boundary;
}
void Player::ReceiveNotification(const std::string &key)
{
    if (!IsAlive())
    {
        return;
    }

    if (key == shift_left_key)
    {
        shifting_left = true;
    }

    else if (key == stop_left_key)
    {
        shifting_left = false;
    }

    else if (key == shift_right_key)
    {
        shifting_right = true;
    }

    else if (key == stop_right_key)
    {
        shifting_right = false;
    }

    else if (key == fire_key)
    {
        requesting_fire = true;
    }
}


void Player::FireBullet(CMPUT350::GameContext* context)
{
    std::shared_ptr<Bullet> bullet_slot_1 = player_bullet_1.lock();
    std::shared_ptr<Bullet> bullet_slot_2 = player_bullet_2.lock();

    if (bullet_slot_1 && bullet_slot_1->IsAlive() && bullet_slot_2 && bullet_slot_2->IsAlive())
    {
        return;
    }

    CMPUT350::Point2D gun_end(player_loc.x, player_loc.y - 22.0f);
    CMPUT350::Point2D bullet_velocity(0.0f, -10.0f);
    std::shared_ptr<Bullet> bullet = std::make_shared<Bullet>(gun_end, bullet_velocity, true, context->CurrObject);

    if (!bullet_slot_1 || !bullet_slot_1->IsAlive())
    {
        player_bullet_1 = bullet;
    }
    else
    {
        player_bullet_2 = bullet;
    }
    context->mEngineView->AddGameObject(bullet);
}
CMPUT350::Point2D Player::GetLocation() const { return player_loc; }
float Player::GetRotation() const { return 0.0f; }

// TODO: Once collisions go through shapes, an empty list means nothing can ever hit the player. 
const std::vector<CMPUT350::Shape>& Player::GetShapes()
{
    return player_shapes;
}
