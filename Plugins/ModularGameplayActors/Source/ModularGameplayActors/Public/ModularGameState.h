﻿// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameState.h"

#include "ModularGameState.generated.h"

class UObject;

/** 이 클래스와 ModularGameModeBase 가 한 쌍으로 사용 됨 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface
};


/** 이 클래스와 ModularGameMode 가 한 쌍으로 사용 됨 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularGameState : public AGameState
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

protected:
	//~ Begin AGameState interface
	virtual void HandleMatchHasStarted() override;
	//~ Begin AGameState interface
};
