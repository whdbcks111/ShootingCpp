// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingCpp/Public/ShootingPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Bullet.h"
#include "Components/SceneComponent.h"
#include "EnemySpawner.h"
#include "GameHudWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

// Sets default values
AShootingPlayer::AShootingPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	// box comp generate
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	boxComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	RootComponent = boxComp;
	
	// set collision preset to block all
	boxComp->SetCollisionProfileName(TEXT("BlockAll"));
	
	// body mesh 
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	bodyMeshComp->SetupAttachment(boxComp);
	
	// set static mesh to cube (load cube)
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded() /*tempMesh.Object != nullptr*/)
	{
		bodyMeshComp->SetStaticMesh(tempMesh.Object);
	}
	
	FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
	FirePosition->SetupAttachment(boxComp);
	FirePosition->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
}

// Called when the game starts or when spawned
void AShootingPlayer::BeginPlay()
{ 
	Super::BeginPlay();
	
	CurrentHealth = MaxHealth;
	UpdatePlayerHpHud();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AShootingPlayer::OnHit);

	AEnemySpawner* enemySpawner = Cast<AEnemySpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass()));
	if (enemySpawner != nullptr)
	{
		enemySpawner->SetPlayer(this);
	}
}

// Called every frame
void AShootingPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 계속 오른쪽으로 이동시키자.
	// 1. 방향이 필요
	// 2. 이동 시키고 싶다.
	// P = P0 + vt
	FVector P0 = GetActorLocation();
	FVector vt = moveDirection * moveSpeed * DeltaTime;
	FVector P = P0 + vt;
	
	// 화면 안으로 clamp
	P.Y = FMath::Clamp(P.Y, this->MinY, this->MaxY);
	P.Z = FMath::Clamp(P.Z, this->MinZ, this->MaxZ);
	
	SetActorLocation(P, true);
	
	moveDirection = FVector::ZeroVector;
}

// Called to bind functionality to input
void AShootingPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// 시용자 입력 매핑 애셋 등록
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(InputContext, 0);
		}
	}
	
	// 처리 함수와 입력 액션 바인딩
	auto playerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (playerInput)
	{
		// 둘다 Triggered, 발사는 쿨타임 처리
		playerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShootingPlayer::MovePlayer);
		playerInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &AShootingPlayer::Fire);
	}
}

void AShootingPlayer::MovePlayer(const FInputActionValue& value)
{
	auto v = value.Get<FVector>();
	
	moveDirection = FVector(0, v.X, v.Y);
	moveDirection.Normalize();
}

void AShootingPlayer::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || CurrentHealth <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	UpdatePlayerHpHud();

	if (CurrentHealth <= 0.0f)
	{
		if (deathSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, deathSound, GetActorLocation(), 0.2f);
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
		OnGameOver();
		Destroy();
	}
	else
	{
		if (hitSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, hitSound, GetActorLocation(), 0.2f);
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

void AShootingPlayer::SetGameHud(UGameHudWidget* newGameHud)
{
	gameHud = newGameHud;
	UpdatePlayerHpHud();
}

void AShootingPlayer::UpdatePlayerHpHud()
{
	if (gameHud != nullptr && MaxHealth > 0.0f)
	{
		gameHud->UpdatePlayerHp(CurrentHealth / MaxHealth);
	}
}

void AShootingPlayer::OnGameOver()
{
	if (gameHud != nullptr)
	{
		gameHud->UpdateGameOverred(true);
	}
}

void AShootingPlayer::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABullet* bullet = Cast<ABullet>(OtherActor);
	if (bullet != nullptr && bullet->isEnemyBullet)
	{
		bullet->Destroy();
		ApplyDamage(bullet->Damage);
		return;
	}
}

void AShootingPlayer::Fire()
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
	
	// 현재 시간 - 마지막 발사 시간 < 쿨타임일떄 -> 발사 중지
	if ( world->GetTimeSeconds() - lastFireTime < fireCooldown )
	{
		return;
	}
	lastFireTime = world->GetTimeSeconds();
	
	FVector spawnLoc = FirePosition->GetComponentLocation();
	FRotator spawnRot = FirePosition->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABullet* bullet = world->SpawnActor<ABullet>(BulletFactory, spawnLoc, spawnRot, SpawnParams);
	if (bullet != nullptr)
	{
		bullet->isEnemyBullet = false;
		bullet->direction = FVector::UpVector;
	}
	
	if (fireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, fireSound, GetActorLocation(), 0.1f);
	}
}
