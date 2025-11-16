#include "TileMapComponent.h"
#include "Actor.h"
#include "SDL.h"

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
    : SpriteComponent(owner, drawOrder)
    , mTileSet(nullptr)
    , mTileSize(32)
    , mTilesPerRow(4)
{
}

TileMapComponent::~TileMapComponent()
{
    // mTileSetÇÕäOïîÇ≈ä«óùÇ≥ÇÍÇÈëOíÒÇ»ÇÃÇ≈âï˙ÇµÇ»Ç¢
}

void TileMapComponent::SetTileSet(SDL_Texture* tileSet, int tileSize, int tilesPerRow)
{
    mTileSet = tileSet;
    mTileSize = tileSize;
    mTilesPerRow = tilesPerRow;
}

void TileMapComponent::SetMapData(const std::vector<std::vector<int>>& mapData)
{
    mMapData = mapData;
}

void TileMapComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    if (!mTileSet) return;
    if (mMapData.empty()) return;

    int rows = static_cast<int>(mMapData.size());
    int cols = static_cast<int>(mMapData[0].size());

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < cols; ++x)
        {
            int tileIndex = mMapData[y][x];
            if (tileIndex < 0) continue;

            int tu = tileIndex % mTilesPerRow;
            int tv = tileIndex / mTilesPerRow;

            SDL_Rect srcRect = { tu * mTileSize, tv * mTileSize, mTileSize, mTileSize };
            SDL_Rect dstRect = {
                static_cast<int>(x * mTileSize - cameraPos.x),
                static_cast<int>(y * mTileSize - cameraPos.y),
                mTileSize,
                mTileSize
            };

            SDL_RenderCopy(renderer, mTileSet, &srcRect, &dstRect);
        }
    }
}
