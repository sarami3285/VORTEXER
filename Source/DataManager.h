// DataManager.h
#pragma once
#include "string"
#include <map>

class DataManager {
public:
    int GetCurrency() const;
    void AddCurrency(int amount);
    bool UseCurrency(int amount);
    void SetCurrency(int amount);

    bool SaveGameData(const std::string& path, const std::string& equippedWeaponName);
    bool LoadGameData(const std::string& path, std::string& outEquippedWeaponName);

    bool IsWeaponObtained(const std::string& weaponName) const;
    void SetWeaponObtained(const std::string& weaponName, bool obtained);

private:
    static int mCurrencyAmount;
};

