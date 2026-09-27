#include <cassert>
#include "Player.h"
#include "Bullet.h"
#include <cctype>

Player::Player(CMPUT350::Point2D loc)
    :player_loc(loc), player_boundary(loc.x -20.0f, loc.y - 20.0f, 40.0f, 40.0f), is_player_alive(true)
{
    // TODO: Update code
}

void Player::Initialize(CMPUT350::GameContext* context)
{
}

void Player::Update(CMPUT350::GameContext* context)
{
    if (!IsAlive())
    {
        return;
    }

    int screenWidth = context->ScreenContext->GetWindowWidth();

    if (player_loc.x -20.0f < 0.0f) 
    {
        player_loc.x = 20.0f;
    }

    if (player_loc.x + 20.0f > screenWidth)
    {
        player_loc.x = screenWidth - 20.0f;
    }

    player_boundary = CMPUT350::Rect(player_loc.x - 20.0f, player_loc.y - 20.0f, 40.0f, 40.0f);


}

void Player::LateUpdate(CMPUT350::GameContext* context)
{
}

bool Player::HandleKeyEvent(CMPUT350::GameContext* context, char key)
{
    if (!IsAlive())
    {
        return false;
    }
    
    char normalized_key = std::tolower(key);
    if (normalized_key == 'a')
    {
        player_loc.x -= 20.0f;
        return true;
    }

    if (normalized_key == 'd')
    {
        player_loc.x += 20.0f;
        return true;
    }

    if (key == ' ')
    {
        CMPUT350::Point2D heading(0.0f, -10.0f);

        if (player_bullet_1.expired())
        {
            std::shared_ptr<Bullet> bullet1 = std::make_shared<Bullet>(player_loc, heading, true);
            player_bullet_1 = bullet1; 
            context->mEngineView->AddGameObject(bullet1); 
            return true;
        }
        else if (player_bullet_2.expired())
        {
            std::shared_ptr<Bullet> bullet2 = std::make_shared<Bullet>(player_loc, heading, true);
            player_bullet_2 = bullet2; 
            context->mEngineView->AddGameObject(bullet2); 
            return true;
        }
    }
    return false;


}

void Player::RenderBackground(CMPUT350::GameContext* context)
{
}

void Player::RenderForeground(CMPUT350::GameContext* context)
{
    if (is_player_alive)
    {
        CMPUT350::Rect main_body(player_loc.x - 5.0f, player_loc.y  -15.0f, 10.0f, 35.00f);
        CMPUT350::Rect wings(player_loc.x - 20.0f, player_loc.y + 5.0f, 40.0f, 10.0f);
        CMPUT350::Rect left_cannon(player_loc.x - 20.0f, player_loc.y - 3.0f, 8.0f, 8.0f);
        CMPUT350::Rect right_cannon(player_loc.x + 12.0f, player_loc.y - 3.0f, 8.0f, 8.0f);
        CMPUT350::Rect gun(player_loc.x - 2.0f, player_loc.y - 20.0f, 4.0f, 5.0f);

        context->ScreenContext->DrawRect(main_body, CMPUT350::Colors::white);
        context->ScreenContext->DrawRect(wings, CMPUT350::Colors::green);
        context->ScreenContext->DrawRect(left_cannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(right_cannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(gun, CMPUT350::Colors::red);
    }
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
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
