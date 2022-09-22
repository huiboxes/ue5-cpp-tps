// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//
// 创建自定义委托用于菜单类绑定回调
//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResult, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);


UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();



	// 处理 Session 的函数，让其它类也可以调用

	/**
	 * 创建 Session
	 * 
	 * int32 NumPublicConnections 公开连接数
	 * FString MatchType 匹配类型
	 */ 
	void CreateSession(int32 NumPublicConnections, FString MatchType);

	/**
	 * 查找 Session
	 *
	 * int32 MaxSearchResults 最大搜索结果数
	 */
	void FindSessions(int32 MaxSearchResults);

	/**
	 * 加入 Session
	 *
	 * const FOnlineSessionSearchResult& 查找到的 Session 结果中的某个 Session
	 */
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	/**
	 * 销毁 Session
	 */
	void DestroySession();

	//
	//创建自定义委托用于菜单类绑定回调
	//
	FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
	FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
	FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
	FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
	FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;


	/**
	 * 开始 Session
	 */
	void StartSession();

protected:
	// 内部回调函数，用于添加到 Online Session Interface 委托列表
	// 这个 Class 之外不可调用

	// 创建 Session 后的回调
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	// 查找 Sessions 后的回调
	void OnFindSessionsComplete(bool bWasSuccessful);
	// 加入 Session 后的回调
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	// 销毁 Session 后的回调
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	// 开始 Session 后的回调
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);


private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	// 添加到 Online Session Interface 委托列表
	// MultiplayerSessionsSubsystem 内部回调绑定到这些委托上
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

	bool bCreateSessionOnDestroy{ false };
	int32 LastNumPublicConnections;
	FString LastMatchType;

};
