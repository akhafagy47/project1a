#include "Bullet.h"

#include "Enemy.h"
#include "Player.h"

Bullet::Bullet(CMPUT350::Point2D location, CMPUT350::Vector2D heading, bool player,
               std::weak_ptr<CMPUT350::GameObject> parent)
    : mIsAlive(true),
      mShotByPlayer(player),
      mHasHit(false),
      mLocation(location),
      mPreviousLocation(location),
      mHeading(heading),
      mBounds(location.x, location.y, 2.0f, 2.0f),
      mHitPoint(location),
      mParent(parent),
      mShapes() {
    UpdateCollisionData();
}

bool Bullet::IsPlayerBullet() { return mShotByPlayer; }

void Bullet::Initialize(CMPUT350::GameContext*) {}

void Bullet::Update(CMPUT350::GameContext* context) {
    if (mHasHit) {
        return;
    }

    mPreviousLocation = mLocation;
    mLocation += mHeading;

    std::shared_ptr<CMPUT350::GameObject> parent = mParent.lock();
    if (parent == nullptr || !parent->IsAlive()) {
        Kill();
    }

    int screenHeight = context->ScreenContext->GetWindowHeight();
    int screenWidth = context->ScreenContext->GetWindowWidth();

    // if bullet is out of screen kill it;
    if (mLocation.x < 0.0f || mLocation.x > screenWidth || mLocation.y < 0.0f ||
        mLocation.y > screenHeight) {
        Kill();
    }

    UpdateCollisionData();
}

void Bullet::LateUpdate(CMPUT350::GameContext*) {}

void Bullet::RenderBackground(CMPUT350::GameContext*) {}

void Bullet::RenderForeground(CMPUT350::GameContext* context) {
    CMPUT350::RGBColor bulletColor = CMPUT350::Colors::red;
    if (!mShotByPlayer) {
        bulletColor = CMPUT350::Colors::yellow;
    }

    if (mHasHit) {
        context->ScreenContext->DrawCircle(CMPUT350::Point2D(0.0f, 0.0f), 5.0f,
                                           CMPUT350::Colors::magenta);
    }

    else if (IsAlive()) {
        context->ScreenContext->DrawLine(mPreviousLocation - mLocation,
                                         CMPUT350::Point2D(0.0f, 0.0f), 2.0f, bulletColor);
    }
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) {
    CollisionEnter(obj, CMPUT350::Point2D(0.0f, 0.0f));
}

void Bullet::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                            const CMPUT350::Point2D& collisionPoint) {
    // Bullets pass through other bullets and dont kill each other.
    if (std::dynamic_pointer_cast<Bullet>(obj) != nullptr) {
        return;
    }

    // Makes sure player's own bullets dont kill it when created.
    if (mShotByPlayer && std::dynamic_pointer_cast<Player>(obj) != nullptr) {
        return;
    }

    if (!mShotByPlayer && std::dynamic_pointer_cast<Enemy>(obj) != nullptr) {
        return;
    }

    CMPUT350::Point2D contact = mLocation + collisionPoint;

    if (!mHasHit || contact.y > mHitPoint.y) {
        mHitPoint = contact;
    }

    mHasHit = true;
    mLocation = mHitPoint;
    Kill();
}

void Bullet::Kill() { mIsAlive = false; }

bool Bullet::IsAlive() const { return mIsAlive; }

const CMPUT350::Rect& Bullet::GetBounds() { return mBounds; }
CMPUT350::Point2D Bullet::GetLocation() const { return mLocation; }
float Bullet::GetRotation() const { return 0.0f; }

const std::vector<CMPUT350::Shape>& Bullet::GetShapes() { return mShapes; }

void Bullet::UpdateCollisionData() {
    mShapes.clear();
    mShapes.emplace_back(
        CMPUT350::Line(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f)));
    mBounds = CMPUT350::Rect(mPreviousLocation - mLocation, CMPUT350::Point2D(0.0f, 0.0f));
}