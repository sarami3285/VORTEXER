#include "Game.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "vector"
#include "memory"
#include "algorithm"
#include "Actor.h"
#include "SpriteComponent.h"
#include "LowLevelEnemy.h"
#include "Random.h"
#include "Player.h"
#include "TileMapComponent.h"
#include "CameraComponent.h"
#include "Scenes.h"
#include "HUDSpriteComponent.h"
using namespace std;

Game::Game()
	: mWindow(nullptr)
	, mRenderer(nullptr)
	, mIsRunning(true)
	, mUpdatingActors(false) {

}

bool Game::Initialize() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("初期化エラー：SDLが初期化できませんでした _ %s", SDL_GetError());
		return false;
	}

	if (TTF_Init() != 0) {
		SDL_Log("初期化エラー：フォントシステムの初期化ができませんでした _ %s", TTF_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("VORTEXER", 100, 100, 1024, 768, 0);
	if (!mWindow) {
		SDL_Log("初期化エラー：ウィンドウが作成できませんでした _ %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!mRenderer) {
		SDL_Log("初期化エラー：レンダラーが作成できませんでした _ %s", SDL_GetError());
		return false;
	}

	mFont = TTF_OpenFont("Assets/NSJP.ttf", 32);
	if (!mFont) {
		SDL_Log("初期化エラー：共通フォントのロードに失敗しました _ %s", TTF_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		SDL_Log("初期化エラー：音声システムの初期化ができませんでした _ %s", Mix_GetError());
		return false;
	}

	int loadedCurrency = 0;
	std::string equippedWeaponName;
	if (dataManager.LoadGameData("Assets/Data/SaveData.json", equippedWeaponName)) {
		if (!equippedWeaponName.empty()) {
			WeaponData weapon;
			if (weapon.LoadFromJSON("Assets/Data/WeaponData.json", equippedWeaponName)) {
				mPendingWeapon = weapon;
				mHasPendingWeapon = true;
			}
		}
	}


	Random::Init();
	mTicksCount = SDL_GetTicks();
	ChangeScene(std::make_unique<TitleScene>(this, mRenderer));
	return true;
}

void Game::RunLoop() {
    Uint32 lastTicks = SDL_GetTicks();

    while (mIsRunning) {
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f;
        if (deltaTime > 0.05f) deltaTime = 0.05f;
        lastTicks = currentTicks;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                mIsRunning = false;
            }

            if (mCurrentScene) {
                mCurrentScene->ProcessEvent(e);
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);

        if (mCurrentScene) {
            mCurrentScene->ProcessInput(keyState);
            mCurrentScene->Update(deltaTime);

            SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
            SDL_RenderClear(mRenderer);

            mCurrentScene->Draw(mRenderer);
            SDL_RenderPresent(mRenderer);

            if (mCurrentScene->IsFinished()) {
                ChangeScene(mCurrentScene->NextScene());
            }
        }
    }
}

void Game::GameUpdate() {
	if (!mPlayer || !mPlayer->mHPComponent || !mHPBar || !mStaminaBar) {
		return;
	}

	float hpPercent = static_cast<float>(mPlayer->mHPComponent->GetHP()) / mPlayer->mHPComponent->GetMaxHP();
	mHPBar->SetHPPercent(hpPercent);
	float staminaPercent = (mPlayer->ic->mBoostTimer / mPlayer->ic->mBoostDuration);
	mStaminaBar->SetHPPercent(staminaPercent);

	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.05f) {
		deltaTime = 0.05f;
	}
	mTicksCount = SDL_GetTicks();

	mUpdatingActors = true;
	for (auto actor : mActors) {
		actor->Update(deltaTime);
	}
	mUpdatingActors = false;

	for (auto pending : mWaitingActors) {
		mActors.emplace_back(pending);
	}
	mWaitingActors.clear();

	std::vector<Actor*> deadActors;
	for (auto actor : mActors) {
		if (actor->GetState() == Actor::EStop) {
			// 重複チェック
			if (std::find(deadActors.begin(), deadActors.end(), actor) == deadActors.end()) {
				deadActors.emplace_back(actor);
			}
		}
	}

	for (auto actor : deadActors) {
		if (actor == mPlayer) {
			continue;
		}
		RemoveActor(actor);
	}

	bool allEnemiesDead = true;
	for (auto actor : mActors) {
		if (auto enemy = dynamic_cast<LowLevelEnemy*>(actor)) {
			if (enemy->GetState() == Actor::EAlive) {
				allEnemiesDead = false;
				break;
			}
		}
	}

	if (allEnemiesDead && !mMissionCompleted) {
		mMissionCompleted = true;
		mMissionCompleteTime = SDL_GetTicks();
	}

	if (mMissionCompleted) {
		if (mMissionTextComponent) {
			mMissionTextComponent->Update(deltaTime);
		}

		Uint32 currentTime = SDL_GetTicks();
		if (currentTime - mMissionCompleteTime >= 5000) {
			if (!mSceneChangeRequested) {
				mSceneChangeRequested = true;
				mNextScene = std::make_unique<MenuScene>(this, mRenderer);
			}
		}
	}

	if (mHUDActor && mHUDActor->GetMiniMapLogic()) {
		std::vector<Vector2> enemyPositions;
		for (auto actor : mActors) {
			if (auto enemy = dynamic_cast<LowLevelEnemy*>(actor)) {
				if (enemy->GetState() == Actor::EAlive) {
					enemyPositions.push_back(enemy->GetPosition());
				}
			}
		}
		mHUDActor->GetMiniMapLogic()->SetTargets(enemyPositions);
		mHUDActor->GetMiniMapLogic()->SetPlayerPosition(mPlayer->GetPosition());
	}

	if (mSceneChangeRequested) {
		ChangeScene(std::move(mNextScene));
		mSceneChangeRequested = false;
	}
}



void Game::GameGenerate() {
	SDL_SetRenderDrawColor(mRenderer, 100, 200, 100, 255);
	SDL_RenderClear(mRenderer);

	if (mPlayer) {
		mCameraPos = mPlayer->GetCameraComponent()->GetCameraPos();
	}

	for (auto sprite : mSprites) {
		if (sprite && sprite->GetOwner()->GetState() != Actor::EStop) {
			sprite->Draw(mRenderer, mCameraPos);
		}
	}

	if (mMissionCompleted && mMissionTextComponent) {
		mMissionTextComponent->Draw();
	}

	SDL_RenderPresent(mRenderer);
}

void Game::GameInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_ESCAPE]) {
		mIsRunning = false;
	}

	mUpdatingActors = true;
	for (auto actor : mActors) {
		actor->ProcessInput(keyState);
	}
	mUpdatingActors = false;
}

void Game::LoadData()
{
	PlayBGM("Assets/Audio/Drumnbass_03.mp3", -1);
	mPlayer = new Player(this);
	mPlayer->SetPosition(Vector2(512.0f, 384.0f));
	mPlayer->SetRotation(Math::PiOver2);

	if (mHasPendingWeapon) {
		mPlayer->SetWeapon(mPendingWeapon);
		mHasPendingWeapon = false;
	}

	//--------------------↓↓↓↓↓マップ生成↓↓↓↓↓--------------------
	int tileSize = 540;
	int tileNumWidth = static_cast<int>((CameraComponent::mMapWidth + tileSize - 1.0f) / tileSize);
	int tileNumHeight = static_cast<int>((CameraComponent::mMapHeight + tileSize - 1.0f) / tileSize);
	SDL_Texture* tileSetTex = GetTexture("assets/MapTile.png");
	Actor* mapActor = new Actor(this);
	TileMapComponent* tileMap = new TileMapComponent(mapActor);
	tileMap->SetTileSet(tileSetTex, tileSize, 3);
	std::vector<std::vector<int>> mapData(tileNumHeight, std::vector<int>(tileNumWidth, 0));
	srand(static_cast<unsigned int>(time(nullptr)));

	for (int y = 0; y < tileNumHeight; ++y)
	{
		for (int x = 0; x < tileNumWidth; ++x)
		{
			int r = rand() % 100;
			if (y < tileNumHeight / 3|| y > tileNumHeight*2 / 3 || x < tileNumWidth / 3 || x > tileNumWidth * 2 / 3)
			{
				if (r < 60)
					mapData[y][x] = 0;
				else
					mapData[y][x] = 3;
			}
			else
			{
				if (r < 25)
					mapData[y][x] = 1;
				else if (r < 50)
					mapData[y][x] = 2;
				else if (r < 75)
					mapData[y][x] = 4;
				else
					mapData[y][x] = 5;
			}
		}
	}

	tileMap->SetMapData(mapData);
	mapActor->SetPosition(Vector2(0, 0));
	//--------------------↑↑↑↑↑マップ生成↑↑↑↑↑--------------------


	//--------------------↓↓↓↓↓HUD管理↓↓↓↓↓--------------------
	mHUDActor = new HUDActor(this);
	mHPBack = new HUDSpriteComponent(mHUDActor);
	mHPBar = new HUDSpriteComponent(mHUDActor);
	mHPBack->SetTexture(GetTexture("Assets/HPBack.png"));
	mHPBar->SetTexture(GetTexture("Assets/HP.png"));
	mHPBack->SetOffset(Vector2(250.0f, 50.0f));
	mHPBar->SetOffset(Vector2(250.0f, 50.0f));
	mHPBar->SetHPPercent(1.0f);

	mStaminaBack = new HUDSpriteComponent(mHUDActor);
	mStaminaBar = new HUDSpriteComponent(mHUDActor);
	mStaminaBack->SetTexture(GetTexture("Assets/StaminaBack.png"));
	mStaminaBar->SetTexture(GetTexture("Assets/Stamina1.png"));
	mStaminaBack->SetOffset(Vector2(250.0f, 88.0f));
	mStaminaBar->SetOffset(Vector2(250.0f, 84.0f));
	mStaminaBar->SetHPPercent(1.0f);


	auto weaponBack = new HUDSpriteComponent(mHUDActor);
	weaponBack->SetTexture(GetTexture("Assets/weaponBackGround.png"));
	weaponBack->SetOffset(Vector2(200.0f, 630.0f));

	auto weaponIcon = new HUDSpriteComponent(mHUDActor);

	if (mPlayer && mPlayer->GetWeaponComponent())
	{
		string iconPath = mPlayer->GetWeaponComponent()->GetWeaponIconPath();
		weaponIcon->SetTexture(GetTexture(iconPath));
	}
	else
	{
		weaponIcon->SetTexture(GetTexture("Assets/Weapon1.png"));
	}

	weaponIcon->SetOffset(Vector2(200.0f, 630.0f));
	mHUDActor->SetPosition(Vector2(0.0f, 0.0f));

	const int numEnemys = enemyCount;
	for (int i = 0; i < numEnemys; i++) {
		LowLevelEnemy* enemy = new LowLevelEnemy(this);

		HUDSpriteComponent* hpBar = new HUDSpriteComponent(mHUDActor);
		hpBar->SetTexture(GetTexture("Assets/HP1.png"));
		hpBar->SetOffset(Vector2(0, -50.0f));
		hpBar->SetTargetActor(enemy);
	}
	//--------------------↑↑↑↑↑HUD管理↑↑↑↑↑--------------------

	const int MissionTextY = 500;
	mMissionTextComponent = std::make_unique<BlinkingTextComponent>(
		mRenderer,
		mFont, 
		"Mission Completed !     Return to Base", 
		0,                                        // X座標 (コンポーネント内で中央寄せ)
		MissionTextY,                             // Y座標
		0.8f                                      // 点滅間隔
	);
}

void Game::UnloadData() {
	mActors.clear();
	mWaitingActors.clear();
	mSprites.clear();
	for (auto& tex : mTextures) {
		SDL_DestroyTexture(tex.second);
	}
	mTextures.clear();
	mPlayer = nullptr;
	mHUDActor = nullptr;
	mHPBack = nullptr;
	mHPBar = nullptr;
	mStaminaBack = nullptr;
	mStaminaBar = nullptr;
}

void Game::ChangeScene(std::unique_ptr<Scene> newScene) {
	if (mCurrentScene && dynamic_cast<GameScene*>(mCurrentScene.get()) != nullptr) {
		if (mPlayer && mPlayer->GetWeaponComponent()) {
			mPendingWeapon = mPlayer->GetWeaponComponent()->GetWeaponData();
			mHasPendingWeapon = true;
		}

		UnloadData();
		ResetGameSceneState();
	}

	if (mCurrentBGM) {
		StopBGM();
	}
	if (mPlayer && mPlayer->GetWeaponComponent()) {
		mPendingWeapon = mPlayer->GetWeaponComponent()->GetWeaponData();
		mHasPendingWeapon = true;
	}

	mCurrentScene = std::move(newScene);
	if (mCurrentScene) {
		mCurrentScene->LoadContent();
		mCurrentScene->LoadData();
	}
}

void Game::ResetGameSceneState() {
	mMissionCompleted = false;
	mMissionCompleteTime = 0;
	mSceneChangeRequested = false;
	mNextScene = nullptr;
}

void Game::Shutdown() {
	if (mPlayer) {
		dataManager.SaveGameData("Assets/Data/SaveData.json", mPlayer->GetCurrentWeaponName());
	}
	UnloadData();
	SDL_DestroyWindow(mWindow);
	SDL_DestroyRenderer(mRenderer);
	Mix_CloseAudio();
	SDL_Quit();
}


void Game::AddActor(Actor* actor) {
	// 重複登録防止チェック
	if (std::find(mActors.begin(), mActors.end(), actor) != mActors.end() ||
		std::find(mWaitingActors.begin(), mWaitingActors.end(), actor) != mWaitingActors.end()) {
		SDL_Log("Warning: Actor %p is already added!", actor);
		return;
	}

	if (mUpdatingActors) {
		mWaitingActors.emplace_back(actor);
	}
	else {
		mActors.emplace_back(actor);
	}
}

void Game::RemoveActor(Actor* actor) {
	auto iter_waiting = std::find(mWaitingActors.begin(), mWaitingActors.end(), actor);
	if (iter_waiting != mWaitingActors.end()) {
		std::iter_swap(iter_waiting, mWaitingActors.end() - 1);
		mWaitingActors.pop_back();
	}

	auto iter_actors = std::find(mActors.begin(), mActors.end(), actor);
	if (iter_actors != mActors.end()) {
		std::iter_swap(iter_actors, mActors.end() - 1);
		mActors.pop_back();
	}
	delete actor;
}

void Game::AddSprite(SpriteComponent* sprite) {
	int myDrawOrder = sprite->GetDrawOrder();
	auto iter = mSprites.begin();
	for (; iter != mSprites.end(); iter++) {
		if (myDrawOrder < (*iter)->GetDrawOrder()) {
			break;
		}
	}

	mSprites.insert(iter, sprite);
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
	SDL_Texture* tex = nullptr;
	auto iter = mTextures.find(fileName);
	if (iter != mTextures.end())
	{
		tex = iter->second;
	}
	else
	{
		SDL_Surface* surf = IMG_Load(fileName.c_str());
		if (!surf)
		{
			SDL_Log("Failed to load texture file %s", fileName.c_str());
			return nullptr;
		}

		tex = SDL_CreateTextureFromSurface(mRenderer, surf);
		SDL_FreeSurface(surf);
		if (!tex)
		{
			SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
			return nullptr;
		}

		mTextures.emplace(fileName.c_str(), tex);
	}
	return tex;
}

void Game::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	if (iter != mSprites.end())
	{
		mSprites.erase(iter);
	}
}

void Game::UpdateObtainedWeapons() {
	std::vector<std::string> allWeaponNames = {
		"MusinGun",
		"ShotGun",
		"RailGun"
	};

	mObtainedWeaponNames.clear();

	for (const std::string& name : allWeaponNames) {
		WeaponData data;
		if (data.LoadFromJSON("Assets/Data/WeaponData.json", name) && data.gotWeapon) {
			mObtainedWeaponNames.push_back(name);
		}
	}
}


const std::vector<std::string>& Game::GetObtainedWeaponNames() const {
	return mObtainedWeaponNames;
}

void Game::AddObtainedWeapon(const std::string& weaponName) {
	if (!dataManager.IsWeaponObtained(weaponName)) {
		dataManager.SetWeaponObtained(weaponName, true);
		SDL_Log("INFO: Weapon obtained: %s", weaponName.c_str());
	}
}




void Game::PlayBGM(const std::string& fileName, int loop)
{
	if (mCurrentBGM)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(mCurrentBGM);
		mCurrentBGM = nullptr;
	}

	mCurrentBGM = Mix_LoadMUS(fileName.c_str());
	if (!mCurrentBGM)
	{
		SDL_Log("Failed to load BGM: %s", Mix_GetError());
		return;
	}

	Mix_VolumeMusic(24);

	Mix_PlayMusic(mCurrentBGM, loop);
}

void Game::StopBGM()
{
	if (mCurrentBGM)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(mCurrentBGM);
		mCurrentBGM = nullptr;
	}
}


void Game::RequestSceneChange(std::unique_ptr<Scene> newScene) {
	mSceneChangeRequested = true;
	mNextScene = std::move(newScene);
}