#pragma once

#include "Character.hpp"

namespace BVA {

// Bas - Master of the legendary Bas Veeg technique
class BasCharacter : public Character {
public:
    BasCharacter() : Character(CharacterID::Bas) {
        name = "Bas";
        ability = {
            "Bas Veeg",
            "BAS VEEG!",
            {{AbilityEffect::SplashDamage, 45.0f}},
            0.1f,  // instant
            11.0f,
            "Master of the legendary Bas Veeg technique. AOE specialist who controls the battlefield."
        };
        stats.maxHealth = 120.0f;
        stats.attackDamage = 12.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Berkay - Special Kebab power
class BerkayCharacter : public Character {
public:
    BerkayCharacter() : Character(CharacterID::Berkay) {
        name = "Berkay";
        ability = {
            "Special Kebab",
            "ik kan niet stoppen!",
            {{AbilityEffect::DamageBoost, 2.2f}, {AbilityEffect::HealthBoost, 35.0f}},
            6.0f,
            12.0f,
            "Kebab-powered warrior with unstoppable momentum."
        };
        stats.maxHealth = 110.0f;
        stats.attackDamage = 11.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Luca - Winter Arc
class LucaCharacter : public Character {
public:
    LucaCharacter() : Character(CharacterID::Luca) {
        name = "Luca";
        ability = {
            "Winter Arc",
            "nee nu ben ik klaar ik ga in mijn winter arc",
            {{AbilityEffect::DamageBoost, 2.8f}, {AbilityEffect::HealthBoost, 15.0f}},
            5.0f,
            12.0f,
            "Enters winter arc for massive damage output."
        };
        stats.maxHealth = 100.0f;
        stats.attackDamage = 10.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Gefferinho - Maar Mevrouw Rage
class GefferinhoCharacter : public Character {
public:
    GefferinhoCharacter() : Character(CharacterID::Gefferinho) {
        name = "Gefferinho";
        ability = {
            "Maar Mevrouw Rage",
            "maar mevouw wat doe je",
            {{AbilityEffect::SpeedBoost, 2.0f}, {AbilityEffect::DamageBoost, 2.0f}, {AbilityEffect::HealthBoost, 25.0f}},
            6.0f,
            13.0f,
            "Balanced fighter with strong all-around buffs."
        };
        stats.maxHealth = 115.0f;
        stats.attackDamage = 11.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Hadi - Dubai Emirates speed demon
class HadiCharacter : public Character {
public:
    HadiCharacter() : Character(CharacterID::Hadi) {
        name = "Hadi";
        ability = {
            "Dubai Emirates",
            "Dubai Emirates!",
            {{AbilityEffect::SpeedBoost, 3.2f}, {AbilityEffect::DamageBoost, 1.5f}},
            6.0f,
            12.0f,
            "Lightning fast from Dubai Emirates. Speed demon who strikes before you can react."
        };
        stats.maxHealth = 95.0f;
        stats.moveSpeed = 6.0f;
        stats.attackDamage = 9.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Nitin - Fire damage specialist
class NitinCharacter : public Character {
public:
    NitinCharacter() : Character(CharacterID::Nitin) {
        name = "Nitin";
        ability = {
            "Barra in je Kont",
            "Barra in je kont!",
            {{AbilityEffect::FireDamage, 8.0f}},
            1.0f,  // applies DOT
            11.0f,
            "Sets the competition ablaze. DOT specialist with burning passion."
        };
        stats.maxHealth = 105.0f;
        stats.attackDamage = 10.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// PalaBaba (Yigit Baba) - The ultimate fighter
class PalaBabaCharacter : public Character {
public:
    PalaBabaCharacter() : Character(CharacterID::PalaBaba) {
        name = "PalaBaba (Yigit Baba)";
        ability = {
            "Sivas Rage",
            "TURKIYEEEE",
            {{AbilityEffect::DamageBoost, 3.0f}, {AbilityEffect::SpeedBoost, 3.0f}, {AbilityEffect::HealthBoost, 30.0f}},
            10.0f,
            30.0f,
            "THE ULTIMATE FIGHTER. Turkish powerhouse with unmatched raw power."
        };
        stats.maxHealth = 140.0f;
        stats.attackDamage = 15.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Fufinho - Projectile master
class FufinhoCharacter : public Character {
public:
    FufinhoCharacter() : Character(CharacterID::Fufinho) {
        name = "Fufinho";
        ability = {
            "Fufu Throw",
            "ik eet fufu!",
            {{AbilityEffect::Projectile, 80.0f}},
            0.1f,  // instant projectile
            8.0f,
            "Fufu master with projectile prowess. Throws devastating fufu bombs that deal massive damage."
        };
        stats.maxHealth = 100.0f;
        stats.attackDamage = 10.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// EfeAbi - Lahmacun-powered warrior
class EfeAbiCharacter : public Character {
public:
    EfeAbiCharacter() : Character(CharacterID::EfeAbi) {
        name = "Efe abi";
        ability = {
            "Lahmacun Power",
            "Ik eet lahmacun!",
            {{AbilityEffect::SpeedBoost, 2.5f}, {AbilityEffect::DamageBoost, 2.0f}},
            5.0f,
            15.0f,
            "Lahmacun-powered warrior. Gains incredible speed and power from Turkish street food."
        };
        stats.maxHealth = 108.0f;
        stats.attackDamage = 11.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Jad - KFC rage
class JadCharacter : public Character {
public:
    JadCharacter() : Character(CharacterID::Jad) {
        name = "Jad";
        ability = {
            "KFC Rage",
            "ik eet",
            {{AbilityEffect::SpeedBoost, 2.2f}, {AbilityEffect::DamageBoost, 2.5f}},
            6.0f,
            20.0f,
            "KFC rage incarnate. When triggered, unleashes chicken-fueled fury for massive boosts."
        };
        stats.maxHealth = 112.0f;
        stats.attackDamage = 12.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Umut - Terraria arc
class UmutCharacter : public Character {
public:
    UmutCharacter() : Character(CharacterID::Umut) {
        name = "Umut";
        ability = {
            "Terraria Arc",
            "Ik ga nu in mijn terraria arc",
            {{AbilityEffect::DamageBoost, 2.3f}},
            5.0f,
            13.0f,
            "Master builder from Terraria. Enters his building arc for serious damage boosts."
        };
        stats.maxHealth = 102.0f;
        stats.attackDamage = 11.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

// Keizer Bom Taha - Aerial superiority
class KeizerBomTahaCharacter : public Character {
public:
    KeizerBomTahaCharacter() : Character(CharacterID::KeizerBomTaha) {
        name = "Keizer Bom Taha";
        ability = {
            "Lucht Aanval",
            "ik gooi bommen!",
            {{AbilityEffect::PlaneSummon, 1.0f}},
            15.0f,
            25.0f,
            "Military commander with aerial superiority. Summons plane for devastating bomb barrages."
        };
        stats.maxHealth = 130.0f;
        stats.attackDamage = 13.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
    void updateAbility(float dt) override;
private:
    Ogre::SceneNode* planeNode = nullptr;
};

// Goon Lord Tobese - Special Milk power
class GoonLordTobeseCharacter : public Character {
public:
    GoonLordTobeseCharacter() : Character(CharacterID::GoonLordTobese) {
        name = "Goon Lord Tobese";
        ability = {
            "Speciale Melk",
            "Ik hou van padme!",
            {{AbilityEffect::HealthBoost, 40.0f}, {AbilityEffect::SpeedBoost, 1.8f}, {AbilityEffect::DamageBoost, 1.6f}},
            7.0f,
            14.0f,
            "Master of Special Milk power. Balanced fighter with strong buffs from dairy supremacy."
        };
        stats.maxHealth = 118.0f;
        stats.attackDamage = 11.0f;
        currentHealth = stats.maxHealth;
    }
protected:
    void onAbilityActivated() override;
};

} // namespace BVA
