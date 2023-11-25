// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"

#include "ModularGameMode.generated.h"

class UObject;

/** 이 클래스와 ModularGameStateBase 가 한 쌍으로 사용 됨 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
	AModularGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

/** 이 클래스와 ModularGameState 가 한 쌍으로 사용 됨 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularGameMode : public AGameMode
{
	GENERATED_BODY()

	public:
	AModularGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};