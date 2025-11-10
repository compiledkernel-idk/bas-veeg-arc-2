#include "gameplay/Boss.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace BVA {

Boss::Boss(BossType type) : Character(CharacterID::Bas), bossType(type) {
    // Boss-specific initialization
}

void Boss::update(float dt) {
    // Call base character update
    Character::update(dt);

    if (inIntro) {
        introTimer += dt;
        if (introTimer >= 3.0f) {
            inIntro = false;
            std::cout << name << " battle begins!" << std::endl;
        }
        return;
    }

    // Update attack cooldown
    if (attackCooldown > 0.0f) {
        attackCooldown -= dt;
    }

    // Update attack windup
    if (attackWindup > 0.0f) {
        attackWindup -= dt;
        if (attackWindup <= 0.0f && currentAttack) {
            executeAttack(*currentAttack);
            currentAttack = nullptr;
        }
    }

    // Update AI
    updateAI(dt);
}

void Boss::takeDamage(float damage, Character* attacker) {
    Character::takeDamage(damage, attacker);

    float healthPercent = getHealthPercent();

    // Phase transitions
    if (!hasEnteredPhase2 && healthPercent <= phase2HealthPercent) {
        hasEnteredPhase2 = true;
        transitionToPhase(BossPhase::Phase2);
    } else if (!hasEnteredPhase3 && healthPercent <= phase3HealthPercent) {
        hasEnteredPhase3 = true;
        transitionToPhase(BossPhase::Phase3);
    }

    targetPlayer = attacker;
}

void Boss::startBattle() {
    inIntro = true;
    introTimer = 0.0f;
    playIntro();
}

void Boss::updateAI(float dt) {
    // Base AI: attack when cooldown is ready
    if (attackCooldown <= 0.0f && !currentAttack && !availableAttacks.empty()) {
        selectNextAttack();
    }
}

void Boss::selectNextAttack() {
    if (availableAttacks.empty()) return;

    // Simple random selection
    int attackIndex = rand() % availableAttacks.size();
    currentAttack = &availableAttacks[attackIndex];
    attackWindup = currentAttack->windupTime;
    attackCooldown = currentAttack->cooldown;

    std::cout << name << " prepares " << currentAttack->name << "!" << std::endl;
}

void Boss::executeAttack(const BossAttack& attack) {
    std::cout << name << " uses " << attack.name << "!" << std::endl;

    if (attack.executeFunc) {
        attack.executeFunc(this, targetPlayer);
    }
}

void Boss::onPhaseChange(BossPhase newPhase) {
    std::cout << name << " enters phase " << (int)newPhase + 1 << "!" << std::endl;
}

void Boss::playIntro() {
    std::cout << "\n=== BOSS BATTLE ===" << std::endl;
    std::cout << name << std::endl;
    std::cout << introText << std::endl;
    std::cout << "===================" << std::endl;
}

void Boss::addAttack(const BossAttack& attack) {
    availableAttacks.push_back(attack);
}

void Boss::transitionToPhase(BossPhase phase) {
    currentPhase = phase;
    onPhaseChange(phase);
}

// ===== Boss Implementations =====

// Bastiaan - The Artist
BastiaanBoss::BastiaanBoss() : Boss(BossType::Bastiaan) {
    name = "Bastiaan";
    introText = "You dare interrupt my artistic vision? Prepare to become part of my masterpiece!";

    stats.maxHealth = 800.0f;
    stats.moveSpeed = 4.0f;
    stats.attackDamage = 15.0f;
    stats.defense = 5.0f;
    currentHealth = stats.maxHealth;

    // Paint Splash Attack
    addAttack({
        "Paint Splash",
        20.0f,
        8.0f,
        3.0f,
        0.5f,
        true,
        false,
        [](Boss* boss, Character* target) {
            if (target) {
                target->takeDamage(20.0f, boss);
                std::cout << "Bastiaan splashes paint everywhere!" << std::endl;
            }
        }
    });

    // Canvas Throw
    addAttack({
        "Canvas Throw",
        30.0f,
        15.0f,
        4.0f,
        1.0f,
        false,
        true,
        [](Boss* boss, Character* target) {
            if (target) {
                target->takeDamage(30.0f, boss);
                std::cout << "Bastiaan hurls a heavy canvas!" << std::endl;
            }
        }
    });
}

void BastiaanBoss::updateAI(float dt) {
    Boss::updateAI(dt);
    // Custom AI behaviors
}

void BastiaanBoss::onPhaseChange(BossPhase newPhase) {
    Boss::onPhaseChange(newPhase);
    if (newPhase == BossPhase::Phase2) {
        std::cout << "Bastiaan: \"You've ruined my canvas! Now you'll pay!\"" << std::endl;
        stats.attackDamage *= 1.3f;
    }
}

// Keizer Bom Taha Boss Version
KeizerBomTahaBoss::KeizerBomTahaBoss() : Boss(BossType::KeizerBomTaha) {
    name = "Keizer Bom Taha";
    introText = "I am the supreme military commander! You will fall before my aerial bombardment!";

    stats.maxHealth = 900.0f;
    stats.moveSpeed = 4.5f;
    stats.attackDamage = 18.0f;
    stats.defense = 8.0f;
    currentHealth = stats.maxHealth;

    // Bomb Drop
    addAttack({
        "Aerial Bombardment",
        40.0f,
        12.0f,
        5.0f,
        2.0f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "Bombs rain from above!" << std::endl;
            if (target) {
                target->takeDamage(40.0f, boss);
            }
        }
    });

    // Ground Pound
    addAttack({
        "Ground Pound",
        25.0f,
        8.0f,
        3.5f,
        0.8f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "Keizer Bom Taha slams the ground!" << std::endl;
            if (target) {
                target->takeDamage(25.0f, boss);
            }
        }
    });
}

void KeizerBomTahaBoss::updateAI(float dt) {
    Boss::updateAI(dt);
}

void KeizerBomTahaBoss::onPhaseChange(BossPhase newPhase) {
    Boss::onPhaseChange(newPhase);
    if (newPhase == BossPhase::Phase2) {
        std::cout << "Keizer Bom Taha: \"Calling in reinforcements!\"" << std::endl;
    } else if (newPhase == BossPhase::Phase3) {
        std::cout << "Keizer Bom Taha: \"Maximum bombardment!\"" << std::endl;
        stats.attackDamage *= 1.5f;
    }
}

// Mees - Pita Sirracha Thrower
MeesBoss::MeesBoss() : Boss(BossType::Mees) {
    name = "Mees";
    introText = "You want some pita? Here, have ALL the pitas!";

    stats.maxHealth = 750.0f;
    stats.moveSpeed = 5.0f;
    stats.attackDamage = 12.0f;
    stats.defense = 3.0f;
    currentHealth = stats.maxHealth;

    // Pita Throw
    addAttack({
        "Pita Sirracha Barrage",
        15.0f,
        20.0f,
        2.0f,
        0.3f,
        false,
        true,
        [](Boss* boss, Character* target) {
            std::cout << "Mees throws burning hot pitas!" << std::endl;
            if (target) {
                target->takeDamage(15.0f, boss);
                target->applyFireDamage(3.0f, 4.0f);
            }
        }
    });
}

void MeesBoss::updateAI(float dt) {
    Boss::updateAI(dt);
    // Mees is aggressive and fast
}

void MeesBoss::throwPitaSirracha() {
    std::cout << "Pita incoming!" << std::endl;
}

// Principal Van Der Berg - 3-Phase Authority Figure
PrincipalVanDerBergBoss::PrincipalVanDerBergBoss() : Boss(BossType::PrincipalVanDerBerg) {
    name = "Principal Van Der Berg";
    introText = "Students! You will respect my AUTHORITY!";

    stats.maxHealth = 1200.0f;
    stats.moveSpeed = 3.5f;
    stats.attackDamage = 20.0f;
    stats.defense = 10.0f;
    currentHealth = stats.maxHealth;

    // Authority Shout
    addAttack({
        "Authority Shout",
        25.0f,
        15.0f,
        4.0f,
        1.0f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "SILENCE! You will obey!" << std::endl;
            if (target) {
                target->takeDamage(25.0f, boss);
                target->applySpeedBoost(0.5f, 3.0f); // Slow debuff
            }
        }
    });

    // Ruler Smack
    addAttack({
        "Ruler Smack",
        35.0f,
        5.0f,
        3.0f,
        0.5f,
        false,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "Principal Van Der Berg swings his massive ruler!" << std::endl;
            if (target) {
                target->takeDamage(35.0f, boss);
            }
        }
    });
}

void PrincipalVanDerBergBoss::updateAI(float dt) {
    Boss::updateAI(dt);

    // Build authority meter over time
    authorityMeter += dt * 10.0f;
    if (authorityMeter >= 100.0f) {
        summonPrefects();
        authorityMeter = 0.0f;
    }
}

void PrincipalVanDerBergBoss::onPhaseChange(BossPhase newPhase) {
    Boss::onPhaseChange(newPhase);
    if (newPhase == BossPhase::Phase2) {
        std::cout << "Van Der Berg: \"You dare challenge me? Prefects, to me!\"" << std::endl;
        summonPrefects();
    } else if (newPhase == BossPhase::Phase3) {
        std::cout << "Van Der Berg: \"ENOUGH! I will handle this myself!\"" << std::endl;
        stats.attackDamage *= 1.8f;
        stats.moveSpeed *= 1.3f;
    }
}

void PrincipalVanDerBergBoss::summonPrefects() {
    std::cout << "Principal Van Der Berg summons prefects!" << std::endl;
    prefectsSummoned++;
    // TODO: Spawn actual prefect enemies
}

// Janitor King - Master of Mop Combat
JanitorKingBoss::JanitorKingBoss() : Boss(BossType::JanitorKing) {
    name = "The Janitor King";
    introText = "You kids made a mess... and now I'll clean YOU up!";

    stats.maxHealth = 1500.0f;
    stats.moveSpeed = 4.0f;
    stats.attackDamage = 22.0f;
    stats.defense = 12.0f;
    currentHealth = stats.maxHealth;

    // Mop Swing
    addAttack({
        "Mop Whirlwind",
        30.0f,
        10.0f,
        3.5f,
        0.8f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "The Janitor King spins his legendary mop!" << std::endl;
            if (target) {
                target->takeDamage(30.0f, boss);
            }
        }
    });

    // Wet Floor
    addAttack({
        "Slippery Floor",
        10.0f,
        20.0f,
        6.0f,
        1.5f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "Watch out! The floor is wet!" << std::endl;
            if (target) {
                target->takeDamage(10.0f, boss);
                target->applySpeedBoost(0.3f, 5.0f); // Major slow
            }
        }
    });
}

void JanitorKingBoss::updateAI(float dt) {
    Boss::updateAI(dt);
}

void JanitorKingBoss::onPhaseChange(BossPhase newPhase) {
    Boss::onPhaseChange(newPhase);
    if (newPhase == BossPhase::Phase2) {
        std::cout << "Janitor King: \"Time to bring out the industrial cleaner!\"" << std::endl;
        createSlipperyFloor();
    } else if (newPhase == BossPhase::Phase3) {
        std::cout << "Janitor King: \"You've really made me angry now!\"" << std::endl;
        stats.attackDamage *= 1.5f;
        createSlipperyFloor();
    }
}

void JanitorKingBoss::createSlipperyFloor() {
    std::cout << "The entire floor becomes dangerously slippery!" << std::endl;
    // TODO: Create actual slippery zones
    slipperyZones.push_back(Ogre::Vector3(0, 0, 0));
}

// Head Chef Ramsey - Culinary Combat Expert
HeadChefBoss::HeadChefBoss() : Boss(BossType::HeadChef) {
    name = "Head Chef Ramsey";
    introText = "This cafeteria is MY KITCHEN! And you're BURNT!";

    stats.maxHealth = 1000.0f;
    stats.moveSpeed = 5.0f;
    stats.attackDamage = 18.0f;
    stats.defense = 6.0f;
    currentHealth = stats.maxHealth;

    // Pan Throw
    addAttack({
        "Burning Pan",
        28.0f,
        12.0f,
        2.5f,
        0.4f,
        false,
        true,
        [](Boss* boss, Character* target) {
            std::cout << "Chef Ramsey throws a red-hot frying pan!" << std::endl;
            if (target) {
                target->takeDamage(28.0f, boss);
                target->applyFireDamage(5.0f, 3.0f);
            }
        }
    });

    // Food Barrage
    addAttack({
        "IT'S RAW!",
        20.0f,
        15.0f,
        3.0f,
        0.6f,
        true,
        false,
        [](Boss* boss, Character* target) {
            std::cout << "WHERE'S THE LAMB SAUCE?!" << std::endl;
            if (target) {
                target->takeDamage(20.0f, boss);
            }
        }
    });
}

void HeadChefBoss::updateAI(float dt) {
    Boss::updateAI(dt);

    // Enter rage mode at low health
    if (!inRageMode && getHealthPercent() < 0.3f) {
        inRageMode = true;
        std::cout << "Chef Ramsey: \"YOU DONKEY! I'LL SHOW YOU REAL COOKING!\"" << std::endl;
        stats.attackDamage *= 2.0f;
        stats.moveSpeed *= 1.5f;
    }
}

void HeadChefBoss::onPhaseChange(BossPhase newPhase) {
    Boss::onPhaseChange(newPhase);
    if (newPhase == BossPhase::Phase2) {
        std::cout << "Chef Ramsey: \"This dish is DISGUSTING!\"" << std::endl;
        throwFood();
    }
}

void HeadChefBoss::throwFood() {
    std::cout << "Chef throws various kitchen items!" << std::endl;
}

// Boss Factory
std::unique_ptr<Boss> createBoss(BossType type) {
    switch (type) {
        case BossType::Bastiaan:
            return std::make_unique<BastiaanBoss>();
        case BossType::KeizerBomTaha:
            return std::make_unique<KeizerBomTahaBoss>();
        case BossType::Mees:
            return std::make_unique<MeesBoss>();
        case BossType::PrincipalVanDerBerg:
            return std::make_unique<PrincipalVanDerBergBoss>();
        case BossType::JanitorKing:
            return std::make_unique<JanitorKingBoss>();
        case BossType::HeadChef:
            return std::make_unique<HeadChefBoss>();
        default:
            return nullptr;
    }
}

} // namespace BVA
