// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);
	
	// 它的类型是 TObjectPtr,它是 UE5 引入的一种新包装器
	// TObjectPtr 用于引擎类中的一些变量，通过它可以检测到什么时候对象正在被使用
	// 使用 Get 函数可以访问有这个对象指针存储的游戏状态指针
	// 获取游戏数据中的玩家数组的大小，也就是玩家数
	int32 NumberOfPlayer = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayer == 2) { // 当玩家数大于设定数值时，切换到正式游戏关卡
		UWorld* World = GetWorld();
		if (World) {
			bUseSeamlessTravel = true;
			World->ServerTravel("/Game/Maps/BlasterMap?listen");
		}
	}
}
