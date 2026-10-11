#include "Enemy.h"

#include <algorithm>
#include <cmath>

#include "Bezier.h"
#include "Bullet.h"
#include "Player.h"

namespace
{
const float kPi = 3.14159265f;
const float kCollisionHalfSize = 16.0f;    // Collision box is 32x32, a little inside the drawing
const int kSamplesPerSegment = 100;        // Arc-length samples per cubic segment (~6 px apart)
const float kHoverAmplitude = 20.0f;       // Pixels either side of the formation slot
const float kHoverRate = 2.0f * kPi / 120.0f;  // Radians per frame: one sway every 4 seconds
const float kFormationTurnRate = 0.2f;     // Radians per frame when turning to face down
const float kBulletSpeed = 9.0f;
const float kArcHandle = 0.5523f;          // Cubic Bezier handle length for a quarter circle

/**
 * @brief Returns v scaled to length 1, or (0, 0) if v has no length.
 */
CMPUT350::Point2D Unit(CMPUT350::Point2D v)
{
    v.Normalize();
    return v;
}
}  // namespace


// Enemy (base class)


Enemy::Enemy(CMPUT350::Point2D home, std::vector<CMPUT350::Point2D> entryPath)
    : mLocation(entryPath.empty() ? home : entryPath.front()),
      mHeading(0.0f, 1.0f),
      mHome(home),
      mEntryPath(std::move(entryPath)),
      mShapes{CMPUT350::Rect(-kCollisionHalfSize, -kCollisionHalfSize, 2 * kCollisionHalfSize,
                             2 * kCollisionHalfSize)}
{
}

void Enemy::Initialize(CMPUT350::GameContext* context)
{
    mState = State::Entering;
    StartPath(mEntryPath);
    if (mPath.empty())
    {
        return;  // No valid entry path: Update() places the enemy straight in formation
    }

    // The entry path ends at the home slot, but the formation hovers. Shift the path's last two
    // control points (which keeps its final direction) to where the slot will be on arrival.
    // Arrival time depends on the path length, so refine the estimate a few times.
    for (int i = 0; i < 3; i++)
    {
        int framesToArrive = static_cast<int>(std::ceil(mSampleLength.back() / mSpeed));
        CMPUT350::Point2D shift = HoverPosition(context->currentFrame + framesToArrive) - mHome;
        std::vector<CMPUT350::Point2D> path = mEntryPath;
        path[path.size() - 1] += shift;
        path[path.size() - 2] += shift;
        StartPath(std::move(path));
    }
}

void Enemy::Update(CMPUT350::GameContext* context)
{
    if (mState == State::InFormation)
    {
        mLocation = HoverPosition(context->currentFrame);
        mHeading = CMPUT350::Point2D(0.0f, 1.0f);
        // Turn back to nose-down gradually instead of snapping after arriving
        float rotation = std::remainder(mRotation, 2.0f * kPi);
        mRotation = std::fabs(rotation) <= kFormationTurnRate
                        ? 0.0f
                        : rotation - std::copysign(kFormationTurnRate, rotation);
        return;
    }

    bool finished = AdvanceAlongPath();
    if (mState == State::Attacking)
    {
        FireIfScheduled(context);
    }
    if (!finished)
    {
        return;
    }

    switch (mState)
    {
        case State::Entering:
            mState = State::InFormation;
            mLocation = HoverPosition(context->currentFrame);
            break;
        case State::Attacking:
            OnAttackPathFinished();
            mState = State::Returning;
            StartJoinFormation(context);
            break;
        case State::Returning:
            mState = State::InFormation;
            mLocation = HoverPosition(context->currentFrame);
            break;
        case State::InFormation:
            break;
    }
}

void Enemy::LateUpdate(CMPUT350::GameContext* context)
{
    if (!IsAlive() || mPendingHits == 0)
    {
        return;
    }

    mHealth -= mPendingHits;
    mPendingHits = 0;
    if (mHealth > 0)
    {
        return;
    }

    // Enemies out of formation (entering, attacking or returning) are worth more points, so say
    // which one it was
    bool inFormation = mState == State::InFormation;
    if (context->mNotificationManager != nullptr)
    {
        context->mNotificationManager->Notify(
            TypeName() + (inFormation ? "KilledInFormation" : "KilledOutOfFormation"));
    }
    Kill();
}

void Enemy::CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj)
{
    if (std::dynamic_pointer_cast<Player>(obj) != nullptr)
    {
        mPendingHits += mHealth;  // Crashing into the player always destroys the enemy
        return;
    }

    std::shared_ptr<Bullet> bullet = std::dynamic_pointer_cast<Bullet>(obj);
    if (bullet != nullptr && bullet->IsPlayerBullet())
    {
        mPendingHits++;
    }
    // Anything else (enemy bullets, other enemies) is ignored
}

const CMPUT350::Rect& Enemy::GetBounds()
{
    // Bounding box of the collision square after rotation
    float c = std::fabs(std::cos(mRotation));
    float s = std::fabs(std::sin(mRotation));
    float halfWidth = kCollisionHalfSize * (c + s);
    float halfHeight = kCollisionHalfSize * (s + c);
    mBounds = CMPUT350::Rect(mLocation.x - halfWidth, mLocation.y - halfHeight, 2 * halfWidth,
                             2 * halfHeight);
    return mBounds;
}

const std::vector<CMPUT350::Shape>& Enemy::GetShapes() { return mShapes; }

CMPUT350::Point2D Enemy::GetLocation() const { return mLocation; }

float Enemy::GetRotation() const { return mRotation; }

bool Enemy::IsInFormation() const { return mState == State::InFormation; }

void Enemy::OnAttackPathFinished() {}

void Enemy::StartAttack(std::vector<CMPUT350::Point2D> path, std::vector<float> fireAt,
                        CMPUT350::Point2D target)
{
    mState = State::Attacking;
    mTarget = target;
    mFireAt = std::move(fireAt);
    mNextFire = 0;
    StartPath(std::move(path));
}

void Enemy::SetHealth(int health)
{
    mHealth = health;
    mMaxHealth = health;
}

int Enemy::Health() const { return mHealth; }

int Enemy::MaxHealth() const { return mMaxHealth; }

CMPUT350::Point2D Enemy::ScreenSize(CMPUT350::GameContext* context)
{
    if (context == nullptr || context->ScreenContext == nullptr)
    {
        return CMPUT350::Point2D(768.0f, 1024.0f);
    }
    return CMPUT350::Point2D(static_cast<float>(context->ScreenContext->GetWindowWidth()),
                             static_cast<float>(context->ScreenContext->GetWindowHeight()));
}

/**
 * @brief Makes the given control points the current path and builds its arc-length table.
 *
 * The curve is sampled kSamplesPerSegment times per cubic segment, and the cumulative length at
 * each sample is stored, so a distance along the curve can later be turned back into a t value.
 * A path that isn't 3k+1 points (k >= 1) is left empty, which AdvanceAlongPath() treats as
 * already finished.
 * @param path Bezier control points.
 */
void Enemy::StartPath(std::vector<CMPUT350::Point2D> path)
{
    mPath.clear();
    mSampleT.clear();
    mSampleLength.clear();
    mDistance = 0.0f;
    if (path.size() < 4 || (path.size() - 1) % 3 != 0)
    {
        return;
    }

    mPath = std::move(path);
    int segments = static_cast<int>(mPath.size() - 1) / 3;
    int samples = segments * kSamplesPerSegment;
    CMPUT350::Point2D previous = CMPUT350::Bezier::GetPoint(mPath, 0.0f);
    float length = 0.0f;
    mSampleT.push_back(0.0f);
    mSampleLength.push_back(0.0f);
    for (int i = 1; i <= samples; i++)
    {
        float t = static_cast<float>(segments) * i / samples;
        CMPUT350::Point2D point = CMPUT350::Bezier::GetPoint(mPath, t);
        length += static_cast<float>(point.Distance(previous));
        mSampleT.push_back(t);
        mSampleLength.push_back(length);
        previous = point;
    }
}

/**
 * @brief Moves mSpeed pixels further along the current path, so speed stays constant no matter
 * how the control points are spaced. Updates location, heading and rotation.
 *
 * The distance travelled is converted to t by binary searching the arc-length table and
 * interpolating linearly between the two surrounding samples.
 * @return true if the end of the path has been reached (or there is no path).
 */
bool Enemy::AdvanceAlongPath()
{
    if (mPath.empty())
    {
        return true;
    }

    float total = mSampleLength.back();
    mDistance += mSpeed;
    bool finished = mDistance >= total;

    float t = mSampleT.back();
    if (!finished)
    {
        // First sample whose cumulative length is past mDistance; mDistance lies before it
        auto upper = std::upper_bound(mSampleLength.begin(), mSampleLength.end(), mDistance);
        size_t i = static_cast<size_t>(upper - mSampleLength.begin());
        float span = mSampleLength[i] - mSampleLength[i - 1];
        float fraction = span > 0.0f ? (mDistance - mSampleLength[i - 1]) / span : 0.0f;
        t = mSampleT[i - 1] + fraction * (mSampleT[i] - mSampleT[i - 1]);
    }

    mLocation = CMPUT350::Bezier::GetPoint(mPath, t);
    CMPUT350::Point2D slope = CMPUT350::Bezier::GetSlope(mPath, t);
    if (slope.x != 0.0f || slope.y != 0.0f)
    {
        mHeading = Unit(slope);
        // The enemy is drawn nose-down (+y), which is angle pi/2, so subtract that
        mRotation = std::atan2(mHeading.y, mHeading.x) - kPi / 2.0f;
    }

    if (finished)
    {
        mPath.clear();
    }
    return finished;
}

/**
 * @brief Starts a short path from the current location into the formation slot.
 *
 * The formation hovers, so the path aims at where the slot will be when the enemy arrives: the
 * arrival time is estimated from the path length, and the path is rebuilt with the improved
 * estimate a few times. The curve leaves along the current heading and enters the slot from the
 * side the enemy is coming from (from above if it is above the slot, from below otherwise), so
 * it never has to turn around right at the end.
 * @param context Pointer to global GameContext (for the current frame).
 */
void Enemy::StartJoinFormation(CMPUT350::GameContext* context)
{
    CMPUT350::Point2D start = mLocation;
    CMPUT350::Point2D target = HoverPosition(context->currentFrame);
    for (int i = 0; i < 3; i++)
    {
        // Shorter handles for short hops, so a nearby slot doesn't cause a loop
        float handle = std::min(80.0f, static_cast<float>(start.Distance(target)) * 0.5f);
        float approach = start.y < target.y ? -handle : handle;
        StartPath({start, start + mHeading * handle, target + CMPUT350::Point2D(0.0f, approach),
                   target});
        if (mPath.empty())
        {
            return;
        }
        int framesToArrive = static_cast<int>(std::ceil(mSampleLength.back() / mSpeed));
        target = HoverPosition(context->currentFrame + framesToArrive);
    }
}

/**
 * @brief Fires a bullet at the attack target each time the enemy passes one of the scheduled
 * points of its attack path. Bullets are only fired downward, toward the player.
 * @param context Pointer to global GameContext (for adding the bullet and its parent).
 */
void Enemy::FireIfScheduled(CMPUT350::GameContext* context)
{
    if (mSampleLength.empty())
    {
        return;
    }
    float total = mSampleLength.back();
    while (mNextFire < mFireAt.size() && mDistance >= mFireAt[mNextFire] * total)
    {
        mNextFire++;
        CMPUT350::Point2D toTarget = mTarget - mLocation;
        if (toTarget.y > 0.0f)
        {
            CMPUT350::Point2D heading = Unit(toTarget) * kBulletSpeed;
            context->mEngineView->AddGameObject(
                std::make_shared<Bullet>(mLocation, heading, false, context->CurrObject));
        }
    }
}

/**
 * @brief Where the formation slot is on a given frame. All enemies use the shared frame
 * counter, so the whole formation sways together.
 * @param frame The game frame.
 * @return The slot position on that frame.
 */
CMPUT350::Point2D Enemy::HoverPosition(int frame) const
{
    return CMPUT350::Point2D(mHome.x + kHoverAmplitude * std::sin(frame * kHoverRate), mHome.y);
}


// Enemy1: bee


void Enemy1::Initialize(CMPUT350::GameContext* context)
{
    SetHealth(1);
    Enemy::Initialize(context);
}

bool Enemy1::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target)
{
    if (!IsInFormation())
    {
        return false;
    }

    // Hop up and over to the target's x, then dive straight down and off the bottom
    CMPUT350::Point2D start = mLocation;
    float height = ScreenSize(context).y;
    float diveTop = start.y + 120.0f;
    StartAttack({start, start + CMPUT350::Point2D(0.0f, -70.0f),
                 CMPUT350::Point2D(target.x, start.y - 40.0f),
                 CMPUT350::Point2D(target.x, diveTop),
                 CMPUT350::Point2D(target.x, diveTop + (height - diveTop) / 3.0f),
                 CMPUT350::Point2D(target.x, diveTop + 2.0f * (height - diveTop) / 3.0f),
                 CMPUT350::Point2D(target.x, height + 40.0f)},
                {0.30f, 0.45f}, target);
    return true;
}

void Enemy1::OnAttackPathFinished()
{
    // Re-enter from just above the top of the screen, heading down
    mLocation = CMPUT350::Point2D(mLocation.x, -40.0f);
    mHeading = CMPUT350::Point2D(0.0f, 1.0f);
}

void Enemy1::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* draw = context->ScreenContext;
    draw->DrawRect(CMPUT350::Rect(-18.0f, -8.0f, 10.0f, 12.0f), CMPUT350::Colors::blue);  // Wings
    draw->DrawRect(CMPUT350::Rect(8.0f, -8.0f, 10.0f, 12.0f), CMPUT350::Colors::blue);
    draw->DrawCircle(CMPUT350::Point2D(0.0f, 0.0f), 10.0f, Color());                        // Body
    draw->DrawLine(CMPUT350::Point2D(0.0f, 8.0f), CMPUT350::Point2D(0.0f, 18.0f), 3.0f,
                   CMPUT350::Colors::red);                                                  // Stinger
}

CMPUT350::RGBColor Enemy1::Color() const { return CMPUT350::Colors::yellow; }

std::string Enemy1::TypeName() const { return "Enemy1"; }


// Enemy2: butterfly


void Enemy2::Initialize(CMPUT350::GameContext* context)
{
    SetHealth(1);
    Enemy::Initialize(context);
}

bool Enemy2::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target)
{
    if (!IsInFormation())
    {
        return false;
    }

    // Three S-curves, swinging alternately left and right of the line toward the target and
    // stopping above it
    CMPUT350::Point2D start = mLocation;
    CMPUT350::Point2D end(target.x, target.y - 140.0f);
    const float swing = 140.0f;
    std::vector<CMPUT350::Point2D> path{start};
    CMPUT350::Point2D previous = start;
    for (int k = 1; k <= 3; k++)
    {
        CMPUT350::Point2D next = start + (end - start) * (k / 3.0f);
        float side = (k % 2 == 1) ? swing : -swing;
        float drop = (next.y - previous.y) / 3.0f;
        path.push_back(previous + CMPUT350::Point2D(side, drop));
        path.push_back(next + CMPUT350::Point2D(side, -drop));
        path.push_back(next);
        previous = next;
    }
    StartAttack(path, {0.33f, 0.66f, 0.95f}, target);
    return true;
}

void Enemy2::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* draw = context->ScreenContext;
    draw->DrawCircle(CMPUT350::Point2D(-11.0f, -2.0f), 9.0f, Color());                 // Wings
    draw->DrawCircle(CMPUT350::Point2D(11.0f, -2.0f), 9.0f, Color());
    draw->DrawRect(CMPUT350::Rect(-4.0f, -14.0f, 8.0f, 28.0f), CMPUT350::Colors::white);  // Body
    draw->DrawCircle(CMPUT350::Point2D(0.0f, 12.0f), 4.0f, CMPUT350::Colors::blue);    // Head
}

CMPUT350::RGBColor Enemy2::Color() const { return CMPUT350::Colors::red; }

std::string Enemy2::TypeName() const { return "Enemy2"; }


// Enemy3: boss


void Enemy3::Initialize(CMPUT350::GameContext* context)
{
    SetHealth(2);
    Enemy::Initialize(context);
}

bool Enemy3::Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target)
{
    if (!IsInFormation())
    {
        return false;
    }

    // Swoop down to the top of a circle around the target, fly one full clockwise loop, and
    // finish back at the top. The circle is kept on screen.
    const float radius = 150.0f;
    float height = ScreenSize(context).y;
    CMPUT350::Point2D center(target.x, std::min(target.y, height - radius - 30.0f));
    CMPUT350::Point2D start = mLocation;
    CMPUT350::Point2D top = center + CMPUT350::Point2D(0.0f, -radius);

    std::vector<CMPUT350::Point2D> path{start, start + CMPUT350::Point2D(0.0f, 120.0f),
                                        top - CMPUT350::Point2D(radius * kArcHandle, 0.0f), top};
    // Four quarter arcs, clockwise on screen: top -> right -> bottom -> left -> top
    for (int quarter = 0; quarter < 4; quarter++)
    {
        float a0 = -kPi / 2.0f + quarter * kPi / 2.0f;
        float a1 = a0 + kPi / 2.0f;
        CMPUT350::Point2D p0 = center + CMPUT350::Point2D(std::cos(a0), std::sin(a0)) * radius;
        CMPUT350::Point2D p1 = center + CMPUT350::Point2D(std::cos(a1), std::sin(a1)) * radius;
        CMPUT350::Point2D tangent0(-std::sin(a0), std::cos(a0));
        CMPUT350::Point2D tangent1(-std::sin(a1), std::cos(a1));
        path.push_back(p0 + tangent0 * (radius * kArcHandle));
        path.push_back(p1 - tangent1 * (radius * kArcHandle));
        path.push_back(p1);
    }
    StartAttack(path, {0.25f, 0.45f, 0.65f}, target);
    return true;
}

void Enemy3::RenderForeground(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* draw = context->ScreenContext;
    CMPUT350::RGBColor body = Color();
    draw->DrawRect(CMPUT350::Rect(-20.0f, -6.0f, 6.0f, 16.0f), body);  // Wings
    draw->DrawRect(CMPUT350::Rect(14.0f, -6.0f, 6.0f, 16.0f), body);
    draw->DrawRect(CMPUT350::Rect(-14.0f, -14.0f, 28.0f, 24.0f), body);  // Body
    draw->DrawCircle(CMPUT350::Point2D(-6.0f, 4.0f), 3.0f, CMPUT350::Colors::yellow);  // Eyes
    draw->DrawCircle(CMPUT350::Point2D(6.0f, 4.0f), 3.0f, CMPUT350::Colors::yellow);
    draw->DrawLine(CMPUT350::Point2D(0.0f, 10.0f), CMPUT350::Point2D(0.0f, 18.0f), 4.0f,
                   CMPUT350::Colors::white);  // Nose
}

CMPUT350::RGBColor Enemy3::Color() const
{
    return Health() < MaxHealth() ? CMPUT350::Colors::magenta : CMPUT350::Colors::green;
}

std::string Enemy3::TypeName() const { return "Enemy3"; }
