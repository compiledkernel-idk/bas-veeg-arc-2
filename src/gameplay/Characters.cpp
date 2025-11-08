#include "gameplay/Characters.hpp"
#include <iostream>

namespace BVA {

// Bas - AOE splash damage specialist
void BasCharacter::onAbilityActivated() {
    // Create massive AOE splash damage
    applySplashDamage(45.0f, 180.0f);

    // Visual effect (massive shockwave)
    std::cout << "BAS VEEG creates massive shockwave!" << std::endl;
    // TODO: Create particle effect
}

// Berkay - Damage and health boost
void BerkayCharacter::onAbilityActivated() {
    applyDamageBoost(2.2f, 6.0f);
    heal(35.0f);
    std::cout << "Berkay gains kebab power!" << std::endl;
}

// Luca - Massive damage boost
void LucaCharacter::onAbilityActivated() {
    applyDamageBoost(2.8f, 5.0f);
    heal(15.0f);
    std::cout << "Luca enters winter arc mode!" << std::endl;
}

// Gefferinho - Balanced all-around boost
void GefferinhoCharacter::onAbilityActivated() {
    applySpeedBoost(2.0f, 6.0f);
    applyDamageBoost(2.0f, 6.0f);
    heal(25.0f);
    std::cout << "Gefferinho goes into rage mode!" << std::endl;
}

// Hadi - Speed demon
void HadiCharacter::onAbilityActivated() {
    applySpeedBoost(3.2f, 6.0f);
    applyDamageBoost(1.5f, 6.0f);
    std::cout << "Hadi activates Dubai Emirates speed!" << std::endl;
}

// Nitin - Fire damage over time
void NitinCharacter::onAbilityActivated() {
    // Apply fire damage to self and nearby enemies
    applyFireDamage(8.0f, 6.0f);
    std::cout << "Nitin ignites with fire damage!" << std::endl;
    // TODO: Apply fire to nearby enemies
}

// PalaBaba - THE ULTIMATE FIGHTER
void PalaBabaCharacter::onAbilityActivated() {
    applyDamageBoost(3.0f, 10.0f);
    applySpeedBoost(3.0f, 10.0f);
    heal(30.0f);
    std::cout << "TURKIYEEEE! PalaBaba unleashes ultimate power!" << std::endl;
}

// Fufinho - Projectile attack
void FufinhoCharacter::onAbilityActivated() {
    // Create projectile
    std::cout << "Fufinho throws massive fufu bomb (80 damage)!" << std::endl;
    // TODO: Create actual projectile entity
}

// EfeAbi - Speed and damage from lahmacun
void EfeAbiCharacter::onAbilityActivated() {
    applySpeedBoost(2.5f, 5.0f);
    applyDamageBoost(2.0f, 5.0f);
    std::cout << "Efe abi powered up by lahmacun!" << std::endl;
}

// Jad - KFC rage
void JadCharacter::onAbilityActivated() {
    applySpeedBoost(2.2f, 6.0f);
    applyDamageBoost(2.5f, 6.0f);
    std::cout << "Jad enters KFC rage mode!" << std::endl;
}

// Umut - Terraria arc damage boost
void UmutCharacter::onAbilityActivated() {
    applyDamageBoost(2.3f, 5.0f);
    std::cout << "Umut enters Terraria arc!" << std::endl;
}

// Keizer Bom Taha - Summon plane for aerial bombardment
void KeizerBomTahaCharacter::onAbilityActivated() {
    std::cout << "Keizer Bom Taha summons military plane!" << std::endl;
    // TODO: Create plane entity and bombing logic
}

void KeizerBomTahaCharacter::updateAbility(float dt) {
    if (planeNode && abilityActiveTimer > 0.0f) {
        // Move plane and drop bombs
        // TODO: Implement plane movement and bombing
    }
}

// Goon Lord Tobese - Special Milk balanced buffs
void GoonLordTobeseCharacter::onAbilityActivated() {
    heal(40.0f);
    applySpeedBoost(1.8f, 7.0f);
    applyDamageBoost(1.6f, 7.0f);
    std::cout << "Goon Lord Tobese gains power of Special Milk!" << std::endl;
}

} // namespace BVA
