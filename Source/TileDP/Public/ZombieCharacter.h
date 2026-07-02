// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

UCLASS()
class TILEDP_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieCharacter();
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 체력 변수
	float Health;

	bool waitingSp=false;
	FTimerHandle MoveTimerHandle;
	// 체력 초기화
	void InitializeHealth(float InitialHealth);

	// 데미지 처리 함수
	void HaveDamage(float DamageAmount);
	
	// 죽을 때 호출되는 함수
	void Die();
	UFUNCTION(BlueprintCallable)
	void DestroyZombie();
	bool IsValidTile(FIntPoint Tile);
	class AMyGameModeBase* GameMode;
	class Uzombie_Anim* ZombieAnim;
	class AMyPlayer* PlayerPawn;
	FTimerHandle DestroyTimerHandle;

};
