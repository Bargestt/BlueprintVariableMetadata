// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.


#include "BlueprintVariableMetadataSettings.h"


UBlueprintVariableMetadataSettings::UBlueprintVariableMetadataSettings()
{
	DefaultOptions = GetMutableDefault<UDefaultBlueprintVariableMetadataSettings>();

	bEnableFiltering = true;
	bShowCustomOption = true;
	bEnableCategories = true;
}

void UBlueprintVariableMetadataSettings::PostInitProperties()
{
	Super::PostInitProperties();

	Options.StableSort([](const FMetaDataOption& A, const FMetaDataOption& B)
	{
		return A.Key < B.Key;
	});
	Rebuild();
}

#if WITH_EDITOR
void UBlueprintVariableMetadataSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	Rebuild();
}
#endif // WITH_EDITOR


const FMetaDataOption* UBlueprintVariableMetadataSettings::GetOptionPtr(FName Key) const
{
	const int32* OptionIndex = MetaDataIndex.Find(Key);
	if (OptionIndex && Options.IsValidIndex(*OptionIndex))
	{
		return &Options[*OptionIndex];
	}

	OptionIndex = MetaDataIndex_Default.Find(Key);
	if (OptionIndex && DefaultOptions->Options.IsValidIndex(*OptionIndex))
	{
		return &DefaultOptions->Options[*OptionIndex];
	}
	return nullptr;
}

const FMetaDataOption& UBlueprintVariableMetadataSettings::GetOption(FName Key) const
{
	const FMetaDataOption* Option = GetOptionPtr(Key);
	return Option ? *Option : InvalidOption;
}

bool UBlueprintVariableMetadataSettings::IsRestricted(FName Key) const
{
	return RestrictedOptions.Contains(Key) || DefaultOptions->RestrictedOptions.Contains(Key);
}

bool UBlueprintVariableMetadataSettings::IsReadOnly_Key(FName Key) const
{
	return !bShowCustomOption || MetaDataIndex.FindRef(Key) != 0;
}


FString UBlueprintVariableMetadataSettings::GetMetaCategory(const FName& Key) const
{
	FString Category = TEXT("");

	if (const FMetaDataOption* Option = GetOptionPtr(Key))
	{
		Category = Option->Category;
	}

	return Category;
}

int32 UBlueprintVariableMetadataSettings::GetCategoryPriority(const FString& Category) const
{
	const TArray<FString>& Order = (CategoryOrder.Num() > 0) ? CategoryOrder : DefaultOptions->DefaultCategoryOrder;

	int32 Index = Order.IndexOfByKey(Category);
	return (Index >= 0) ? Index : MAX_int32;
}

void UBlueprintVariableMetadataSettings::Rebuild()
{
	MetaDataIndex.Reset();
	MetaDataIndex_Default.Reset();

	{
		const TArray<FMetaDataOption>& Source = DefaultOptions->Options;
		for (int32 Index = 0; Index < Source.Num(); Index++)
		{
			FName OptionKey = *Source[Index].Key;
			if (!OptionKey.IsNone())
			{
				MetaDataIndex_Default.Add(OptionKey, Index);
			}
		}
	}

	{
		const TArray<FMetaDataOption>& Source = Options;
		for (int32 Index = 0; Index < Source.Num(); Index++)
		{
			FName OptionKey = *Source[Index].Key;
			if (!OptionKey.IsNone())
			{
				MetaDataIndex_Default.Remove(OptionKey);
				MetaDataIndex.Add(OptionKey, Index);
			}
		}
	}
}

TArray<FString> UBlueprintVariableMetadataSettings::GetGroupOptions()
{
	TArray<FString> Arr;

	const UBlueprintVariableMetadataSettings* Settings = GetDefault<UBlueprintVariableMetadataSettings>();
	if (Settings)
	{
		for (const FMetadataFilterGroup& Group : Settings->Groups)
		{
			Arr.Add(Group.Name);
		}
	}

	//Arr.StableSort();
	Arr.Insert(TEXT("None"), 0);
	return Arr;
}


TMap<FName, FMetaDataOption> UBlueprintVariableMetadataSettings::CollectAllOptions() const
{
	TMap<FName, FMetaDataOption> AllMetaDataOptions;
	for (const FMetaDataOption& Option : DefaultOptions->Options)
	{
		FName OptionKey = *Option.Key;
		if (!OptionKey.IsNone())
		{
			AllMetaDataOptions.Add(OptionKey, Option);
		}
	}
	for (const FMetaDataOption& Option : Options)
	{
		FName OptionKey = *Option.Key;
		if (!OptionKey.IsNone())
		{
			AllMetaDataOptions.Add(OptionKey, Option);
		}
	}

	return AllMetaDataOptions;
}

TMap<FName, FMetadataFilterGroup> UBlueprintVariableMetadataSettings::CollectAllGroups() const
{
	TMap<FName, FMetadataFilterGroup> AllGroups;
	for (const FMetadataFilterGroup& Group : DefaultOptions->Groups)
	{
		FName Key = *Group.Name;
		if (!Key.IsNone())
		{
			AllGroups.Add(Key, Group);
		}
	}
	for (const FMetadataFilterGroup& Group : Groups)
	{
		FName Key = *Group.Name;
		if (!Key.IsNone())
		{
			AllGroups.Add(Key, Group);
		}
	}
	return AllGroups;
}
