// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombieCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"


/**
 * 
 */
// 게임의 턴 상태를 정의하는 Enum
UENUM(BlueprintType)
enum class EGameTurn : uint8
{
	PlayerTurn UMETA(DisplayName = "Player Turn"),
	MonsterTurn UMETA(DisplayName = "Monster Turn")
};

UCLASS()
class TILEDP_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameModeBase();
	virtual void BeginPlay() override;

	// 턴 상태 관리 함수들
	UFUNCTION(BlueprintCallable)
	void StartPlayerTurn();  // 플레이어 턴 시작
	void EndPlayerTurn();    // 플레이어 턴 종료
	void StartMonsterTurn(); // 몬스터 턴 시작
	void EndMonsterTurn();   // 몬스터 턴 종료
	// 좀비 한 마리가 이번 턴의 행동(공격/이동/이동 실패 포함)을 마쳤을 때 호출.
	// 모든 좀비가 행동을 마치면 그때 한 번만 EndMonsterTurn()을 실행한다.
	void NotifyZombieActed();
	UFUNCTION(BlueprintCallable)
	void SpawnMonsters(int32 MonsterCount);//몬스터 스폰하기
	bool GetRandomSpawnLocation(float MinX, float MaxX, float MinY, float MaxY, FVector& OutLocation);
	void SpawnMonsterAtLocation(UWorld* World, FVector SpawnLocation);
	void OnZombieDied();
	void HandleGameOver();
	TArray<FVector> SpawnedLocations; //스폰 위치
	// 좀비를 저장할 클래스 변수 선언
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AZombieCharacter> ZombieCharacterClass;
	// 현재 턴 상태
	EGameTurn GetCurrentTurn() const { return CurrentTurn; }
	//스폰할위치범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite ,Category = "Spawning")	
	float Sp_X=350.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite ,Category = "Spawning")	
	float Sp_Y=350.f;
	
	int32 AliveZombies = 0;//현재 생존중인 몬스터
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
private:
	EGameTurn CurrentTurn;  // 현재 턴 상태
	FTimerHandle MonsterTurnTimerHandle;  // 몬스터 턴 타이머 핸들
	FTimerHandle PlayerTurnTimerHandle;  // 플레이어 턴 타이머 핸들
	FTimerHandle EndGameTimerHandle;  // 플레이어 턴 타이머 핸들

	int32 ZombiesToActThisTurn = 0;  // 이번 몬스터 턴에 행동해야 하는 좀비 수
	int32 ZombiesActedThisTurn = 0;  // 이번 몬스터 턴에 행동을 마친 좀비 수
};
