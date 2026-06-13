// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossEnemy.generated.h"

class AEnemySpawner;

UCLASS()
class SHOOTINGCPP_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	ABossEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void ApplyHitDamage(float DamageAmount) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Pattern")
	FString currentPattern = TEXT("Basic");

	UFUNCTION(BlueprintCallable, Category = "Boss Pattern")
	void SetPattern(const FString& nextPattern);

	void SetEnemySpawner(AEnemySpawner* spawner);

private:
	void SpawnEnemy(FVector Location);
	void BasicPattern(float DeltaTime);
	void RandomFirePattern(float DeltaTime);
	void EnemySpawnPattern(float DeltaTime);
	void RapidPattern(float DeltaTime);

	FVector patternStartLocation = FVector::ZeroVector;
	float patternElapsedTime = 0.0f;
	float basicFireElapsedTime = 0.0f;

	UPROPERTY()
	AEnemySpawner* enemySpawner = nullptr;
};
