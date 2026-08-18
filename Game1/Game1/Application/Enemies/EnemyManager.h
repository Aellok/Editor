#pragma once
#include "MeleeEnemy.h"
#include "System\Types.h"
#define ENEMY_LIMIT 32

struct EnemyManager
{
	f32 Counter;
	f32 SpawnTimer;
	MeleeEnemy List[ENEMY_LIMIT];

	u32 LiveMeleeCount = 0;
	MeleeEnemy* AliveList[ENEMY_LIMIT];
	u32 DeadMeleeCount = 0;
	MeleeEnemy* DeadList[ENEMY_LIMIT];
	void Init(Object2D* Player,f32 SpawnInterval);
	void SpawnMelee();
	void Update();
	void Draw();
};