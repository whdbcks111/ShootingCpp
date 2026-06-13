// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossEnemy.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemy;

UCLASS()
class SHOOTINGCPP_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type endPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	class AShootingPlayer* player;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AEnemy> enemyFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ABossEnemy> bossEnemyFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float spawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float minSpawnInterval = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float spawnIntervalDecreasePerMinute = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int maxEnemyCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float spawnX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float minSpawnY = -500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float maxSpawnY = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	float spawnZ = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	float enemyMoveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	float maxEnemyMoveSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	float enemySpeedIncreasePerMinute = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficulty")
	float enemyMaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	int32 killsToSpawnBoss = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float bossSpawnX = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float bossSpawnY = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float bossSpawnZ = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float bossMoveSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float bossMaxHealth = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UGameHudWidget> gameHudFactory;
	
	UPROPERTY()
	UGameHudWidget* gameHud = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StartSpawn();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void StopSpawn();

	void SetPlayer(class AShootingPlayer* newPlayer);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void TrySpawnEnemy();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AEnemy* SpawnEnemyAtLocation(FVector spawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void TrySpawnBoss();

	UFUNCTION(BlueprintPure, Category = "Spawn")
	int32 GetAliveEnemyCount() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty")
	float GetCurrentSpawnInterval() const;

	UFUNCTION(BlueprintPure, Category = "Difficulty")
	float GetCurrentEnemyMoveSpeed() const;
	
	UFUNCTION()
	void HandleEnemyDied(AEnemy* enemy);
	
	UFUNCTION()
	void UpdateBossHp(AEnemy* enemy);

private:

	void RegisterSpawnedEnemy(AEnemy* enemy, bool isBoss);
	void BroadcastEnemyState();
	void CompactEnemyList();

	float startTime = 0.0f;
	float spawnTimer = 0.0f;
	int killCount = 0;
	bool isSpawning = false;
	int nextBossKillTarget = 0;
	
	bool isBossSpawned = false;
	bool isGameCleared = false;

	UPROPERTY()
	ABossEnemy* bossEnemy = nullptr;

	TArray<AEnemy*> aliveEnemies;
};
