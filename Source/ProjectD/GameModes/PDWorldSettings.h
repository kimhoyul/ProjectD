// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "PDWorldSettings.generated.h"

class UPDExperienceDefinition;

/**
 * 이 맵을 플레이할 때 사용할 DefaultGameplayExperience 을 설정하는 데 사용할 기본 월드 설정 객체
 */
UCLASS()
class PROJECTD_API APDWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	APDWorldSettings(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

public:
	// DefaultGameplayExperience 가 재정의되지 않은 경우 서버가 이 맵을 열 때 사용할 DefaultGameplayExperience 의 FPrimaryAssetId 를 반환
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// DefaultGameplayExperience 가 재정의되지 않은 경우 서버가 이 맵을 열 때 사용할 DefaultGameplayExperience
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UPDExperienceDefinition> DefaultGameplayExperience;

#if WITH_EDITORONLY_DATA
	// 이 맵이 front-end 또는 standalone 으로 실행할 GameplayExperience 라면,
	// true 로 설정하면 에디터에서 플레이를 누르면 넷 모드가 강제로 standalone 으로 전환.
	UPROPERTY(EditDefaultsOnly, Category=PIE)
	bool ForceStandaloneNetMode = false;
#endif
};
