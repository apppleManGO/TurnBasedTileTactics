// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZombieAIController.h"
#include "zombie_Anim.generated.h"

/**
 * 
 */
UCLASS()
class TILEDP_API Uzombie_Anim : public UAnimInstance
{
	GENERATED_BODY()
	public:
        virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Anim_attack=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Anim_hit=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Anim_die=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Anim_move=false;

	
};
