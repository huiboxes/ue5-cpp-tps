// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay) {
	if (DisplayText) {
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}

}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn) {
	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole) {
	case ROLE_None:
		Role = FString("None");
		break;
	case ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;
	case ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;
	case ROLE_Authority:
		Role = FString("Authority");
		break;
	}
	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	SetDisplayText(LocalRoleString);

}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) {
	RemoveFromParent(); // 从父节点移除当前组件
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);


}
