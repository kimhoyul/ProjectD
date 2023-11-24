// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayMessageSubsystem.h"
#include "GameplayMessageTypes2.h"

#include "AsyncAction_ListenForGameplayMessage.generated.h"

class UScriptStruct;
class UWorld;
struct FFrame;

/**
 * ProxyObject 핀은 K2Node_GameplayMessageAsyncAction에서 숨겨집니다. DELEGATE 트리거링을 위한 객체 참조를 얻기 위해 사용됩니다.
 *
 * @param ActualChannel		수신한 Payload의 실제 메시지 채널 (Channel로 시작하지만 부분 매치가 활성화된 경우 더 구체적일 수 있음)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGameplayMessageDelegate, UAsyncAction_ListenForGameplayMessage*, ProxyObject, FGameplayTag, ActualChannel);

UCLASS(BlueprintType, meta=(HasDedicatedAsyncNode))
class GAMEPLAYMESSAGERUNTIME_API UAsyncAction_ListenForGameplayMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()

	public:
	/**
	 * 지정된 채널에서 게임플레이 메시지가 방송될 때까지 비동기적으로 대기합니다.
	 *
	 * @param Channel			듣고자 하는 메시지 채널
	 * @param PayloadType		사용할 메시지 구조의 종류 (이는 발신자가 방송하는 타입과 일치해야 함)
	 * @param MatchType			방송된 메시지와 채널을 매치하는 데 사용되는 규칙
	 */
	UFUNCTION(BlueprintCallable, Category = Messaging, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ListenForGameplayMessage* ListenForGameplayMessages(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* PayloadType, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch);

	/**
	 * 방송된 게임플레이 메시지에서 수신한 페이로드를 지정된 와일드카드에 복사하려고 시도합니다.
	 * 와일드카드의 타입은 수신된 메시지의 타입과 일치해야 합니다.
	 *
	 * @param OutPayload	페이로드가 복사될 와일드카드 참조
	 * @return				복사가 성공적이었는지 여부
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

public:
	/** 지정된 채널에서 메시지가 방송될 때 호출됩니다. 메시지 페이로드를 요청하려면 GetPayload()를 사용하세요. */
	UPROPERTY(BlueprintAssignable)
	FAsyncGameplayMessageDelegate OnMessageReceived;

private:
	void HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload);

private:
	const void* ReceivedMessagePayloadPtr = nullptr;

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	TWeakObjectPtr<UScriptStruct> MessageStructType = nullptr;
	EGameplayMessageMatch MessageMatchType = EGameplayMessageMatch::ExactMatch;

	FGameplayMessageListenerHandle ListenerHandle;
};
