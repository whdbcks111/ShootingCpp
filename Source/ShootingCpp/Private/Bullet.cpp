// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Components/BoxComponent.h"


// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// box comp generate
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	boxComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	RootComponent = boxComp;
	
	// 총알은 물리적으로 막히지 않고 겹침 판정만 사용한다.
	boxComp->SetCollisionProfileName(TEXT("OverlapAll"));
	
	// body mesh 
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	bodyMeshComp->SetupAttachment(boxComp);
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// set static mesh to cube (load cube)
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded() /*tempMesh.Object != nullptr*/)
	{
		bodyMeshComp->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector P0 = GetActorLocation();
	FVector moveDir = direction;
	moveDir.Normalize();
	FVector vt = moveDir * moveSpeed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P, false);
}

