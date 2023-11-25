// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"

#include "ModularAIController.generated.h"

class UObject;

/** GameFeaturePlugins에 의한 확장을 지원하는 초기 클래스 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularAIController : public AAIController
{
	GENERATED_BODY()

public:
	//~ Begin AActor Interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface
};
