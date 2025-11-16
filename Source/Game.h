// Game.h

#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include "CameraComponent.h"
#include "Player.h"
#include <SDL_mixer.h>
#include "SDL_ttf.h"
#include "HUDActor.h"
#include "HPComponent.h"
#include "HUDSpriteComponent.h"
#include "Scene.h"
#include "DataManager.h"
#include "BlinkingTextComponent.h"

using namespace std;

enum class GameState {
    Title,
    BaseMenu,
    MissionStart,
    InMission,
    Result,
    ReturnToBase
};

class Game {
public:
    Game();
    bool Initialize();
    void RunLoop();
    void Shutdown();

    void ChangeScene(std::unique_ptr<Scene> newScene);

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    void AddSprite(class SpriteComponent* sprite);
    void RemoveSprite(class SpriteComponent* sprite);

    SDL_Texture* GetTexture(const std::string& fileName);

    vector<class Actor*>& GetActors() { return mActors; }
    //vector<class LowLevelEnemy*>& GetEnemy() { return mEnemys; }

    Vector2 mCameraPos;

    CameraComponent* GetCamera() const { return mCamera; }
    void SetCamera(CameraComponent* camera) { mCamera = camera; }
    Player* GetPlayer() const { return mPlayer; }
    void SetPlayer(Player* player) { mPlayer = player; }

    DataManager& GetDataManager() { return dataManager; }

    void GameInput();
    void GameUpdate();
    void GameGenerate();

    void PlayBGM(const std::string& fileName, int loop = -1);
    void StopBGM();

    void LoadData();
    void UnloadData();

    SDL_Renderer* GetRenderer() const { return mRenderer; }
    bool IsRunning() const { return mIsRunning; }

    void RequestSceneChange(std::unique_ptr<Scene> newScene);

    void UpdateObtainedWeapons();
    const std::vector<std::string>& GetObtainedWeaponNames()const;
    void AddObtainedWeapon(const std::string& weaponName);
    WeaponData mPendingWeapon; // Player生成前にWeaponを渡す用
    bool mHasPendingWeapon = false;
    int enemyCount;
    std::vector<std::string> mObtainedWeaponNames;
    std::vector<std::string> mObtainedWeapons;

    bool mMissionCompleted = false;    // 任務が完了したかどうか
    Uint32 mMissionCompleteTime = 0;   // 任務完了時刻
    bool mSceneChangeRequested = false; // シーン遷移がリクエストされたか
    std::unique_ptr<Scene> mNextScene;  // 次のシーンのポインタ

private:
    void ResetGameSceneState();

    vector<class Actor*> mActors;
    vector<class Actor*> mWaitingActors;
    vector<class SpriteComponent*> mSprites;

    std::unique_ptr<Scene> mCurrentScene;

    unordered_map<std::string, SDL_Texture*> mTextures;

    Mix_Music* mCurrentBGM = nullptr;

    Game* mGame;
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mUpdatingActors;
    class Player* mPlayer;
    CameraComponent* mCamera;
    HUDSpriteComponent* mHPBar;
    HUDSpriteComponent* mHPBack;
    HUDSpriteComponent* mStaminaBar;
    HUDSpriteComponent* mStaminaBack;
    HPComponent* mHPComponent;
    DataManager dataManager;

    SDL_Surface* mMissionCompleteSurface = nullptr;
    SDL_Texture* mMissionCompleteTexture = nullptr;

    std::unique_ptr<BlinkingTextComponent> mMissionTextComponent;

    HUDActor* mHUDActor;
};
