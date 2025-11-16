#include "MiniMapSpriteComponent.h"
#include "MiniMapComponent.h"
#include "Game.h"
#include <cmath>

MiniMapSpriteComponent::MiniMapSpriteComponent(Actor* owner, MiniMapComponent* logic, int drawOrder)
    : HUDSpriteComponent(owner, drawOrder)
    , mLogic(logic)
{
}

void MiniMapSpriteComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    if (!mLogic) return;

    Vector2 center = GetOwner()->GetPosition() + mOffset;
    float radius = mLogic->GetRadius();
    float visibleRange = mLogic->GetVisibleRange();
    const Vector2& playerPos = mLogic->GetPlayerPosition();
    const auto& enemies = mLogic->GetEnemyPositions();

    // 背景
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect bg = { static_cast<int>(center.x - radius), static_cast<int>(center.y - radius),
                    static_cast<int>(radius * 2), static_cast<int>(radius * 2) };
    SDL_RenderFillRect(renderer, &bg);

    // 枠線（白い円）
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int angle = 0; angle < 360; ++angle) {
        for (float size = 30; size <= radius; size += 30) {
            float rad = angle * Math::Pi / 180.0f;
            int x = static_cast<int>(center.x + size * std::cos(rad));
            int y = static_cast<int>(center.y + size * std::sin(rad));
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    // プレイヤー位置（緑の中心点）
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect playerDot = { static_cast<int>(center.x) - 3, static_cast<int>(center.y) - 3, 6, 6 };
    SDL_RenderFillRect(renderer, &playerDot);

    //スキャンライン
    float angle = mLogic->GetScanAngle();
    int endX = static_cast<int>(center.x + radius * std::cos(angle));
    int endY = static_cast<int>(center.y + radius * std::sin(angle));
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); // 薄めの緑
    SDL_RenderDrawLine(renderer, static_cast<int>(center.x), static_cast<int>(center.y), endX, endY);

    const auto& detectedEnemies = mLogic->GetDetectedEnemies();
    const float FADE_OUT_DURATION = 1.0f;

    // 検出された敵を一つずつ描画
    for (const auto& detected : detectedEnemies) {
        Vector2 diff = detected.position - playerPos;
        float dist = diff.Length();
        if (dist > visibleRange) continue;
        float ratio = detected.remainingTime / FADE_OUT_DURATION;
        // 比率を0-255のアルファ値に変換
        Uint8 alpha = static_cast<Uint8>(Math::Clamp(ratio, 0.0f, 1.0f) * 255);

        // SDL描画色を設定 (赤色で、計算されたアルファ値を使用)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, alpha);

        // 描画位置を計算 (ミニマップの中心を基準にスケーリング)
        Vector2 scaled = diff * (radius / visibleRange);
        SDL_Rect dot = {
            static_cast<int>(center.x + scaled.x) - 2,
            static_cast<int>(center.y + scaled.y) - 2,
            4, 4
        };

        // ドットを描画
        SDL_RenderFillRect(renderer, &dot);
    }
}