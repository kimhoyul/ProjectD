// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "PDPawnData.generated.h"

class APawn;
class UPDAbilitySet;
class UPDAbilityTagRelationshipMapping;
class UPDInputConfig;
class UPDCameraMode;

/**
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "ProjectD Pawn Data", ShortTooltip = "폰을 정의하는 데 사용되는 데이터 에셋."))
class PROJECTD_API UPDPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPDPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// 이 폰에 대해 인스턴스화할 클래스 (일반적으로 APDPawn 또는 APDCharacter에서 파생되어야 함)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// 이 폰의 ability system 에 부여할 Ability sets
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	TArray<TObjectPtr<UPDAbilitySet>> AbilitySets;

	// 이 폰이 수행하는 행동에 사용할 ability tags 의 매핑
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	TObjectPtr<UPDAbilityTagRelationshipMapping> TagRelationshipMapping;

	// 플레이어가 조종하는 폰이 입력 매핑을 생성하고 입력 동작을 바인딩하는 데 사용되는 Input configuration
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	TObjectPtr<UPDInputConfig> InputConfig;

	// 플레이어가 조종하는 폰에 의해 사용되는 기본 카메라 모드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	TSubclassOf<UPDCameraMode> DefaultCameraMode;
};
