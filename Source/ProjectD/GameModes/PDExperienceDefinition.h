// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "PDExperienceDefinition.generated.h"

class UGameFeatureAction;
class UPDPawnData;
class UPDExperienceActionSet;

/**
 * Definition of an experience
 */
UCLASS(BlueprintType, Const)
class PROJECTD_API UPDExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPDExperienceDefinition();

	//~UObject interface (현재 호출되는 지점 못찾음)
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface
	
public:
	// 이 Experience 가 활성화하길 원하는 Game Features Plugin 목록.
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	/** 플레이어를 위해 생성할 기본 폰 클래스 */
	//@TODO: Make soft?
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UPDPawnData> DefaultPawnData;

	// 이 경험이 로드되거나 활성화되거나 비활성화되거나 언로드될 때 수행할 행동 목록.
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of additional action sets to compose into this experience
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<UPDExperienceActionSet>> ActionSets;
};
