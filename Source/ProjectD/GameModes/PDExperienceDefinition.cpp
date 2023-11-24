// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/PDExperienceDefinition.h"
#include "GameFeatureAction.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UPDExperienceDefinition::UPDExperienceDefinition()
{
}

#if WITH_EDITOR
EDataValidationResult UPDExperienceDefinition::IsDataValid(FDataValidationContext& Context) const
{
	// ���� ȣ��Ǵ� ���� ��ã��

	return EDataValidationResult();
}
#endif


#if WITH_EDITORONLY_DATA
void UPDExperienceDefinition::UpdateAssetBundleData()
{
	Super::UpdateAssetBundleData();

	for (UGameFeatureAction* Action : Actions)
	{
		if (Action)
		{
			Action->AddAdditionalAssetBundleData(AssetBundleData);
		}
	}
}
#endif // WITH_EDITORONLY_DATA

