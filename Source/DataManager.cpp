#include "DataManager.h"
#include <fstream>
#include <string>
#include "json.hpp"
#include "SDL.h"

using json = nlohmann::json;
int DataManager::mCurrencyAmount = 0;

int DataManager::GetCurrency() const {
    return mCurrencyAmount;
}

void DataManager::AddCurrency(int amount) {
    if (amount < 0) return;
    mCurrencyAmount += amount;
}

bool DataManager::UseCurrency(int amount) {
    if (amount < 0) return false;
    if (mCurrencyAmount < amount) return false;
    mCurrencyAmount -= amount;
    return true;
}

void DataManager::SetCurrency(int amount) {
    if (amount >= 0) {
        mCurrencyAmount = amount;
    }
}

// 武器データをJSONファイルから直接読み込む
bool DataManager::IsWeaponObtained(const std::string& weaponName) const {
    std::ifstream file("Assets/Data/WeaponData.json");
    if (!file.is_open()) return false;

    json j;
    file >> j;
    file.close();

    if (j.contains("weapons") && j["weapons"].contains(weaponName)) {
        return j["weapons"][weaponName].value("GetWeapon", false);
    }
    return false;
}

// 武器獲得状態をJSONファイルに直接書き込む
void DataManager::SetWeaponObtained(const std::string& weaponName, bool obtained) {
    std::ifstream inFile("Assets/Data/WeaponData.json");
    if (!inFile.is_open()) {
        SDL_Log("Error: Could not open WeaponData.json for reading.");
        return;
    }

    json data;
    try {
        inFile >> data;
        inFile.close();
    }
    catch (json::exception& e) {
        SDL_Log("JSON parse error on read: %s", e.what());
        return;
    }

    if (data.contains("weapons") && data["weapons"].contains(weaponName)) {
        data["weapons"][weaponName]["GetWeapon"] = obtained;
        std::ofstream outFile("Assets/Data/WeaponData.json");
        if (outFile.is_open()) {
            outFile << data.dump(4);
            outFile.close();
        }
        else {
            SDL_Log("Error: Could not open WeaponData.json for writing.");
        }
    }
}

// ゲームデータの保存（通貨と装備武器のみを保存）
bool DataManager::SaveGameData(const std::string& path, const std::string& equippedWeaponName) {
    json j;
    j["currency"] = mCurrencyAmount;
    j["equippedWeapon"] = equippedWeaponName;

    std::ofstream ofs(path);
    if (!ofs.is_open()) {
        SDL_Log("Error: Could not open save file for writing.");
        return false;
    }
    ofs << j.dump(4);
    return true;
}

// ゲームデータのロード（通貨と装備武器のみをロード）
bool DataManager::LoadGameData(const std::string& path, std::string& outEquippedWeaponName) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        // セーブファイルがない場合は初期状態を返す
        SDL_Log("Save file not found, starting a new game.");
        return false;
    }

    json j;
    try {
        ifs >> j;
    }
    catch (json::exception& e) {
        SDL_Log("JSON parse error on load: %s", e.what());
        return false;
    }

    if (j.contains("currency")) {
        mCurrencyAmount = j["currency"].get<int>();
    }
    if (j.contains("equippedWeapon")) {
        outEquippedWeaponName = j["equippedWeapon"].get<std::string>();
    }

    return true;
}