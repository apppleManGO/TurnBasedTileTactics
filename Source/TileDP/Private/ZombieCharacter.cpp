// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieCharacter.h"
#include "MyGameModeBase.h"
#include "zombie_Anim.h"
#include "TimerManager.h"
#include "MyPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AZombieCharacter::AZombieCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Health = 110.0f; // 기본 체력
	ConstructorHelpers::FClassFinder<UAnimInstance>temp(TEXT("/Game/BP/ABP_Zombie_anim"));
	if(temp.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(temp.Class);
	}
}

// Called when the game starts or when spawned
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AMyGameModeBase>(GetWorld()->GetAuthGameMode());
	ZombieAnim = Cast<Uzombie_Anim>(GetMesh()->GetAnimInstance());
	// 월드에서 플레이어 가져오기
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerPawn = Cast<AMyPlayer>(PlayerActor);
}

// Called every frame
void AZombieCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieCharacter::InitializeHealth(float InitialHealth)
{
	Health = InitialHealth;
}

void AZombieCharacter::HaveDamage(float DamageAmount)
{
	Health -= DamageAmount;
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector EnemyLocation = GetActorLocation();
        
	FRotator LookAtRotation = (PlayerLocation - EnemyLocation).Rotation();
	
        
	SetActorRotation(LookAtRotation);
	//UE_LOG(LogTemp, Warning, TEXT("Zombie Attacked %f"), Health);
	if (Health <= 0.0f)
	{
		GameMode->OnZombieDied();
		Die();
	}
	else
	{
		ZombieAnim->Anim_hit=true;
	}
}

void AZombieCharacter::Die()
{
	ZombieAnim->Anim_die=true;

	
}
void AZombieCharacter::DestroyZombie()
{
	Destroy();
}

