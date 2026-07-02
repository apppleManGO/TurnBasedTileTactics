// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 총기 메시 생성
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	SetRootComponent(GunMesh);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Gun Owner: %s"), *GetOwner()->GetName());
	}
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::SpawnFireEffect()
{
	if (GunMesh)
	{
		// "Muzzle" 소켓 위치 가져오기
		FVector MuzzleLocation = GunMesh->GetSocketLocation(TEXT("Muzzle"));
		FRotator MuzzleRotation = GunMesh->GetSocketRotation(TEXT("Muzzle"));

		// FireEffect 파티클을 해당 위치에서 생성
		if (FireEffect)
		{
			// 파티클이 1초 동안만 실행되도록 설정
			UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, MuzzleLocation, MuzzleRotation, true);
            
			if (ParticleComp)
			{
				// 파티클 지속 시간 1초로 설정
				ParticleComp->SetFloatParameter(TEXT("Lifetime"), 1.0f);

				// 파티클이 자동으로 사라지도록 설정
				ParticleComp->bAutoDestroy = true;
			}
		}
	}
}