// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

#include "Components/BoxComponent.h"
#include "EnemySpawner.h"
#include "ShootingPlayer.h"

ABossEnemy::ABossEnemy()
{
	isBoss = true;
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	patternStartLocation = GetActorLocation();
	patternElapsedTime = 0.0f;
}

void ABossEnemy::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);

	patternElapsedTime += DeltaTime;
	
	FVector P0 = GetActorLocation();
	FVector moveDir = FVector::DownVector;
	if (IsValid(targetPlayer))
	{
		moveDir = targetPlayer->GetActorLocation() - P0;
		moveDir.X = 0.0f;
		moveDir.Normalize();
	}
	
	float angle = FMath::Atan2(moveDir.Y, -moveDir.Z);
	float degree = FMath::RadiansToDegrees(angle);
	
	boxComp->SetRelativeRotation(FRotator(0.0f, 0.0f,  -degree ));
	
	if (currentPattern == TEXT("Basic"))
	{
		BasicPattern(DeltaTime);
	}
	else if (currentPattern == TEXT("RandomFire"))
	{
		RandomFirePattern(DeltaTime);
	}
	else if (currentPattern == TEXT("EnemySpawn"))
	{
		EnemySpawnPattern(DeltaTime);
	}
	else if (currentPattern == TEXT("Rapid"))
	{
		RapidPattern(DeltaTime);
	}
	else
	{
		SetPattern(TEXT("Basic"));
	}
}

void ABossEnemy::SetPattern(const FString& nextPattern)
{
	currentPattern = nextPattern;
	patternStartLocation = GetActorLocation();
	patternElapsedTime = 0.0f;
}

void ABossEnemy::SetEnemySpawner(AEnemySpawner* spawner)
{
	enemySpawner = spawner;
}

void ABossEnemy::ApplyHitDamage(float DamageAmount)
{
	Super::ApplyHitDamage(DamageAmount);
	
	if (enemySpawner != nullptr)
	{
		enemySpawner->UpdateBossHp(this);
	}
}

void ABossEnemy::SpawnEnemy(FVector loc)
{
	if (enemySpawner == nullptr || FirePosition == nullptr)
	{
		return;
	}

	enemySpawner->SpawnEnemyAtLocation(loc);
}

void ABossEnemy::BasicPattern(float DeltaTime)
{
	basicFireElapsedTime += DeltaTime;
	
	if (basicFireElapsedTime >= fireInterval)
	{
		basicFireElapsedTime -= fireInterval;
		Fire();
	}
	
	if (patternElapsedTime >= 10.0f)
	{
		SetPattern(TEXT("RandomFire"));
	}
}

void ABossEnemy::RandomFirePattern(float DeltaTime)
{
	basicFireElapsedTime += DeltaTime;
	
	SetActorLocation(patternStartLocation + 
		FVector(0.0f, FMath::Sin(patternElapsedTime * PI) * 300, 0.0f));
	
	if (basicFireElapsedTime >= fireInterval * 0.3f)
	{
		basicFireElapsedTime -= fireInterval * 0.3f;
		FireTo(FVector(0.0f, FMath::RandRange(-2.0f, 2.0f), -1.0f));
	}
	
	if (patternElapsedTime >= 5.0f)
	{
		SetPattern(TEXT("EnemySpawn"));
	}
	
}

void ABossEnemy::EnemySpawnPattern(float DeltaTime)
{
	basicFireElapsedTime += DeltaTime;
	
	if (basicFireElapsedTime >= fireInterval * 2.f)
	{
		basicFireElapsedTime -= fireInterval * 2.f;
		for (int i = 0; i < 3; ++i) SpawnEnemy(
			FirePosition->GetComponentLocation() 
			+ FVector(0.0f, FMath::RandRange(-500.0f, 500.0f), 0.0f)
			);
	}
	
	if (patternElapsedTime >= 6.0f)
	{
		SetPattern(TEXT("Rapid"));
	}
	
}

void ABossEnemy::RapidPattern(float DeltaTime)
{
	
	SetActorLocation(patternStartLocation + 
		FVector(0.0f, FMath::Sin(patternElapsedTime * PI * 0.3f) * 600, 0.0f));
	
	
	if (FMath::Modulo(patternElapsedTime, 1.5f) < 0.6f)
	{
		basicFireElapsedTime += DeltaTime;
		if (basicFireElapsedTime >= fireInterval * 0.1f)
		{
			basicFireElapsedTime -= fireInterval * 0.1f;
			Fire();
		}
	}
	else
	{
		basicFireElapsedTime = 0.0f;
	}
	
	if (patternElapsedTime >= 10.0f)
	{
		SetPattern(TEXT("Basic"));
	}
}
