// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"
#include "MyGameModeBase.h"
#include "ZombieCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Gun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "MyPlayer_anim.h"
// Sets default values
AMyPlayer::AMyPlayer()
{
	// Tick 활성화
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 루트: RootComponent에 종속하지 않고 월드에 고정
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetWorldLocation(FVector(0.0f, 0.0f, 500.0f)); // 위치는 적절히 설정 (필요 시 조정)

	// SpringArm을 카메라 루트에 부착 (월드에 고정)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CameraRoot);
	SpringArm->TargetArmLength = 1200.0f;
	SpringArm->SetRelativeRotation(FRotator(65.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = false; // 카메라가 플레이어의 Yaw 회전을 따라가지 않음

	// 카메라 설정
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// 캐릭터가 자동으로 이동 방향으로 회전하지 않도록 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	
	TargetLocation = FVector::ZeroVector; // 초기 목표 위치 설정
	MoveSpeed = 500.0f; // 이동 속도 설정 (값을 조절하여 이동 속도를 변경 가능)
	
}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();
	TargetLocation = GetActorLocation(); // 처음에는 현재 위치를 목표 위치로 설정
	PlayerHealth=100;
	GameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode());
	playAnim = Cast<UMyPlayer_anim>(GetMesh()->GetAnimInstance());
	if (GunClass)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		if (Gun)
		{
			Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_r"));
			//Gun->SetActorRelativeLocation(FVector(0, 0, 0));
			Gun->SetActorRelativeRotation(FRotator(0, 90, 90));
		}
	}
	
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// 현재 위치에서 목표 위치로 부드럽게 이동 (Lerp)
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 5.0f);
	SetActorLocation(NewLocation);
	// 카메라가 플레이어를 바라보도록 회전
	if (CameraRoot)
	{
		FVector CameraToPlayer = GetActorLocation() - CameraRoot->GetComponentLocation();
		FRotator LookAtRotation = CameraToPlayer.Rotation();
		LookAtRotation.Pitch = -65.0f;  // 위에서 바라보는 각도 유지
		LookAtRotation.Roll = 0.0f;
		CameraRoot->SetWorldRotation(LookAtRotation);
	}
}

// Called to bind functionality to input
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("MoveForward", IE_Pressed, this, &AMyPlayer::MoveForward);
	PlayerInputComponent->BindAction("MoveBack", IE_Pressed, this, &AMyPlayer::MoveBack);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &AMyPlayer::MoveRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &AMyPlayer::MoveLeft);
}

void AMyPlayer::EnablePlayerInput()
{
	// 플레이어의 컨트롤러를 가져와 입력을 활성화
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 플레이어의 입력 활성화
		PlayerController->EnableInput(PlayerController);
	}
}

void AMyPlayer::DisablePlayerInput()
{
	// 플레이어의 컨트롤러를 가져와 입력을 비활성화
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 플레이어의 입력 비활성화
		PlayerController->DisableInput(PlayerController);
	}
}


// 이동 함수 (키를 누를 때 목표 위치 변경)
void AMyPlayer::MoveForward()
{	//이동과 공격방향 분리
	if(!bIsSelectingAttackDirection)
	{
	//x축 맵 이동제한
		if (TargetLocation.X+100 <Maxmove)
		{
			FVector3d tempLocation = TargetLocation + FVector(100.0f, 0.0f, 0.0f);
			if(!IsAnythingAtLocation(tempLocation))
			{
				playAnim->Anim_move=true;
				TargetLocation += FVector(100.0f, 0.0f, 0.0f);
				FVector MoveDirection = TargetLocation - GetActorLocation();

				if (!MoveDirection.IsNearlyZero())
				{
					FRotator LookAtRotation = MoveDirection.Rotation();
					LookAtRotation.Pitch = 0.0f;
					LookAtRotation.Roll = 0.0f;
					SetActorRotation(LookAtRotation); // 플레이어만 회전!

					UE_LOG(LogTemp, Warning, TEXT("MoveForward Rotation: %s"), *LookAtRotation.ToString());
					DontShowMovableTiles();
					GetWorld()->GetTimerManager().SetTimer(ShowAttackTilesTimerHandle, this, &AMyPlayer::ShowAttackTiles, 2.0f, false);
					bIsSelectingAttackDirection=true;
				}
			}
		}
	}
	else
	{
		AttackDirection = (FVector(100.0f, 0.0f, 0.0f));
		if (!AttackDirection.IsNearlyZero())
		{
			FRotator AttackRotation = AttackDirection.Rotation();
			AttackRotation.Pitch = 0.0f;
			AttackRotation.Roll = 0.0f;
			SetActorRotation(AttackRotation); // 공격 방향으로 회전

			// 공격 함수 실행
			Attack();
		}
	}
	
}

void AMyPlayer::MoveBack()
{
	//이동과 공격방향 분리
	if(!bIsSelectingAttackDirection)
	{
		if (TargetLocation.X-100 >-Maxmove)
		{
			FVector3d tempLocation = TargetLocation + FVector(-100.0f, 0.0f, 0.0f);
			if(!IsAnythingAtLocation(tempLocation))
			{
				playAnim->Anim_move=true;
				TargetLocation += FVector(-100.0f, 0.0f, 0.0f);
				FVector MoveDirection = TargetLocation - GetActorLocation();

				if (!MoveDirection.IsNearlyZero())
				{
					FRotator LookAtRotation = MoveDirection.Rotation();
					LookAtRotation.Pitch = 0.0f;
					LookAtRotation.Roll = 0.0f;
					SetActorRotation(LookAtRotation); // 플레이어만 회전!

					UE_LOG(LogTemp, Warning, TEXT("MoveForward Rotation: %s"), *LookAtRotation.ToString());
					DontShowMovableTiles();
					GetWorld()->GetTimerManager().SetTimer(ShowAttackTilesTimerHandle, this, &AMyPlayer::ShowAttackTiles, 2.0f, false);
					bIsSelectingAttackDirection=true;
				}
			}
		}
	
	}
	else
	{
		AttackDirection = (FVector(-100.0f, 0.0f, 0.0f));
		if (!AttackDirection.IsNearlyZero())
		{
			FRotator AttackRotation = AttackDirection.Rotation();
			AttackRotation.Pitch = 0.0f;
			AttackRotation.Roll = 0.0f;
			SetActorRotation(AttackRotation); // 공격 방향으로 회전

			// 공격 함수 실행
			Attack();
		}
	}
}
void AMyPlayer::MoveRight()
	{
		//이동과 공격방향 분리
		if(!bIsSelectingAttackDirection)
		{
			if (TargetLocation.Y+100 <Maxmove)
			{
				FVector3d tempLocation = TargetLocation + FVector(0.0f, 100.0f, 0.0f);
				if(!IsAnythingAtLocation(tempLocation))
				{
					playAnim->Anim_move=true;
					TargetLocation += FVector(0.0f, 100.0f, 0.0f);
					FVector MoveDirection = TargetLocation - GetActorLocation();

					if (!MoveDirection.IsNearlyZero())
					{
						FRotator LookAtRotation = MoveDirection.Rotation();
						LookAtRotation.Pitch = 0.0f;
						LookAtRotation.Roll = 0.0f;
						SetActorRotation(LookAtRotation); // 플레이어만 회전!

						UE_LOG(LogTemp, Warning, TEXT("MoveForward Rotation: %s"), *LookAtRotation.ToString());
						DontShowMovableTiles();
						GetWorld()->GetTimerManager().SetTimer(ShowAttackTilesTimerHandle, this, &AMyPlayer::ShowAttackTiles, 2.0f, false);
						bIsSelectingAttackDirection=true;
					}
				}
			}
			
		}
		else
		{
			AttackDirection = (FVector(0.0f, 100.0f, 0.0f));
			if (!AttackDirection.IsNearlyZero())
			{
				FRotator AttackRotation = AttackDirection.Rotation();
				AttackRotation.Pitch = 0.0f;
				AttackRotation.Roll = 0.0f;
				SetActorRotation(AttackRotation); // 공격 방향으로 회전

				// 공격 함수 실행
				Attack();
			}
		}
	}
void AMyPlayer::MoveLeft()
{
	//이동과 공격방향 분리
	if(!bIsSelectingAttackDirection)
	{
		if (TargetLocation.Y-100 >-Maxmove)
		{
			FVector3d tempLocation = TargetLocation + FVector(0.0f, -100.0f, 0.0f);
			if(!IsAnythingAtLocation(tempLocation))
			{
				playAnim->Anim_move=true;
				TargetLocation += FVector(0.0f, -100.0f, 0.0f);
				FVector MoveDirection = TargetLocation - GetActorLocation();

				if (!MoveDirection.IsNearlyZero())
				{
					FRotator LookAtRotation = MoveDirection.Rotation();
					LookAtRotation.Pitch = 0.0f;
					LookAtRotation.Roll = 0.0f;
					SetActorRotation(LookAtRotation); // 플레이어만 회전!

					UE_LOG(LogTemp, Warning, TEXT("MoveForward Rotation: %s"), *LookAtRotation.ToString());
					DontShowMovableTiles();
					GetWorld()->GetTimerManager().SetTimer(ShowAttackTilesTimerHandle, this, &AMyPlayer::ShowAttackTiles, 2.0f, false);
					bIsSelectingAttackDirection=true;
				}
			}
		}
	}
	else
	{
		AttackDirection = (FVector(0.0f, -100.0f, 0.0f));
		if (!AttackDirection.IsNearlyZero())
		{
			FRotator AttackRotation = AttackDirection.Rotation();
			AttackRotation.Pitch = 0.0f;
			AttackRotation.Roll = 0.0f;
			SetActorRotation(AttackRotation); // 공격 방향으로 회전

			// 공격 함수 실행
			Attack();
		}
	}
}
bool AMyPlayer::IsAnythingAtLocation(FVector Location)
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
void AMyPlayer::ShowMovableTiles()
{
	//UE_LOG(LogTemp, Warning, TEXT("ShowMovableTiles이 실행되었습니다"));
	UMaterialInterface* DecalMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/RedTileDecal.RedTileDecal"));

	if (!DecalMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Decal Material을 찾을 수 없습니다!"));
		return;
	}

	// 이동 가능한 방향들
	TArray<FVector> Directions = {
		FVector(100, 0, 0),
		FVector(-100, 0, 0),
		FVector(0, 100, 0),
		FVector(0, -100, 0)
	};

	// 현재 위치 가져오기
	FVector PlayerLocation = GetActorLocation();

	for (FVector Direction : Directions)
	{
		FVector TileLocation = PlayerLocation + Direction;

		// 이동 범위 제한
		if (TileLocation.X >-Maxmove && TileLocation.X <Maxmove && TileLocation.Y >-Maxmove && TileLocation.Y <Maxmove&&!IsAnythingAtLocation(TileLocation))
		{
			// 데칼 스폰
			UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(60, 50, 100), TileLocation, FRotator(0, 0, 0));
			if (Decal)
			{
				Decals.Add(Decal);  // 생성된 데칼을 배열에 추가
			}
			
		}
	}
}
void AMyPlayer::DontShowMovableTiles()
{
	// 기존 데칼 모두 제거
	for (UDecalComponent* Decal : Decals)
	{
		if (Decal)
		{
			Decal->DestroyComponent();
		}
	}
	Decals.Empty();  // 배열 초기화	
}

void AMyPlayer::ShowAttackTiles()
{
	playAnim->Anim_move=false;
	UMaterialInterface* DecalMaterial2 = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Materials/GreenTileDecal.GreenTileDecal"));
	int32 checkNozimbies=0;
	if (!DecalMaterial2)
	{
		UE_LOG(LogTemp, Warning, TEXT("GREen Decal Material을 찾을 수 없습니다!"));
		return;
	}

	// 공격 가능한 방향들
	TArray<FVector> Directions = {
		FVector(100, 0, 0),
		FVector(-100, 0, 0),
		FVector(0, 100, 0),
		FVector(0, -100, 0)
	};

	// 현재 위치 가져오기
	FVector PlayerLocation = GetActorLocation();

	for (FVector Direction : Directions)
	{
		FVector TileLocation = PlayerLocation + Direction;
		FVector TileLocation2 = PlayerLocation + Direction*2;
		// 생성 범위 제한
		if (TileLocation.X >-Maxmove && TileLocation.X <Maxmove && TileLocation.Y >-Maxmove && TileLocation.Y <Maxmove)
		{
			// TileLocation과 TileLocation2에 모두 좀비가 있는지 확인
			if (AreZombiesAtLocations(TileLocation, TileLocation2))
			{
				checkNozimbies++;
				// 데칼 스폰
				UDecalComponent* Decal2 = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial2, FVector(60, 50, 100), TileLocation, FRotator(0, 0, 0));
				UDecalComponent* Decal3 = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial2, FVector(60, 50, 100), TileLocation2, FRotator(0, 0, 0));

				if (Decal2&&Decal3)
				{
					Decals.Add(Decal2);  // 생성된 데칼을 배열에 추가
					Decals.Add(Decal3);
				}
			}
			
		}
	}
	if (checkNozimbies==0)//이동후에 상하좌우에 몬스터가 없을경우 플레이어 턴 종료
	{
		playAnim->Anim_move=false;
		bIsSelectingAttackDirection=false;
		GameMode->EndPlayerTurn();
	}
}
bool AMyPlayer::AreZombiesAtLocations(FVector Location1, FVector Location2)
{
	TArray<AActor*> OutActors1, OutActors2;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn 유형만 탐색
	TArray<AActor*> IgnoreActors;

	// 첫 번째 위치에서 좀비 확인
	bool bFound1 = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Location1,
		25.0f, // 반경
		ObjectTypes,
		AZombieCharacter::StaticClass(),
		IgnoreActors,
		OutActors1
	);

	// 두 번째 위치에서 좀비 확인
	bool bFound2 = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Location2,
		25.0f, // 반경
		ObjectTypes,
		AZombieCharacter::StaticClass(),
		IgnoreActors,
		OutActors2
	);

	// 두 위치에 중 좀비가 있을 경우 true 반환
	return bFound1 || bFound2;
}
void AMyPlayer::Attack()
{
	if (Gun)
	{
		// 총기에서 파티클 이펙트를 생성하는 메서드 호출
		Gun->SpawnFireEffect();
	}
	DontShowMovableTiles(); // 공격 후 이동/공격 타일 제거
	DisablePlayerInput(); // 공격 중 입력 비활성화 (선택 사항)

	FVector PlayerLocation = GetActorLocation();
	bool bAttacked = false;
	bIsSelectingAttackDirection=false;
	// 선택된 방향으로만 공격
	FVector AttackLocation = PlayerLocation + AttackDirection;
	FVector AttackLocation2 = PlayerLocation + AttackDirection * 2;

	if (AreZombiesAtLocations(AttackLocation, AttackLocation2))
	{
		playAnim->Anim_attack=true;
		if (AttackZombieAtLocation(AttackLocation))
		{
			bAttacked = true;
		}
		if (AttackZombieAtLocation(AttackLocation2))
		{
			bAttacked = true;
		}
	}


	//float EndTurnDelay = 1.0f;
	//FTimerHandle EndTurnTimerHandle;
	//FTimerDelegate EndTurnDelegate;
	//EndTurnDelegate.BindUFunction(this, FName("EndPlayerTurn"), bAttacked);
	//GetWorld()->GetTimerManager().SetTimer(EndTurnTimerHandle, EndTurnDelegate, EndTurnDelay, false);

	// 공격 후 공격 방향 초기화
	AttackDirection = FVector::ZeroVector;
	
	

}
bool AMyPlayer::AttackZombieAtLocation(FVector Location)
{
	
	TArray<AActor*> OutZombies;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TArray<AActor*> IgnoreActors;

	bool bZombieFound = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Location,
		25.0f,
		ObjectTypes,
		AZombieCharacter::StaticClass(),
		IgnoreActors,
		OutZombies
	);

	if (bZombieFound)
	{
		for (AActor* ZombieActor : OutZombies)
		{
			AZombieCharacter* Zombie = Cast<AZombieCharacter>(ZombieActor);
			if (Zombie)
			{
				Zombie->HaveDamage(100.0f);
				UE_LOG(LogTemp, Warning, TEXT("Zombie->HaveDamage(100.0f);"));
				GameMode->EndPlayerTurn();
				return true;
			}
		}
	}
	return false;
}

void AMyPlayer::Playerhit(float hit,FVector Location)
{
	PlayerHealth-=hit;
	
	HitAinm();
	FVector PlayerLocation = GetActorLocation();

	// Z값(높이)을 같게 맞춰서 수평 회전만 하도록
	PlayerLocation.Z = Location.Z;

	FRotator LookAtRotation = (Location-PlayerLocation).Rotation();
	SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));
	if (PlayerHealth<=0)
	{
		Die();
	}
}

void AMyPlayer::Die()
{
	playAnim->Anim_die=true;
	GameMode->HandleGameOver();
}

void AMyPlayer::HitAinm()
{
	playAnim->Anim_hit=true;
}
