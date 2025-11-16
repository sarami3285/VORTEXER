#pragma once
#include "Scene.h"
#include <memory>
#include "SDL_ttf.h"
#include "UIButton.h"
#include "HUDActor.h"
#include "ScrollItemComponent.h"
#include "BlinkingTextComponent.h"
#include "Player.h"
#include "string"

class Game;
class GameScene;

class TitleScene : public Scene {
public:
    TitleScene(Game* game, SDL_Renderer* renderer);
    ~TitleScene();
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

    bool IsFinished() const override;

    std::unique_ptr<Scene> NextScene() override;
private:
    void LoadContent();

    Game* mGame;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTextureBack = nullptr;
    SDL_Texture* mTextureTitle = nullptr;
    TTF_Font* mFont = nullptr;
    Mix_Chunk* StartSound;

    bool mIsFadingOut = false;
    Uint8 mFadeAlpha = 0;

    std::unique_ptr<BlinkingTextComponent> mBlinkingText;
};

class MenuScene : public Scene {
public:
    MenuScene(Game* game, SDL_Renderer* renderer);

    ~MenuScene();

    void ProcessEvent(const SDL_Event& e);
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    Game* mGame;
    Player* mPlayer;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture = nullptr;
    SDL_Texture* mBackTexture = nullptr;
    SDL_Texture* mWeaponTexture = nullptr;
    TTF_Font* mFont = nullptr;

    std::unique_ptr<UIButton> startButton;
    std::unique_ptr<UIButton> storeButton;
    std::unique_ptr<UIButton> customizeButton;

    SDL_Texture* mCurrencyText = nullptr;
    SDL_Rect mCurrencyRect{};

    void LoadContent();
    void UnloadContent();
};

class CustomizeScene : public Scene {
public:
    CustomizeScene(class Game* game, SDL_Renderer* renderer);
    ~CustomizeScene();

    void LoadContent() override;
    void UnloadContent();
    void ProcessEvent(const SDL_Event& e) override;
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    class Game* mGame;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture = nullptr;
    TTF_Font* mFont = nullptr;
    std::unique_ptr<class UIButton> quitButton;
    std::unique_ptr<class HUDActor> mHUDActor;
    std::unique_ptr<class ScrollItemComponent> mScrollList;
    std::string mCurrentWeaponName;
};

class ShopScene : public Scene {
public:
    ShopScene(class Game* game, SDL_Renderer* renderer);
    ~ShopScene();
    void LoadContent() override;
    void UnloadContent();
    void ProcessEvent(const SDL_Event& e) override;
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;
    bool IsFinished() const override { return false; }
    std::unique_ptr<Scene> NextScene() override { return nullptr; }
    void Refresh();
private:
    void SelectWeapon(const std::string& weaponName, SDL_Texture* texture, const std::string& description);
    class Game* mGame;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture = nullptr;
    SDL_Texture* mCurrencyText = nullptr;
    SDL_Rect mCurrencyRect;
    TTF_Font* mFont = nullptr;
    Mix_Chunk* BuySound = nullptr;
    SDL_Texture* mSoldOutTexture = nullptr;
    bool mAllWeaponsSoldOut = false;
    std::unique_ptr<UIButton> quitButton;
    std::unique_ptr<ScrollItemComponent> mScrollList;
    std::unique_ptr<HUDActor> mHUDActor;
};

class MissionScene : public Scene {
public:
    MissionScene(Game* game, SDL_Renderer* renderer);
    ~MissionScene();

    void ProcessEvent(const SDL_Event& e);
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

private:
    Game* mGame;
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture = nullptr;
    TTF_Font* mFont = nullptr;

    std::unique_ptr<UIButton> quitButton = nullptr;
    std::unique_ptr<UIButton> missionButton[3] = { nullptr };
    std::unique_ptr<HUDActor> mHUDActor;

    ScrollItemComponent* mScrollList = nullptr;
    SDL_Texture* mCurrencyText = nullptr;
    SDL_Rect mCurrencyRect{};

    void LoadContent();
};

class GameScene : public Scene {
public:
    explicit GameScene(Game* game, SDL_Renderer* renderer);
    void ProcessInput(const Uint8* keyState) override;
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;
    void LoadContent();
    void LoadData();
private:
    Game* mGame;
};
