// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "EngineUtils.h"
#include "MyPlayer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ZombieAIController.h"
#include "ZombieCharacter.h"
#include <ctime> // 시간 관련 헤더 추가

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AMyGameModeBase::AMyGameModeBase()
{
	// 게임의 초기 상태 설정
	CurrentTurn = EGameTurn::PlayerTurn;
}
void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	//난이도를 선택하면 실행되도록 수정하기
	//SpawnMonsters(10); // 게임 시작 시 몬스터 10마리 스폰
	//StartPlayerTurn();
}

void AMyGameModeBase::StartPlayerTurn()
{
	// 플레이어 턴 시작
	CurrentTurn = EGameTurn::PlayerTurn;

	// 플레이어가 행동할 수 있도록 처리 (예: 플레이어의 입력을 받기 시작)
	AMyPlayer* Player = Cast<AMyPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		Player->EnablePlayerInput();  // 플레이어 입력 활성화
		Player->ShowMovableTiles(); //플레이어 이동 타일 활설화
	}
}

void AMyGameModeBase::EndPlayerTurn()
{
	// 플레이어 턴 종료
	CurrentTurn = EGameTurn::MonsterTurn;

	// 몬스터 턴 시작 (몬스터 행동 시작)
	// 1초 후에 턴으로 돌아감
	GetWorld()->GetTimerManager().SetTimer(PlayerTurnTimerHandle, this, &AMyGameModeBase::StartMonsterTurn, 1.0f, false);
	
}

void AMyGameModeBase::StartMonsterTurn()
{
	// 몬스터 턴 시작
	AMyPlayer* Player = Cast<AMyPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (Player)
	{
		Player->DisablePlayerInput();  // 몬스터 턴에는 플레이어의 입력 비활성화
	}

	// 몬스터가 이동하도록 처리
	for (TActorIterator<AZombieCharacter> It(GetWorld()); It; ++It)
	{
		AZombieCharacter* Zombie = *It;
		if (Zombie)
		{
			AZombieAIController* AIController = Cast<AZombieAIController>(Zombie->GetController());
			if (AIController)
			{
				// 각 좀비마다 다른 딜레이를 가진 타이머 설정
				float Delay = FMath::RandRange(0.0f, 1.0f); // 0초에서 1초 사이의 랜덤 딜레이

				// 람다 함수 내에서 MoveTowardsPlayer() 호출
				GetWorldTimerManager().SetTimer(Zombie->MoveTimerHandle, [AIController]() {
					AIController->MoveTowardsPlayer(); // MoveTowardsPlayer() 호출
				}, Delay, false);
				
			}
		}
	}

	// 몬스터 턴이 끝났으면 1초 후에 플레이어 턴으로 돌아감
	//GetWorld()->GetTimerManager().SetTimer(MonsterTurnTimerHandle, this, &AMyGameModeBase::EndMonsterTurn, 5.0f, false);
}

void AMyGameModeBase::EndMonsterTurn()
{
	// 몬스터 턴 종료 후, 다시 플레이어 턴으로 넘어가게 처리
	// 1초 후에 턴으로 돌아감
	GetWorld()->GetTimerManager().SetTimer(PlayerTurnTimerHandle, this, &AMyGameModeBase::StartPlayerTurn, 1.0f, false);
	
}
void AMyGameModeBase::SpawnMonsters(int32 MonsterCount)
{
	// 랜덤 시드 설정 (게임 시작 시 한 번만 호출)
	FMath::RandInit(std::time(0));

	UWorld* World = GetWorld();
	if (!World) return;

	int32 CountPerZone = MonsterCount / 4; // 4개 구역에 몬스터를 균등하게 분배

	// 1번 구역 (오른쪽)
	for (int32 i = 0; i < CountPerZone; i++)
	{
		FVector SpawnLocation;
		do
		{
			SpawnLocation = GetRandomSpawnLocation(Sp_X, Sp_X + 100, -Sp_Y, Sp_Y, true);
		} while (SpawnedLocations.Contains(SpawnLocation));

		SpawnMonsterAtLocation(World, SpawnLocation);
	}

	// 2번 구역 (위쪽)
	for (int32 i = 0; i < CountPerZone; i++)
	{
		FVector SpawnLocation;
		do
		{
			SpawnLocation = GetRandomSpawnLocation(-Sp_X, Sp_X, Sp_Y, Sp_Y + 100, false);
		} while (SpawnedLocations.Contains(SpawnLocation));

		SpawnMonsterAtLocation(World, SpawnLocation);
	}

	// 3번 구역 (왼쪽)
	for (int32 i = 0; i < CountPerZone; i++)
	{
		FVector SpawnLocation;
		do
		{
			SpawnLocation = GetRandomSpawnLocation(-Sp_X - 100, -Sp_X, -Sp_Y, Sp_Y, true);
		} while (SpawnedLocations.Contains(SpawnLocation));

		SpawnMonsterAtLocation(World, SpawnLocation);
	}

	// 4번 구역 (아래쪽)
	for (int32 i = 0; i < CountPerZone; i++)
	{
		FVector SpawnLocation;
		do
		{
			SpawnLocation = GetRandomSpawnLocation(-Sp_X, Sp_X, -Sp_Y - 100, -Sp_Y, false);
		} while (SpawnedLocations.Contains(SpawnLocation));

		SpawnMonsterAtLocation(World, SpawnLocation);
	}
}
FVector AMyGameModeBase::GetRandomSpawnLocation(float MinX, float MaxX, float MinY, float MaxY, bool bFixedX)
{
    TArray<FVector> PossibleValues;

    for (float X = MinX; X <= MaxX; X += 100.0f)
    {
        for (float Y = MinY; Y <= MaxY; Y += 100.0f)
        {
            FVector Location(X, Y, 88.0f);
            
                if (!SpawnedLocations.Contains(Location))
                {
                    PossibleValues.Add(Location);
                }
           
        }
    }

    if (PossibleValues.Num() == 0)
    {
        return FVector(bFixedX ? 850.0f : 50.0f, bFixedX ? 50.0f : 850.0f, 88.0f); // 각 구역의 기본 위치 반환
    }

    int32 RandomIndex = FMath::RandRange(0, PossibleValues.Num() - 1);
    return PossibleValues[RandomIndex];
}
void AMyGameModeBase::SpawnMonsterAtLocation(UWorld* World, FVector SpawnLocation)
{
	FRotator SpawnRotation = FRotator::ZeroRotator;
	if (ZombieCharacterClass)
	{
		AZombieCharacter* SpawnedZombie = World->SpawnActor<AZombieCharacter>(ZombieCharacterClass, SpawnLocation, SpawnRotation);
		if (SpawnedZombie)
		{
			SpawnedZombie->SpawnDefaultController();
			SpawnedLocations.Add(SpawnLocation);
			AliveZombies++;
		}
	}
}
void AMyGameModeBase::OnZombieDied()
{
	AliveZombies--;

	if (AliveZombies <= 0)
	{
		// 2초 후에 턴으로 돌아감
		GetWorld()->GetTimerManager().SetTimer(EndGameTimerHandle, this, &AMyGameModeBase::HandleGameOver, 2.0f, false);
	}
}
void AMyGameModeBase::HandleGameOver()
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		// 게임을 멈추고 마우스 커서를 표시
		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;

		// UI 입력 모드로 변경
		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}

	// 게임 오버 UI 표시
	if (GameOverWidgetClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}
}