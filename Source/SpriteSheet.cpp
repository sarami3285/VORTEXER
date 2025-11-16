#include "SpriteSheet.h"
#include <iostream>

SpriteSheet::SpriteSheet(SDL_Renderer* renderer, const std::string& filePath, int spriteWidth, int spriteHeight)
    : mRenderer(renderer), mSpriteWidth(spriteWidth), mSpriteHeight(spriteHeight)
{
    // スプライトシートの画像をロード
    SDL_Surface* tempSurface = SDL_LoadBMP(filePath.c_str());
    if (!tempSurface) {
        std::cerr << "Failed to load sprite sheet: " << SDL_GetError() << std::endl;
        return;
    }

    mTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    if (!mTexture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        return;
    }

    LoadSprites();  // スプライトシートからスプライトをロード
}

SpriteSheet::~SpriteSheet() {
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
    }
}

void SpriteSheet::LoadSprites() {
    // スプライトシートの横幅と縦幅を取得
    int sheetWidth, sheetHeight;
    SDL_QueryTexture(mTexture, NULL, NULL, &sheetWidth, &sheetHeight);

    // スプライトシート内のスプライトの数を計算
    int cols = sheetWidth / mSpriteWidth;  // 横のスプライト数
    int rows = sheetHeight / mSpriteHeight; // 縦のスプライト数

    // スプライトを配列に格納
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            mSprites.push_back(Sprite{
                col * mSpriteWidth,  // X座標
                row * mSpriteHeight, // Y座標
                mSpriteWidth,        // 幅
                mSpriteHeight        // 高さ
                });
        }
    }
}

void SpriteSheet::RenderSprite(int index, int x, int y) {
    if (index < 0 || index >= mSprites.size()) {
        std::cerr << "Invalid sprite index!" << std::endl;
        return;
    }

    // スプライト情報を取得
    Sprite& sprite = mSprites[index];

    SDL_Rect srcRect = { sprite.x, sprite.y, sprite.width, sprite.height };
    SDL_Rect destRect = { x, y, sprite.width, sprite.height };

    // スプライトをレンダリング
    SDL_RenderCopy(mRenderer, mTexture, &srcRect, &destRect);
}
