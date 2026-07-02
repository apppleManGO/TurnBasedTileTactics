// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameModeBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "MyPlayer.generated.h"
class AGun;  // 총기 클래스 전방 선언

UCLASS()
class TILEDP_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EnablePlayerInput();
	void DisablePlayerInput();
	void MoveForward();
	void MoveBack();
	void MoveRight();
	void MoveLeft();
	bool IsAnythingAtLocation(FVector Location);
	void ShowMovableTiles();
	void DontShowMovableTiles();
	void ShowAttackTiles();
	bool AreZombiesAtLocations(FVector Location1, FVector Location2);
	void Attack();
	bool AttackZombieAtLocation(FVector Location);
	void Playerhit(float hit,FVector Location);
	void Die();
	UFUNCTION(BlueprintCallable)
	void HitAinm();
	void SpawnFireEffect();
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UCameraComponent* Camera;
	
	USceneComponent* CameraRoot;
	
	bool bIsSelectingAttackDirection = false;
	
	TArray<UDecalComponent*> Decals;  // 생성된 데칼들을 저장할 배열
	class AMyGameModeBase* GameMode;
	class UMyPlayer_anim* playAnim;
	
	// 총기 클래스 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AGun> GunClass;

	
	//플레이어가 공격할 방향
	FVector AttackDirection;
	FVector TargetLocation;
	float MoveSpeed;
	float PlayerHealth;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float Maxmove = 350;
	FTimerHandle ShowAttackTilesTimerHandle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator StartCamera;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FRotator TargetCamera;
	// 현재 장착한 총기
	AGun* Gun;
};
