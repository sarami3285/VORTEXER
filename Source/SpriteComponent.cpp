#include "SpriteComponent.h"
#include "Actor.h"
#include "SDL.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
    :Component(owner)
    , mTexture(nullptr)
    , mDrawOrder(drawOrder)
    , mTexWidth(0)
    , mTexHeight(0)
    , mAlpha(1.0f)
    , mCastShadow(false)
{
    mOwner->GetGame()->AddSprite(this);
}

SpriteComponent::~SpriteComponent() {
    if (mOwner && mOwner->GetGame()) {
        mOwner->GetGame()->RemoveSprite(this);
    }
}

void SpriteComponent::SetTexture(SDL_Texture* texture) {
	mTexture = texture;
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}

void SpriteComponent::SetSpriteUV(float u, float v, float uWidth, float vHeight)
{
    mUseSrcRect = true;
    mSrcRect.x = static_cast<int>(u);
    mSrcRect.y = static_cast<int>(v);
    mSrcRect.w = static_cast<int>(uWidth);
    mSrcRect.h = static_cast<int>(vHeight);
}

void SpriteComponent::Draw(SDL_Renderer* renderer, const Vector2& cameraPos)
{
    if (this->mTexture)
    {
        SDL_Rect src;
        if (this->mUseSrcRect)
        {
            src = this->mSrcRect;
        }
        else
        {
            src = { 0, 0, this->mTexWidth, this->mTexHeight };
        }

        int w = static_cast<int>(src.w * mOwner->GetScale());
        int h = static_cast<int>(src.h * mOwner->GetScale());
        Vector2 pos = mOwner->GetPosition() - cameraPos;

        SDL_Rect dst;
        dst.w = w;
        dst.h = h;
        dst.x = static_cast<int>(pos.x - w / 2);
        dst.y = static_cast<int>(pos.y - h / 2);

        double angle = Math::ToDegrees(mOwner->GetRotation());

        // ----------------------------------------------------
        // ★影の描画ロジック★
        if (mCastShadow) {
            // ... (影を描画するコード) ...

            // 影の描画が終わったら、スプライト本体用にmColorの値に戻す
            Uint8 red = static_cast<Uint8>(mColor.x * 255.0f);
            Uint8 green = static_cast<Uint8>(mColor.y * 255.0f);
            Uint8 blue = static_cast<Uint8>(mColor.z * 255.0f);
            SDL_SetTextureColorMod(mTexture, red, green, blue);

            // アルファ値は次の行で設定されるので、ここでは白 (255) に戻す
            SDL_SetTextureAlphaMod(mTexture, 255);
        }
        // ----------------------------------------------------

        SDL_SetTextureAlphaMod(mTexture, static_cast<Uint8>(mAlpha * 255));

        // ★ スプライト本体の描画 ★
        SDL_RenderCopyEx(renderer, mTexture, &src, &dst, angle, nullptr, SDL_FLIP_NONE);

        // ----------------------------------------------------
        // ★★★ 描画完了後、テクスチャの色とアルファをデフォルトに戻す ★★★
        // これにより、次にこのテクスチャを使う他のスプライト（プレイヤー機など）に影響が出ない
        SDL_SetTextureColorMod(mTexture, 255, 255, 255);
        SDL_SetTextureAlphaMod(mTexture, 255);
        // ----------------------------------------------------
    }
}

void SpriteComponent::SetColor(float r, float g, float b)
{
    // SetColorは引き続き、mColorの値を更新し、テクスチャの色変調を設定する役割を果たす
    Uint8 red = static_cast<Uint8>(r * 255.0f);
    Uint8 green = static_cast<Uint8>(g * 255.0f);
    Uint8 blue = static_cast<Uint8>(b * 255.0f);

    mColor = Vector3(r, g, b);

    if (mTexture)
    {
        SDL_SetTextureColorMod(mTexture, red, green, blue);
    }
}
