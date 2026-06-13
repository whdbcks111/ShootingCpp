// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class SHOOTINGCPP_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	FVector direction = FVector::UpVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shoot Component")
	bool isEnemyBullet = false;
	
public:
	// 필요속성: 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float moveSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Damage = 1.0f;
};
