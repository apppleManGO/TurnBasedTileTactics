// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyPlayer_anim.generated.h"

/**
 * 
 */
UCLASS()
class TILEDP_API UMyPlayer_anim : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool Anim_attack=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool Anim_hit=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool Anim_die=false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool Anim_move=false;
};
