// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Bullet.h"
#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "ShootingPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// box comp generate
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	boxComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	RootComponent = boxComp;
	
	// set collision preset to block all
	boxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	
	// body mesh 
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	bodyMeshComp->SetupAttachment(boxComp);
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(boxComp);
	FirePosition->SetRelativeLocation(FVector(0.0f, 0.0f, -120.0f));
	
	// set static mesh to cube (load cube)
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded() /*tempMesh.Object != nullptr*/)
	{
		bodyMeshComp->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);

	fireElapsedTime = fireInterval > 0.0f ? FMath::RandRange(0.0f, fireInterval) : 0.0f;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector P0 = GetActorLocation();
	FVector moveDir = FVector::DownVector;
	if (IsValid(targetPlayer))
	{
		moveDir = targetPlayer->GetActorLocation() - P0;
		moveDir.X = 0.0f;
		moveDir.Normalize();
	}
	
	FVector vt = moveDir * moveSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P, false);
	
	float angle = FMath::Atan2(moveDir.Y, -moveDir.Z);
	float degree = FMath::RadiansToDegrees(angle);
	
	boxComp->SetRelativeRotation(FRotator(0.0f, 0.0f,  -degree ));

	if (BulletFactory != nullptr && fireInterval > 0.0f)
	{
		fireElapsedTime += DeltaTime;
		if (fireElapsedTime >= fireInterval)
		{
			fireElapsedTime -= fireInterval;
			Fire();
		}
	}
}

void AEnemy::InitializeEnemy(float InMoveSpeed, float InMaxHealth, bool bInIsBoss, AShootingPlayer* target)
{
	moveSpeed = InMoveSpeed;
	MaxHealth = FMath::Max(1.0f, InMaxHealth);
	CurrentHealth = MaxHealth;
	isBoss = bInIsBoss;
	targetPlayer = target;

}

void AEnemy::ApplyHitDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		if (hitSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, hitSound, GetActorLocation(), 0.2f);
		}
		
		if (deathEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				deathEffect,
				GetActorLocation(),
				GetActorRotation()
			);
		}
		
		AEnemySpawner* Spawner = Cast<AEnemySpawner>(GetOwner());
		if (Spawner != nullptr)
		{
			Spawner->HandleEnemyDied(this);
		}
		
		Destroy();
	}
	else
	{
		if (deathSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, deathSound, GetActorLocation(), 0.2f);
		}
		
		if (hitEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				hitEffect,
				GetActorLocation(),
				GetActorRotation()
			);
		}
	}
}

void AEnemy::Fire()
{
	const FVector spawnLoc = FirePosition->GetComponentLocation();

	FVector fireDirection = FVector::DownVector;
	if (IsValid(targetPlayer))
	{
		fireDirection = targetPlayer->GetActorLocation() - spawnLoc;
		fireDirection.X = 0.0f;
		fireDirection.Normalize();
	}
	
	FireTo(fireDirection);
}

void AEnemy::FireTo(FVector direction)
{
	if (BulletFactory == nullptr)
	{
		return;
	}

	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	const FVector spawnLoc = FirePosition->GetComponentLocation();
	const FRotator spawnRot = direction.Rotation() + FRotator(90.0f, 0.0f, 0.0f);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABullet* bullet = world->SpawnActor<ABullet>(BulletFactory, spawnLoc, spawnRot, spawnParams);
	if (bullet != nullptr)
	{
		bullet->isEnemyBullet = true;
		bullet->direction = direction;
	}
	
	if (fireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, 
			fireSound, GetActorLocation(), 0.2f);
	}
}

void AEnemy::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet* Bullet = Cast<ABullet>(OtherActor);
	if (Bullet != nullptr && !Bullet->isEnemyBullet)
	{
		Bullet->Destroy();
		ApplyHitDamage(Bullet->Damage);
		return;
	}

	AShootingPlayer* Player = Cast<AShootingPlayer>(OtherActor);
	if (Player != nullptr)
	{
		Player->ApplyDamage(ContactDamage);
		Destroy();
		return;
	}
}
