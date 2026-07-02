// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "ZombieCharacter.h"
#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "zombie_Anim.h"

AZombieAIController::AZombieAIController()
{

}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AZombieAIController::BeginPlay() - GetWorld() is nullptr"));
		return;
	}

	GameMode = Cast<AMyGameModeBase>(World->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMyGameModeBase을 찾을 수 없습니다!"));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (PlayerController)
	{
		PlayerPawn = Cast<AMyPlayer>(PlayerController->GetPawn());
	}

	
	FTimerHandle TimerHandle_ZombiePawn;
	GetWorldTimerManager().SetTimer(TimerHandle_ZombiePawn, [this]() {
		if (GetPawn())
		{
			ZombiePawn = Cast<AZombieCharacter>(GetPawn());
			if (!ZombiePawn)
			{
				UE_LOG(LogTemp, Error, TEXT("AZombieAIController::BeginPlay() - ZombiePawn is nullptr"));
				PrimaryActorTick.bCanEverTick = false;
				return;
			}

			ZombiePawn->InitializeHealth(100);
			TargetLocation = ZombiePawn->GetActorLocation();

			//UE_LOG(LogTemp, Warning, TEXT("ZombiePawn 초기화 완료!")); // 초기화 확인 로그
		}
	}, 0.1f, false); // 0.1초 후에 실행
	
	
	//UE_LOG(LogTemp, Warning, TEXT("AZombieAIController::BeginPlay() - Initialization started!")); // 시작 로그
}

void AZombieAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ZombiePawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("!ZombiePawn"));
		PrimaryActorTick.bCanEverTick = false; // Tick() 함수 호출 중지
		return;
	}

	if (ZombiePawn->waitingSp)
	{
		if(ZombiePawn->Health>0)
		{
			FVector NewLocation = FMath::VInterpTo(ZombiePawn->GetActorLocation(), TargetLocation, DeltaTime, 5.0f);
			ZombiePawn->SetActorLocation(NewLocation);
		}
		
	}
	
}


void AZombieAIController::MoveTowardsPlayer()
{
	if (!ZombiePawn || !PlayerPawn)
	{
		// 초기화가 아직 안 끝난 경우 등: 이번 턴은 행동 없이 완료 처리
		if (GameMode)
		{
			GameMode->NotifyZombieActed();
		}
		return;
	}

	//이동애니메이션 실행
	ZombiePawn->ZombieAnim->Anim_move=true;
	ZombiePawn->waitingSp = true;

	// 플레이어 주변에 있으면 바로 공격하고 이번 턴 행동 종료
	if(IsPlayerNearby())
	{
		AttackPlayer();
		return;
	}
	// 플레이어 위치와 좀비 위치 가져오기
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FVector ZombieLoc = ZombiePawn->GetActorLocation();

	// X, Y 좌표 차이 계산
	float DeltaX = PlayerLoc.X - ZombieLoc.X;
	float DeltaY = PlayerLoc.Y - ZombieLoc.Y;

	// 이동 방향 결정 (한 번에 한 방향만 이동)
	FVector NewLocation = ZombieLoc;

	bool bMoveX = FMath::Abs(DeltaX) > 25.0f;
	bool bMoveY = FMath::Abs(DeltaY) > 25.0f;

	//x,y축 모두 이동이 필요할때
	if (bMoveX && bMoveY) 
	{
		// X축 또는 Y축 중 랜덤 선택
		if (FMath::RandBool()) 
		{
			NewLocation.X += (DeltaX > 0) ? 100.0f : -100.0f;
			// 이동 가능 여부 확인
			if (!CheckCanMove(NewLocation))
			{
				TargetLocation = NewLocation;
				// **좀비가 이동 방향을 바라보도록 회전 추가**
				FRotator LookAtRotation = (TargetLocation - ZombieLoc).Rotation();
				ZombiePawn->SetActorRotation(LookAtRotation);
				//1초뒤에 IsPlayerNearby 함수를 실행
				GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
			}
			else
			{
				// 이동 불가능한 경우, 다른 위치로 이동
				FindAlternativeLocation(true);
			}
		} 
		else 
		{
			NewLocation.Y += (DeltaY > 0) ? 100.0f : -100.0f;
			// 이동 가능 여부 확인
			if (!CheckCanMove(NewLocation))
			{
				TargetLocation = NewLocation;
				// **좀비가 이동 방향을 바라보도록 회전 추가**
				FRotator LookAtRotation = (TargetLocation - ZombieLoc).Rotation();
				ZombiePawn->SetActorRotation(LookAtRotation);
				//1초뒤에 IsPlayerNearby 함수를 실행
				GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
		
			}
			else
			{
				// 이동 불가능한 경우, 다른 위치로 이동
				FindAlternativeLocation(false);
			}
		}
	}
	else if (bMoveX) //x축만 이동이 필요할때
	{
		NewLocation.X += (DeltaX > 0) ? 100.0f : -100.0f;
		// 이동 가능 여부 확인
        	if (!CheckCanMove(NewLocation))
        	{
        		TargetLocation = NewLocation;
        		// **좀비가 이동 방향을 바라보도록 회전 추가**
                        FRotator LookAtRotation = (TargetLocation - ZombieLoc).Rotation();
                        ZombiePawn->SetActorRotation(LookAtRotation);
        		//1초뒤에 IsPlayerNearby 함수를 실행
        		GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
		
        	}
        	else
        	{
        		// 이동 불가능한 경우, 다른 위치로 이동
        		FindAlternativeLocation(true);
        	}
	}
	else if (bMoveY) //y축만 이동이 필요할때
	{
		NewLocation.Y += (DeltaY > 0) ? 100.0f : -100.0f;
		// 이동 가능 여부 확인
		if (!CheckCanMove(NewLocation))
		{
			TargetLocation = NewLocation;
			// **좀비가 이동 방향을 바라보도록 회전 추가**
			FRotator LookAtRotation = (TargetLocation - ZombieLoc).Rotation();
			ZombiePawn->SetActorRotation(LookAtRotation);
			//1초뒤에 IsPlayerNearby 함수를 실행
			GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
		
		}
		else
		{
			// 이동 불가능한 경우, 다른 위치로 이동
			FindAlternativeLocation(false);
		}
	}

	
}

bool AZombieAIController::CheckCanMove(FVector Location)
{
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	// 확인할 오브젝트 타입 설정 (Pawn, WorldStatic, WorldDynamic 등 원하는 타입 추가)
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));     // Pawn (좀비, 플레이어 등)
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic)); // Static Mesh (지형, 건물 등)
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // Dynamic Mesh (움직이는 오브젝트)
	// 필요에 따라 다른 ObjectType 추가 가능

	TArray<AActor*> IgnoreActors;

	bool bFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Location,
		25.0f, // 반경 (원하는 값으로 조절 가능)
		ObjectTypes,
		nullptr, // 클래스 필터 없이 모든 ObjectType에 대해 충돌 검사
		IgnoreActors,
		OutActors
	);
	return bFound;
}
void AZombieAIController::FindAlternativeLocation(bool xy)
{
	FVector PlayerLoc = PlayerPawn->GetActorLocation();
	FVector ZombieLoc = ZombiePawn->GetActorLocation();
	if(xy)//true면 x축으로 이동못하니까 y축으로 이동
	{
		// 1. Y축 이동 시도
		ZombieLoc.Y += FMath::RandBool() ? 100.0f : -100.0f;

		if (!CheckCanMove(ZombieLoc))
		{
			TargetLocation = ZombieLoc;
			// **좀비가 이동 방향을 바라보도록 회전 추가**
			FRotator LookAtRotation = (TargetLocation - ZombieLoc).Rotation();
			ZombiePawn->SetActorRotation(LookAtRotation);
			//1초뒤에 IsPlayerNearby 함수를 실행
			GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
		
			return; // Y축 이동 성공 시 함수 종료
		}
	}
	else//false면 y축으로 이동못하니까 x축으로 이동
	{
		// 2. X축 이동 시도 (Y축 이동 실패 시)
		ZombieLoc = ZombiePawn->GetActorLocation(); // 원래 위치로 초기화
		ZombieLoc.X += FMath::RandBool() ? 100.0f : -100.0f;

		if (!CheckCanMove(ZombieLoc))
		{
			TargetLocation = ZombieLoc;
			//1초뒤에 IsPlayerNearby 함수를 실행
			GetWorldTimerManager().SetTimer(CheckcanAttackTimerHandle, this, &AZombieAIController::CheckAndAttackPlayer, 1.0f, false);
		
			return; // X축 이동 성공 시 함수 종료
		}
	}

	

	// 3. 둘 다 실패 시 (선택적)
	// (예: 다른 행동을 하거나, 현재 위치를 유지하거나, 더 작은 폭으로 다시 시도 등)
	// 예시: 현재 위치 유지
	TargetLocation = ZombiePawn->GetActorLocation();

	// 이동할 곳이 전혀 없는 경우에도 이번 턴 행동은 끝난 것으로 처리해야
	// 다른 좀비들이 모두 행동을 마쳤을 때 플레이어 턴으로 정상 복귀한다.
	if (GameMode)
	{
		GameMode->NotifyZombieActed();
	}
}
bool AZombieAIController::IsPlayerNearby()
{
	// 순수 확인 함수: 플레이어가 공격 범위 내에 있는지만 판단하고, 부작용은 없다.
	if (!ZombiePawn || !PlayerPawn) return false;

	FVector ZombieLoc = ZombiePawn->GetActorLocation();
	FVector PlayerLoc = PlayerPawn->GetActorLocation();

	// 공격 범위 설정 (원하는 값으로 조절)
	float AttackRange = 125.0f;

	return (PlayerLoc - ZombieLoc).Size() <= AttackRange;
}
void AZombieAIController::CheckAndAttackPlayer()
{
	// 이동 후 플레이어가 범위 내에 있으면 공격, 아니면 이번 턴 행동 종료 처리
	if (IsPlayerNearby())
	{
		AttackPlayer();
	}
	else if (GameMode)
	{
		GameMode->NotifyZombieActed();
	}
}
void AZombieAIController::AttackPlayer()
{
	if (!ZombiePawn || !PlayerPawn) return;
	if(ZombiePawn->Health>0)
	{
		FVector ZombieLoc = ZombiePawn->GetActorLocation();
		FVector PlayerLoc = PlayerPawn->GetActorLocation();
		// Z값(높이)을 같게 맞춰서 수평 회전만 하도록
		PlayerLoc.Z = ZombieLoc.Z;
		FRotator LookAtRotation = (PlayerLoc - ZombieLoc).Rotation();
		ZombiePawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		ZombieAnim = Cast<Uzombie_Anim>(ZombiePawn->GetMesh()->GetAnimInstance());
		ZombieAnim->Anim_attack=true;
		PlayerPawn->Playerhit(20,ZombiePawn->GetActorLocation());
	}
	if (GameMode)
	{
		GameMode->NotifyZombieActed();
	}
}
