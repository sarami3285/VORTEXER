#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

struct WeaponData
{
    std::string name;
    std::string bulletTexturePath;
    int price;
    std::string description;
    float fireRate;
    int baseDamage;
    float bulletSpeed;
    float decayRate;
    int burstCount;
    float burstInterval;
    int pelletCount;
    float spreadAngle;
    float randomSpread;
    std::string weaponIcon;
    std::string shotSound;
    bool gotWeapon;

    bool LoadFromJSON(const std::string& filepath, const std::string& weaponName)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        json j;
        try {
            file >> j;
        }
        catch (const json::parse_error& e) {
            SDL_Log("JSON PARSE ERROR: %s at byte %lu", e.what(), e.byte);
            return false;
        }
        catch (const std::exception& e) {
            SDL_Log("JSON LOADING ERROR: %s", e.what());
            return false;
        }

        if (!j.contains("weapons") || !j["weapons"].contains(weaponName)) return false;

        const auto& w = j["weapons"][weaponName];

        try {
            name = weaponName;
            auto& weaponInfo = j["weapons"][weaponName];
            bulletTexturePath = w["bulletTexturePath"].get<std::string>();
            fireRate = w["fireRate"].get<float>();
            price = w["price"].get<int>();
            description = weaponInfo.value("description", "ê‡ñæï∂Ç™Ç†ÇËÇ‹ÇπÇÒ");
            baseDamage = w["baseDamage"].get<int>();
            bulletSpeed = w["bulletSpeed"].get<float>();
            randomSpread = w.value("randomSpread", 0.0f);
            decayRate = w["decayRate"].get<float>();
            burstCount = w["burstCount"].get<int>();
            burstInterval = w["burstInterval"].get<float>();
            pelletCount = w["pelletCount"].get<int>();
            spreadAngle = w["spreadAngle"].get<float>();
            weaponIcon = w.value("weaponIcon", "");
            shotSound = w.value("shotSound", "");
            gotWeapon = w.value("GetWeapon", false);
        }
        catch (const std::exception& e) {
            SDL_Log("JSON parse error: %s", e.what());
            return false;
        }

        return true;
    }

    WeaponData() = default;
    WeaponData(const WeaponData&) = default;
    WeaponData& operator=(const WeaponData&) = default;

    // ïKóvÇ»ÇÁ move Ç‡
    WeaponData(WeaponData&&) noexcept = default;
    WeaponData& operator=(WeaponData&&) noexcept = default;
};