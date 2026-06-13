// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShootingPlayer.generated.h"


// 사용자의 입력에 따라 상하좌우 이동하게 하고 싶다.
// 필요속성: 이동속도

UCLASS()
class SHOOTINGCPP_API AShootingPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShootingPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class UBoxComponent* boxComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class UStaticMeshComponent* bodyMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	class USceneComponent* FirePosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
	TSubclassOf<class ABullet> BulletFactory;	
	
public:
	// 효과음들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* fireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* hitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* deathSound;
	
	// VFX
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* deathEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* hitEffect;
	
	// 필요속성: 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float moveSpeed = 500.0f;
	
	// 필요속성: 공격 쿨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float fireCooldown = 0.25f;
	
	float lastFireTime = 0.0f;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth = 3.0f;

	// 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float CurrentHealth = 3.0f;
	
	// 필요속성: 이동 방향
	FVector moveDirection = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ApplyDamage(float DamageAmount);

	void SetGameHud(class UGameHudWidget* newGameHud);
	
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

public:
	// 사용자 입력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* InputContext;

	void MovePlayer(const struct FInputActionValue& value);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputAction* FireAction;
	
	void Fire();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Move Limit")
	float MinY = -500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Move Limit")
	float MaxY = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Move Limit")
	float MinZ = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Screen Move Limit")
	float MaxZ = 800.0f;

private:
	void UpdatePlayerHpHud();
	void OnGameOver();

	UPROPERTY()
	class UGameHudWidget* gameHud = nullptr;
};
