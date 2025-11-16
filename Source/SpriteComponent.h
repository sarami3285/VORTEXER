#pragma once
#include "Component.h"
#include "SDL.h"
#include "Math.h"

class SpriteComponent : public Component {
public:
    SpriteComponent(class Actor* owner, int drawOrder = 100);
    ~SpriteComponent();

    virtual void Draw(SDL_Renderer* renderer, const Vector2& cameraPos);
    virtual void SetTexture(SDL_Texture* texture);

    void SetSourceRect(const SDL_Rect& rect) { mSrcRect = rect; mUseSrcRect = true; }
    void ClearSourceRect() { mUseSrcRect = false; }

    int GetDrawOrder() const { return mDrawOrder; }
    int GetTexHeight() const { return mTexHeight; }
    int GetTexWidth() const { return mTexWidth; }
    Actor* GetOwner() const { return mOwner; }
    void SetSpriteUV(float u, float v, float uWidth, float vHeight);

    void SetCastShadow(bool cast) { mCastShadow = cast; }
    bool GetCastShadow() const { return mCastShadow; }

    void SetAlpha(float alpha) { mAlpha = alpha; }
    float GetAlpha() const { return mAlpha; }

    void SetColor(float r, float g, float b);

protected:
    SDL_Texture* mTexture;
    int mDrawOrder;
    int mTexWidth;
    int mTexHeight;
    float mAlpha;
    Vector3 mColor;

    SDL_Rect mSrcRect;
    bool mUseSrcRect = false;

    bool mCastShadow;

};
