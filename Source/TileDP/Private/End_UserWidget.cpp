// Fill out your copyright notice in the Description page of Project Settings.


#include "End_UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameModeBase.h"
void UEnd_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼에 클릭 이벤트 바인딩
	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UEnd_UserWidget::OnRestartClicked);
	}

	if (end)
	{
		RestartButton->OnClicked.AddDynamic(this, &UEnd_UserWidget::OnQuitClicked);
	}
}

// void UEnd_UserWidget::SetTurnText(int TurnCount)
// {
// 	if (TurnText)
// 	{
// 		TurnText->SetText(FText::FromString(FString::Printf(TEXT("%d턴 생존 성공!"), TurnCount)));
// 	}
// }

void UEnd_UserWidget::OnRestartClicked()
{
	// 현재 레벨 다시 로드 (게임 재시작)
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetPause(false); // 게임 일시 정지 해제
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false; // 마우스 커서 숨김
	}
    
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UEnd_UserWidget::OnQuitClicked()
{
	// 게임 완전 종료
	FGenericPlatformMisc::RequestExit(false);
}
