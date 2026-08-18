#include "EnemyManager.h"
#include "Application\Engine.h"
void EnemyManager::Init(Object2D* Player, f32 SpawnInterval)
{
	SpawnTimer = SpawnInterval;
	Counter = 0;
	for (u32 i = 0; i < ENEMY_LIMIT;i++)
	{
		List[i].Init(Player);
		List[i].Die();
		DeadList[i] = &List[i];
	}
	DeadMeleeCount = ENEMY_LIMIT - 1;
}
void EnemyManager::SpawnMelee()
{
	if (Counter > SpawnTimer && LiveMeleeCount < ENEMY_LIMIT - 1)
	{
		DeadList[DeadMeleeCount]->Spawn();
		AliveList[LiveMeleeCount++] = DeadList[DeadMeleeCount--];
		Counter = 0;
		printf("Spawned a Enemy\n");
	}
}
void EnemyManager::Update()
{
	Counter += GEngine.GetFrameTime();
	for (u32 i = 0; i < LiveMeleeCount;i++)
	{
		AliveList[i]->Update();
		if (!AliveList[i]->Alive)
		{
			DeadMeleeCount++;
			DeadList[DeadMeleeCount] = AliveList[i];
			memcpy(AliveList + i, AliveList + i + 1, sizeof(MeleeEnemy*) * (ENEMY_LIMIT - i));
			LiveMeleeCount--;
		}
	}
	SpawnMelee();

}
void EnemyManager::Draw()
{
	for (u32 i = 0; i < LiveMeleeCount;i++)
	{
		AliveList[i]->Draw();
	}
}