// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BlueprintVariableMetadataSettings.generated.h"


/**  */
USTRUCT()
struct FMetaDataOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled;

	UPROPERTY(EditAnywhere)
	FString DefaultValue;

	/** Comma separated list of property types */
	UPROPERTY(EditAnywhere)
	FString RestrictToTypes;

	UPROPERTY(EditAnywhere)
	FString Description;
	
	FMetaDataOption(bool bEnabled = true, FString DefaultValue = TEXT(""), FString RestrictToTypes = TEXT(""), FString Description = TEXT(""))
		: bEnabled(bEnabled)
		, DefaultValue(DefaultValue)
		, RestrictToTypes(RestrictToTypes)		
		, Description(Description)
	{ }
};


/**
 * 
 */
UCLASS(Config = EditorPerProjectUserSettings, defaultconfig, meta = (DisplayName = "Blueprint Variable Metadata"))
class BLUEPRINTVARIABLEMETADATA_API UBlueprintVariableMetadataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	virtual FName GetContainerName() const override { return TEXT("Project"); }
	virtual FName GetCategoryName() const override { return TEXT("Editor"); }

	UBlueprintVariableMetadataSettings();
	virtual void PostInitProperties() override;	
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	void AddDefaultMeta(FName Key, bool bEnabled = true, FString DefaultValue = TEXT(""), FString RestrictToTypes = TEXT(""), FString Description = TEXT(""));
public:
	FMetaDataOption GetOption(FName Key) const;
	bool IsRestricted(FName Key) const;

public:
	
	/** Show restricted options in meta data list. You wont be able to edit them */
	UPROPERTY(config, EditAnywhere, Category = "Display")
	bool bShowRestrictedOptions;


	/** Non editable meta data options */
	UPROPERTY(VisibleAnywhere, Category = "Display")
	TSet<FName> SystemRestrictedOptions;

	/** Non editable meta data options. User specified */
	UPROPERTY(config, EditAnywhere, Category = "Display")
	TSet<FName> UserRestrictedOptions;


	/** Show options that were already added */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bShowAddedOptions;

	/** Disable custom key option. Forces MetaDataOptions use  */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bDisableCustomOption;

	/** Display property class in metadata category */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bDisplayPropertyType;

	/** Options when adding new metadata. Check ObjectMacros.h for info */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	TMap<FName, FMetaDataOption> MetaDataOptions;

	UPROPERTY(VisibleAnywhere, Category = "Add New", meta = (NoClear, EditFixedSize, NoResetToDefault))
	TMap<FName, FMetaDataOption> DefaultOptions;

};
