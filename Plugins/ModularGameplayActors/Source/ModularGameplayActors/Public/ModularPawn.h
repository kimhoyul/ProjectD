// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Pawn.h"

#include "ModularPawn.generated.h"

class UObject;

/** GameFeaturePlugins에 의한 확장을 지원하는 초기 클래스 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularPawn : public APawn
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor interface

};
