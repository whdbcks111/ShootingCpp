// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.generated.h"

class AEnemy;

UCLASS()
class SHOOTINGCPP_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class UBoxComponent* boxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class UStaticMeshComponent* bodyMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class USceneComponent* FirePosition;
	
public:
	// 효과음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* hitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* deathSound;
	
	//VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* deathEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* hitEffect;
	
	// 필요속성: 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float moveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ContactDamage = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHealth = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	bool isBoss = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	class AShootingPlayer* targetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	TSubclassOf<class ABullet> BulletFactory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	float fireInterval = 2.0f;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void InitializeEnemy(float InMoveSpeed, float InMaxHealth, bool bInIsBoss, AShootingPlayer* target);

	UFUNCTION(BlueprintCallable, Category = "Stat")
	virtual void ApplyHitDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void Fire();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireTo(FVector direction);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	float fireElapsedTime = 0.0f;
	
};
