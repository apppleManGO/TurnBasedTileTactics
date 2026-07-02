// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ZombieCharacter.h"
#include "MyPlayer.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class TILEDP_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
public:
	// 기본 생성자
	AZombieAIController();

	// BeginPlay 함수 선언
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void MoveTowardsPlayer();
	bool CheckCanMove(FVector Location);
	void FindAlternativeLocation(bool xy);
	bool IsPlayerNearby();
	void CheckAndAttackPlayer();
	void AttackPlayer();
	class AMyGameModeBase* GameMode;
	// 좀비 캐릭터를 저장하는 변수
	class AZombieCharacter* ZombiePawn;
	// 플레이어 캐릭터를 저장하는 변수
	class AMyPlayer* PlayerPawn;
	FVector TargetLocation;
	FTimerHandle CheckcanAttackTimerHandle;
	TArray<FIntPoint> Path;  // 플레이어를 향한 경로
	int32 CurrentPathIndex;   // 현재 경로에서 목표 지점
	Uzombie_Anim* ZombieAnim;
};
