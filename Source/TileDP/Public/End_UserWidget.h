// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "End_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TILEDP_API UEnd_UserWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 위젯 초기화 함수
	virtual void NativeConstruct() override;

public:
	// UI 업데이트 함수
	//void SetTurnText(int TurnCount);

	// UMG 바인딩할 변수들
	//UPROPERTY(meta = (BindWidget))
	//class UTextBlock* TurnText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RestartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* end;

	// 버튼 클릭 함수
	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnQuitClicked();  // 게임 종료 함수 추가
};
