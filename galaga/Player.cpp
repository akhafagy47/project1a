#include "Player.h"

#include <cassert>
#include <cctype>

#include "Bullet.h"

namespace {
const float kPlayerHalfWidth = 20.0f;
const float kPlayerSpeed = 8.0f;  // Pixels per frame while a move key is held
}  // namespace

Player::Player(CMPUT350::Point2D loc)
    : mIsAlive(true),
      mShiftingLeft(false),
      mShiftingRight(false),
      mRequestingFire(false),
      mBounds(-20.0f, -20.0f, 40.0f, 40.0f),
      mLocation(loc) {
    // mainBody and wings
    mShapes.emplace_back(CMPUT350::Rect(-5.0f, -20.0f, 10.0f, 40.0f));
    mShapes.emplace_back(CMPUT350::Rect(-20.0f, -3.0f, 40.0f, 18.0f));
}

void Player::Initialize(CMPUT350::GameContext* context) {
    if (context->mNotificationManager == nullptr) {
        return;
    }

    context->mNotificationManager->Register(context->CurrObject, kShiftLeftKey);
    context->mNotificationManager->Register(context->CurrObject, kShiftRightKey);
    context->mNotificationManager->Register(context->CurrObject, kStopLeftKey);
    context->mNotificationManager->Register(context->CurrObject, kStopRightKey);
    context->mNotificationManager->Register(context->CurrObject, kFireKey);
}

void Player::Update(CMPUT350::GameContext* context) {
    if (!IsAlive()) {
        return;
    }

    if (mShiftingLeft) {
        mLocation.x -= kPlayerSpeed;
    }

    if (mShiftingRight) {
        mLocation.x += kPlayerSpeed;
    }

    int screenWidth = context->ScreenContext->GetWindowWidth();

    // Prevents the player from exiting the left bound of the screen.
    if (mLocation.x - kPlayerHalfWidth < 0.0f) {
        mLocation.x = kPlayerHalfWidth;
    }

    // Prevents the player from exiting the right bound of the screen.
    if (mLocation.x + kPlayerHalfWidth > screenWidth) {
        mLocation.x = screenWidth - kPlayerHalfWidth;
    }

    if (mRequestingFire) {
        mRequestingFire = false;
        FireBullet(context);
    }
}

void Player::LateUpdate(CMPUT350::GameContext*) {}

void Player::RenderBackground(CMPUT350::GameContext*) {}

void Player::RenderForeground(CMPUT350::GameContext* context) {
    if (mIsAlive) {
        CMPUT350::Rect mainBody(-5.0f, -15.0f, 10.0f, 35.00f);
        CMPUT350::Rect wings(-20.0f, 5.0f, 40.0f, 10.0f);
        CMPUT350::Rect leftCannon(-20.0f, -3.0f, 8.0f, 8.0f);
        CMPUT350::Rect rightCannon(12.0f, -3.0f, 8.0f, 8.0f);
        CMPUT350::Rect gun(-2.0f, -20.0f, 4.0f, 5.0f);

        context->ScreenContext->DrawRect(mainBody, CMPUT350::Colors::white);
        context->ScreenContext->DrawRect(wings, CMPUT350::Colors::green);
        context->ScreenContext->DrawRect(leftCannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(rightCannon, CMPUT350::Colors::red);
        context->ScreenContext->DrawRect(gun, CMPUT350::Colors::red);
    }
}

void Player::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    // Makes sure bullet created by player doesnt destroy player.
    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && bullet->IsPlayerBullet()) {
        return;
    }
    Kill();
}

void Player::Kill() { mIsAlive = false; }

bool Player::IsAlive() const { return mIsAlive; }

const CMPUT350::Rect& Player::GetBounds() { return mBounds; }
void Player::ReceiveNotification(const std::string& key) {
    if (!IsAlive()) {
        return;
    }

    if (key == kShiftLeftKey) {
        mShiftingLeft = true;
    }

    else if (key == kStopLeftKey) {
        mShiftingLeft = false;
    }

    else if (key == kShiftRightKey) {
        mShiftingRight = true;
    }

    else if (key == kStopRightKey) {
        mShiftingRight = false;
    }

    else if (key == kFireKey) {
        mRequestingFire = true;
    }
}

void Player::FireBullet(CMPUT350::GameContext* context) {
    std::shared_ptr<Bullet> bulletSlot1 = mBullet1.lock();
    std::shared_ptr<Bullet> bulletSlot2 = mBullet2.lock();

    if (bulletSlot1 && bulletSlot1->IsAlive() && bulletSlot2 && bulletSlot2->IsAlive()) {
        return;
    }

    CMPUT350::Point2D gunEnd(mLocation.x, mLocation.y - 22.0f);
    CMPUT350::Vector2D bulletVelocity(0.0f, -10.0f);
    std::shared_ptr<Bullet> bullet =
        std::make_shared<Bullet>(gunEnd, bulletVelocity, true, context->CurrObject);

    if (!bulletSlot1 || !bulletSlot1->IsAlive()) {
        mBullet1 = bullet;
    } else {
        mBullet2 = bullet;
    }
    context->mEngineView->AddGameObject(bullet);
}
CMPUT350::Point2D Player::GetLocation() const { return mLocation; }
float Player::GetRotation() const { return 0.0f; }

const std::vector<CMPUT350::Shape>& Player::GetShapes() { return mShapes; }
