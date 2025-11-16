#pragma once

#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL.h>
#include <string>
#include <vector>

class SpriteSheet {
public:
    // コンストラクタとデストラクタ
    SpriteSheet(SDL_Renderer* renderer, const std::string& filePath, int spriteWidth, int spriteHeight);
    ~SpriteSheet();

    // スプライトの描画
    void RenderSprite(int index, int x, int y);

private:
    // スプライトの位置とサイズを保存
    struct Sprite {
        int x;
        int y;
        int width;
        int height;
    };

    SDL_Renderer* mRenderer;           // SDLレンダラー
    SDL_Texture* mTexture;             // スプライトシート用テクスチャ
    std::vector<Sprite> mSprites;      // スプライト情報
    int mSpriteWidth;                  // 各スプライトの幅
    int mSpriteHeight;                 // 各スプライトの高さ

    // スプライトシートからスプライトを読み込む
    void LoadSprites();
};

#endif // SPRITESHEET_H

