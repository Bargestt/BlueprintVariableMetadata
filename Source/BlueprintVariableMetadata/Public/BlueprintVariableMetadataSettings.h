// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BlueprintVariableMetadataSettings.generated.h"





/**  */
USTRUCT()
struct FMetadataFilterGroup
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	FString Name;

	/** Comma separated list of property types */
	UPROPERTY(EditAnywhere)
	FString RestrictToTypes;

	/** Blueprint must be child of any of specified classes */
	UPROPERTY(EditAnywhere, meta = (AllowAbstract=true))
	TArray<FSoftClassPath> BlueprintNamespace;

	UPROPERTY(EditAnywhere)
	FString Description;

};




/**  */
USTRUCT()
struct FMetaDataOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnabled;


	UPROPERTY(EditAnywhere)
	FString Key;

	UPROPERTY(EditAnywhere)
	FString DefaultValue;

	UPROPERTY(EditAnywhere)
	FString Category;


	UPROPERTY(EditAnywhere, meta = (GetOptions = "BlueprintVariableMetadata.BlueprintVariableMetadataSettings.GetGroupOptions"))
	FString Group;

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

public:
};



/**
 * 
 */
UCLASS(Config = Editor, defaultconfig, meta = (DisplayName = "Blueprint Variable Metadata"))
class BLUEPRINTVARIABLEMETADATA_API UBlueprintVariableMetadataSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UBlueprintVariableMetadataSettings();
	virtual void PostInitProperties() override;	
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

public:
	UPROPERTY(VisibleAnywhere, Instanced, Category = "Default")
	TObjectPtr<class UDefaultBlueprintVariableMetadataSettings> DefaultOptions;


	UPROPERTY(config, EditAnywhere, Category = "Display")
	bool bShowInAdvanced;

	/** Show restricted options in meta data list. You wont be able to edit them */
	UPROPERTY(config, EditAnywhere, Category = "Display")
	bool bShowRestrictedOptions;

	/** Non editable meta data options. User specified */
	UPROPERTY(config, EditAnywhere, Category = "Display")
	TSet<FName> RestrictedOptions;


	/** Show options that were already added */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bShowAddedOptions;

//	#TODO
// 	UPROPERTY(config, EditAnywhere, Category = "Add New")
// 	bool bPreserveOrder;

	/** Disable filtering by property type. It will show everything */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bEnableFiltering;

	/** Disable custom key option. */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bShowCustomOption;

	UPROPERTY(config, EditAnywhere, Category = "Add New")
	bool bEnableCategories;

	/** Display categories in same order as in here. Missing properties are pushed to bottom */
	UPROPERTY(config, EditAnywhere, Category = "Add New", meta = (EditCondition = "bEnableCategories"))
	TArray<FString> CategoryOrder;

	/** Group is predefined filter for metadata. Also can combine meta in categories */
	UPROPERTY(config, EditAnywhere, Category = "Add New", meta = (TitleProperty = "{Group}: {Description}\n{RestrictToTypes}"))
	TArray<FMetadataFilterGroup> Groups;

	/** Options when adding new metadata. Check ObjectMacros.h for info */
	UPROPERTY(config, EditAnywhere, Category = "Add New")
	TArray<FMetaDataOption> Options;


public:
	void Rebuild();

	UFUNCTION()
	static TArray<FString> GetGroupOptions();

	TMap<FName, FMetaDataOption> CollectAllOptions() const;
	TMap<FName, FMetadataFilterGroup> CollectAllGroups() const;


	const FMetaDataOption* GetOptionPtr(FName Key) const;
	const FMetaDataOption& GetOption(FName Key) const;

	bool IsRestricted(FName Key) const;

	bool IsReadOnly_Key(FName Key) const;


	FString GetMetaCategory(const FName& Key) const;
	int32 GetCategoryPriority(const FString& Category) const;
	


private:
	FMetaDataOption InvalidOption;

	TMap<FName, int32> MetaDataIndex;
	TMap<FName, int32> MetaDataIndex_Default;
};


/**
 * Default settings
 */
UCLASS(config = BlueprintVariableMetadata, defaultconfig)
class BLUEPRINTVARIABLEMETADATA_API UDefaultBlueprintVariableMetadataSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Editable using DefaultBlueprintVariableMetadata.ini */
	UPROPERTY(config, VisibleAnywhere, Category = "Default Meta Data")
	TArray<FString> DefaultCategoryOrder;

	/** Editable using DefaultBlueprintVariableMetadata.ini */
	UPROPERTY(config, VisibleAnywhere, Category = "Default Meta Data")
	TArray<FName> RestrictedOptions;
	
	/** Editable using DefaultBlueprintVariableMetadata.ini */
	UPROPERTY(config, VisibleAnywhere, Category = "Default Meta Data", meta = (TitleProperty = "Group"))
	TArray<FMetadataFilterGroup> Groups;

	/** Editable using DefaultBlueprintVariableMetadata.ini */
	UPROPERTY(config, VisibleAnywhere, Category = "Default Meta Data")
	TArray<FMetaDataOption> Options;
};
