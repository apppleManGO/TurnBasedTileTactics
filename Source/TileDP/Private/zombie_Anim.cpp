// Fill out your copyright notice in the Description page of Project Settings.


#include "zombie_Anim.h"
void Uzombie_Anim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner(); // 현재 애니메이션이 적용된 Pawn(좀비) 가져오기
	if (Pawn)
	{
		Speed = Pawn->GetVelocity().Size(); // 속도를 계산하여 Speed 변수 업데이트
	}
}