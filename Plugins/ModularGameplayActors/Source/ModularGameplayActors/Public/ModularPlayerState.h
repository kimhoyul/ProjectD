// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"

#include "ModularPlayerState.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UObject;

/** GameFeaturePlugins에 의한 확장을 지원하는 초기 클래스 */
UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	//~ Begin AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~ End AActor interface

protected:
	//~ Begin APlayerState interface
	virtual void CopyProperties(APlayerState* PlayerState);
	//~ End APlayerState interface
};
