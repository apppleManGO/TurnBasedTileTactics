// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class TILEDP_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SpawnFireEffect();

	// 총기 메시
	// 총기 메시 (스태틱 메시)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* GunMesh;

	// 총알이 발사되는 위치
	UPROPERTY(EditAnywhere, Category = "Combat")
	FVector MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);
	//총 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* FireEffect;
};
