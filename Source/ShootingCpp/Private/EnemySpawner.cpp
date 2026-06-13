// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "Enemy.h"
#include "GameHudWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ShootingPlayer.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	startTime = GetWorld() != nullptr ? GetWorld()->GetTimeSeconds() : 0.0f;
	nextBossKillTarget = killsToSpawnBoss;
	if (player == nullptr)
	{
		player = Cast<AShootingPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	}
	
	if (gameHudFactory != nullptr)
	{
		gameHud = CreateWidget<UGameHudWidget>(GetWorld(), gameHudFactory);
		if (gameHud != nullptr)
		{
			gameHud->AddToViewport();
			if (player != nullptr)
			{
				player->SetGameHud(gameHud);
			}
		}
	}

	BroadcastEnemyState();
	StartSpawn();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isSpawning)
	{
		return;
	}

	spawnTimer += DeltaTime;
	if (spawnTimer >= GetCurrentSpawnInterval())
	{
		spawnTimer = 0.0f;
		TrySpawnEnemy();
	}
}

void AEnemySpawner::EndPlay(const EEndPlayReason::Type endPlayReason)
{
	StopSpawn();

	Super::EndPlay(endPlayReason);
}

void AEnemySpawner::StartSpawn()
{
	if (isSpawning)
	{
		return;
	}

	isSpawning = true;
	spawnTimer = 0.0f;
}

void AEnemySpawner::StopSpawn()
{
	isSpawning = false;
	spawnTimer = 0.0f;
}

void AEnemySpawner::SetPlayer(AShootingPlayer* newPlayer)
{
	player = newPlayer;
	if (player != nullptr && gameHud != nullptr)
	{
		player->SetGameHud(gameHud);
	}
}

void AEnemySpawner::TrySpawnEnemy()
{
	if (!isSpawning)
	{
		return;
	}

	if (killsToSpawnBoss > 0 && !isBossSpawned && killCount >= nextBossKillTarget)
	{
		StopSpawn();
		TrySpawnBoss();
		isBossSpawned = true;
		BroadcastEnemyState();
		return;
	}

	UWorld* world = GetWorld();
	if (world == nullptr || enemyFactory == nullptr)
	{
		return;
	}

	if (GetAliveEnemyCount() < maxEnemyCount)
	{
		const float spawnY = FMath::RandRange(minSpawnY, maxSpawnY);
		const FVector spawnLocation = FVector(spawnX, spawnY, spawnZ);
		SpawnEnemyAtLocation(spawnLocation);
	}
}

AEnemy* AEnemySpawner::SpawnEnemyAtLocation(FVector spawnLocation)
{
	UWorld* world = GetWorld();
	if (world == nullptr || enemyFactory == nullptr)
	{
		return nullptr;
	}

	if (GetAliveEnemyCount() >= maxEnemyCount)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemy* enemy = world->SpawnActor<AEnemy>(enemyFactory, spawnLocation, FRotator::ZeroRotator, spawnParams);
	RegisterSpawnedEnemy(enemy, false);

	return enemy;
}

void AEnemySpawner::TrySpawnBoss()
{
	if (bossEnemy != nullptr || bossEnemyFactory == nullptr)
	{
		return;
	}

	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	const FVector spawnLocation = FVector(bossSpawnX, bossSpawnY, bossSpawnZ);
	const FRotator spawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABossEnemy* spawnedBoss = world->SpawnActor<ABossEnemy>(bossEnemyFactory, spawnLocation, spawnRotation, spawnParams);
	if (spawnedBoss != nullptr)
	{
		spawnedBoss->SetEnemySpawner(this);
	}

	RegisterSpawnedEnemy(spawnedBoss, true);
}

int AEnemySpawner::GetAliveEnemyCount() const
{
	int count = 0;
	for (AEnemy* enemy : aliveEnemies)
	{
		if (IsValid(enemy))
		{
			++count;
		}
	}

	return count;
}

float AEnemySpawner::GetCurrentSpawnInterval() const
{
	const UWorld* world = GetWorld();
	const float elapsedTime = world != nullptr ? world->GetTimeSeconds() - startTime : 0.0f;
	const float difficultyMinutes = FMath::Max(0.0f, elapsedTime / 60.0f);
	const float currentInterval = spawnInterval - (spawnIntervalDecreasePerMinute * difficultyMinutes);

	return FMath::Max(minSpawnInterval, currentInterval);
}

float AEnemySpawner::GetCurrentEnemyMoveSpeed() const
{
	const UWorld* world = GetWorld();
	const float elapsedTime = world != nullptr ? world->GetTimeSeconds() - startTime : 0.0f;
	const float difficultyMinutes = FMath::Max(0.0f, elapsedTime / 60.0f);
	const float currentMoveSpeed = enemyMoveSpeed + (enemySpeedIncreasePerMinute * difficultyMinutes);

	return FMath::Min(maxEnemyMoveSpeed, currentMoveSpeed);
}

void AEnemySpawner::HandleEnemyDied(AEnemy* enemy)
{
	if (enemy == nullptr)
	{
		return;
	}

	++killCount;
	aliveEnemies.Remove(enemy);

	if (enemy == Cast<AEnemy>(bossEnemy))
	{
		if (gameHud != nullptr)
		{
			gameHud->UpdateBossHp(0.0f);
			gameHud->UpdateBossBarVisible(false);
			gameHud->UpdateGameCleared(true);
		}
		bossEnemy = nullptr;
	}

	CompactEnemyList();

	if (killsToSpawnBoss > 0 && bossEnemy == nullptr && !isBossSpawned && killCount >= nextBossKillTarget)
	{
		StopSpawn();
		TrySpawnBoss();

		isBossSpawned = true;
	}

	BroadcastEnemyState();
}

void AEnemySpawner::UpdateBossHp(AEnemy* enemy)
{
	if (enemy == nullptr || enemy != bossEnemy || gameHud == nullptr)
	{
		return;
	}
	
	if(enemy->MaxHealth <= 0.0f)
	{
		return;
	}
	
	gameHud->UpdateBossHp(enemy->CurrentHealth / enemy->MaxHealth);
}

void AEnemySpawner::RegisterSpawnedEnemy(AEnemy* enemy, bool isBoss)
{
	if (enemy == nullptr)
	{
		return;
	}

	const float spawnedMoveSpeed = isBoss ? bossMoveSpeed : GetCurrentEnemyMoveSpeed();
	const float spawnedMaxHealth = isBoss ? bossMaxHealth : enemyMaxHealth;
	enemy->InitializeEnemy(spawnedMoveSpeed, spawnedMaxHealth, isBoss, player);

	aliveEnemies.Add(enemy);

	if (isBoss)
	{
		bossEnemy = Cast<ABossEnemy>(enemy);
		if (gameHud != nullptr)
		{
			gameHud->UpdateBossBarVisible(true);
			gameHud->UpdateBossHp(1.0f);
		}
	}

	BroadcastEnemyState();
}

void AEnemySpawner::BroadcastEnemyState()
{
	CompactEnemyList();
	if (gameHud != nullptr)
	{
		gameHud->UpdateSpawnState(GetAliveEnemyCount(), maxEnemyCount, killCount);
	}
}

void AEnemySpawner::CompactEnemyList()
{
	aliveEnemies.RemoveAll([](AEnemy* enemy)
	{
		return !IsValid(enemy);
	});
}
