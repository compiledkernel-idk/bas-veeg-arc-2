// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#pragma once

#include "Character.hpp"
#include <vector>
#include <functional>

namespace BVA {

enum class BossType {
    Bastiaan,
    KeizerBomTaha,
    Mees,
    PrincipalVanDerBerg,
    JanitorKing,
    HeadChef
};

enum class BossPhase {
    Phase1,
    Phase2,
    Phase3
};

struct BossAttack {
    std::string name;
    float damage;
    float range;
    float cooldown;
    float windupTime;
    bool isAOE;
    bool isProjectile;
    std::function<void(class Boss*, Character*)> executeFunc;
};

class Boss : public Character {
public:
    Boss(BossType type);
    virtual ~Boss() = default;

    void update(float dt) override;
    void takeDamage(float damage, Character* attacker = nullptr) override;

    // Boss-specific
    BossType getBossType() const { return bossType; }
    BossPhase getCurrentPhase() const { return currentPhase; }
    const std::string& getIntroText() const { return introText; }
    bool isInIntro() const { return inIntro; }
    void startBattle();

protected:
    virtual void updateAI(float dt);
    virtual void selectNextAttack();
    virtual void executeAttack(const BossAttack& attack);
    virtual void onPhaseChange(BossPhase newPhase);
    virtual void playIntro();

    void addAttack(const BossAttack& attack);
    void transitionToPhase(BossPhase phase);

    BossType bossType;
    BossPhase currentPhase = BossPhase::Phase1;
    std::string introText;

    std::vector<BossAttack> availableAttacks;
    BossAttack* currentAttack = nullptr;
    float attackCooldown = 0.0f;
    float attackWindup = 0.0f;

    bool inIntro = true;
    float introTimer = 0.0f;
    Character* targetPlayer = nullptr;

    // Phase thresholds
    float phase2HealthPercent = 0.66f;
    float phase3HealthPercent = 0.33f;
    bool hasEnteredPhase2 = false;
    bool hasEnteredPhase3 = false;
};

// Boss implementations
class BastiaanBoss : public Boss {
public:
    BastiaanBoss();
protected:
    void updateAI(float dt) override;
    void onPhaseChange(BossPhase newPhase) override;
};

class KeizerBomTahaBoss : public Boss {
public:
    KeizerBomTahaBoss();
protected:
    void updateAI(float dt) override;
    void onPhaseChange(BossPhase newPhase) override;
};

class MeesBoss : public Boss {
public:
    MeesBoss();
protected:
    void updateAI(float dt) override;
    void throwPitaSirracha();
};

class PrincipalVanDerBergBoss : public Boss {
public:
    PrincipalVanDerBergBoss();
protected:
    void updateAI(float dt) override;
    void onPhaseChange(BossPhase newPhase) override;
    void summonPrefects();
private:
    int prefectsSummoned = 0;
    float authorityMeter = 0.0f;
};

class JanitorKingBoss : public Boss {
public:
    JanitorKingBoss();
protected:
    void updateAI(float dt) override;
    void onPhaseChange(BossPhase newPhase) override;
    void createSlipperyFloor();
private:
    std::vector<Ogre::Vector3> slipperyZones;
};

class HeadChefBoss : public Boss {
public:
    HeadChefBoss();
protected:
    void updateAI(float dt) override;
    void onPhaseChange(BossPhase newPhase) override;
    void throwFood();
private:
    bool inRageMode = false;
};

// Boss factory
std::unique_ptr<Boss> createBoss(BossType type);

} // namespace BVA
