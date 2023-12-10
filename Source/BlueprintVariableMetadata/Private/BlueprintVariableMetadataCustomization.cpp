// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#include "BlueprintVariableMetadataCustomization.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include <IDetailGroup.h>
#include "PropertyCustomizationHelpers.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include <Widgets/Input/SComboButton.h>


#define LOCTEXT_NAMESPACE "BlueprintVariableMetadataCustomization"

TSharedPtr<IDetailCustomization> FBlueprintVariableMetadataCustomization::MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor)
{
	const TArray<UObject*>* Objects = (InBlueprintEditor.IsValid() ? InBlueprintEditor->GetObjectsCurrentlyBeingEdited() : nullptr);
	if (Objects && Objects->Num() == 1)
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>((*Objects)[0]))
		{
			return MakeShareable(new FBlueprintVariableMetadataCustomization(InBlueprintEditor, Blueprint));
		}
	}

	return nullptr;
}

FBlueprintVariableMetadataCustomization::FBlueprintVariableMetadataCustomization(TSharedPtr<IBlueprintEditor> InBlueprintEditor, UBlueprint* Blueprint) 
	: BlueprintEditorPtr(InBlueprintEditor)
	, BlueprintPtr(Blueprint)
{

}

void FBlueprintVariableMetadataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	PropertyBeingCustomized.Reset();
	VariableName = NAME_None;	

	const FProperty* TargetProperty = nullptr;
	int32 VariableIndex = INDEX_NONE;

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 0)
	{
		UPropertyWrapper* PropertyWrapper = Cast<UPropertyWrapper>(ObjectsBeingCustomized[0].Get());
		PropertyBeingCustomized = PropertyWrapper ? PropertyWrapper->GetProperty() : nullptr;
		if (PropertyBeingCustomized.IsValid())
		{	
			VariableName = PropertyBeingCustomized->GetFName();
			VariableIndex = FBlueprintEditorUtils::FindNewVariableIndex(BlueprintPtr.Get(), VariableName);
            if (VariableIndex != INDEX_NONE)
            {
				TargetProperty = PropertyBeingCustomized.Get();
            }
		}
	}
	if (TargetProperty == nullptr)
	{
		return;
	}

	const UBlueprintVariableMetadataSettings* Settings = GetDefault<UBlueprintVariableMetadataSettings>();

	const FText ControlPanelHeaderText = LOCTEXT("ControlPanel_Header", "Metadata");
	const FText ControlPanelRowText = LOCTEXT("ControlPanel", "MetaData Control Panel");				
	const FText ControlPanelHeaderTooltipText = LOCTEXT("ControlPanel_Tooltip", "{0}\n\nVariable meta data\nUse with caution!\nConsult with ObjectMacros.h for available options");
	const FText ControlPanelAddText = LOCTEXT("ControlPanel_Add", "Add");
	const FText MetadataRowText = LOCTEXT("MetadataRow", "Metadata");
	const FText PropertyInfo = FText::FromString(FString::JoinBy(GetPropertyInfo(PropertyBeingCustomized.Get()), TEXT(", "), [](const FName& Entry) { return Entry.ToString(); }));
				

	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Variable");
	IDetailGroup* CategoryGroup = nullptr;
	if (!Settings->bShowInAdvanced)
	{
		CategoryGroup = &Category.AddGroup(TEXT("Meta"), INVTEXT("Meta"));					
	}
					
	// Create control panel
	// Title | [Button]
	FDetailWidgetRow& ControlRow = CategoryGroup ? CategoryGroup->HeaderRow() : Category.AddCustomRow(ControlPanelRowText, true);
	ControlRow
		.NameContent()
		[
			SNew(STextBlock)
			.ToolTipText(FText::Format(ControlPanelHeaderTooltipText, PropertyInfo))
			.Text(ControlPanelHeaderText)
			.Font(IDetailLayoutBuilder::GetDetailFont())						
		]
		.ValueContent()
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SComboButton)
				.OnGetMenuContent(this, &FBlueprintVariableMetadataCustomization::GetAddMetaDataDropdown)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Font(IDetailLayoutBuilder::GetDetailFont())
					.Text(ControlPanelAddText)
				]
			]
		];
			
	
	// Build metadata entries				
    for(const FBPVariableMetaDataEntry& MetaDataEntry : BlueprintPtr->NewVariables[VariableIndex].MetaDataArray)
    {
		const FName& MetaKey = MetaDataEntry.DataKey;
		const FString& MetaValue = MetaDataEntry.DataValue;

		bool bIsRestricted = Settings->IsRestricted(MetaKey);					
		if (bIsRestricted && !Settings->bShowRestrictedOptions)
		{
			continue;
		}
		bool bIsKeyReadOnly = bIsRestricted || Settings->IsReadOnly_Key(MetaKey);


		// Metadata description text
		const FText TooltipText = FText::FromString(Settings->GetOption(MetaKey).Description);

		TSharedPtr<SWidget> KeyWidget;
		if (bIsKeyReadOnly)
		{
			KeyWidget = SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(FText::FromName(MetaKey))
				.ToolTipText(TooltipText);
		}
		else 
		{
			KeyWidget = SNew(SEditableTextBox)							
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(FText::FromName(MetaKey))
				.ToolTipText(TooltipText)
				.OnVerifyTextChanged(FOnVerifyTextChanged::CreateSP(this, &FBlueprintVariableMetadataCustomization::VerifyMetaKey))
				.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FBlueprintVariableMetadataCustomization::MetaKeyChanged, MetaKey))
				.IsReadOnly(bIsRestricted || bIsKeyReadOnly)
				.SelectAllTextWhenFocused(true)
				.ClearKeyboardFocusOnCommit(false)
				.SelectAllTextOnCommit(true);
		}
            		
					
		FDetailWidgetRow& MetaRow = CategoryGroup ? CategoryGroup->AddWidgetRow() : Category.AddCustomRow(MetadataRowText, true);
		MetaRow
			.NameContent()
			[
				KeyWidget.ToSharedRef()
			]
			.ValueContent()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
					.MinDesiredWidth(50.0f)
					[
						SNew(SMultiLineEditableTextBox)
						.Text(FText::FromString(MetaDataEntry.DataValue))
						.Font(IDetailLayoutBuilder::GetDetailFont())
						.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FBlueprintVariableMetadataCustomization::MetaValueChanged, MetaKey))
						.IsReadOnly(bIsRestricted)
						.ModiferKeyForNewLine(EModifierKey::Shift)
						.SelectAllTextWhenFocused(false)
						.ClearKeyboardFocusOnCommit(false)								
						.SelectAllTextOnCommit(false)
						.WrapTextAt(200.0f)
					]
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[								
					PropertyCustomizationHelpers::MakeRemoveButton(
						FSimpleDelegate::CreateSP(this, &FBlueprintVariableMetadataCustomization::RemoveMetadata, MetaKey),
						FText::GetEmpty(), 
						!bIsRestricted)								
				]
			];
    }
}

TSharedRef<SWidget> FBlueprintVariableMetadataCustomization::GetAddMetaDataDropdown()
{
	FMenuBuilder MenuBuilder(true, nullptr);

	const UBlueprintVariableMetadataSettings* Settings = GetDefault<UBlueprintVariableMetadataSettings>();

	const FText MetadataAddTooltipText = LOCTEXT("MetadataAdd_Tooltip", "Adds new meta data");
	const FText NoMetaDataToAdd = LOCTEXT("MetadataAdd_NoOptions", "Nothing to add");

	const auto CanExecute_False = FCanExecuteAction::CreateLambda([]() { return false; });
	
	bool bIsEmpty = true;	

	UClass* BlueprintClass = nullptr;
	if (UBlueprint* Blueprint = BlueprintPtr.Get())
	{
		if (Blueprint->GeneratedClass)
		{
			BlueprintClass = Blueprint->GeneratedClass->GetAuthoritativeClass();
		}		
	}
	
	if (BlueprintClass == nullptr)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MetadataAdd_BlueprintError", "Blueprint error"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction(),
				CanExecute_False,
				FIsActionChecked(),
				FIsActionButtonVisible()
			)
		);
		return MenuBuilder.MakeWidget();
	}

	const FProperty* TargetProperty = PropertyBeingCustomized.Get();
	if (TargetProperty == nullptr)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MetadataAdd_PropertyError", "Property error"),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(
				FExecuteAction(),
				CanExecute_False,
				FIsActionChecked(),
				FIsActionButtonVisible()
			)
		);
		return MenuBuilder.MakeWidget();
	}

	
	if (Settings->bShowCustomOption)
	{
		bIsEmpty = false;
		MenuBuilder.AddMenuEntry(
			LOCTEXT("MetadataAddOption_New", "Custom"),
			MetadataAddTooltipText,
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &FBlueprintVariableMetadataCustomization::AddMetadata, FName("None")),
				FCanExecuteAction(),
				FIsActionChecked(),
				FIsActionButtonVisible()
			)
		);
	}
	
	struct FDisplayedMetaOption
	{
		bool bEnabled;
		const FMetaDataOption* Option;
	};
	TMap<FString, TArray<FDisplayedMetaOption>> DisplayedMetas;

	TMap<FName, FMetadataFilterGroup> AllGroups = Settings->CollectAllGroups();
	TMap<FName, FMetaDataOption> AllOptions = Settings->CollectAllOptions();	
	for (const auto& OptionPair : AllOptions)
	{
		const FName MetaKey = OptionPair.Key;
		const FMetaDataOption& OptionSettings = OptionPair.Value;

		if (MetaKey.IsNone())
		{
			continue;
		}

		if (!OptionSettings.bEnabled)
		{
			continue;
		}

		const FMetadataFilterGroup* OptionGroup = AllGroups.Find(*OptionSettings.Group);

		if (Settings->bEnableFiltering)
		{
			bool bChildOfAny = true;

			if (OptionGroup && OptionGroup->BlueprintNamespace.Num() > 0)
			{
				for (const FSoftClassPath& Path : OptionGroup->BlueprintNamespace)
				{
					if (Path.IsNull() || BlueprintClass->IsChildOf(Path.ResolveClass()))
					{
						bChildOfAny = true;
						break;
					}
				}
			}			

			if (!bChildOfAny)
			{
				continue;
			}

			if (!IsPropertyAllowed(TargetProperty, OptionSettings.RestrictToTypes))
			{
				continue;
			}

			if (OptionGroup && !IsPropertyAllowed(TargetProperty, OptionGroup->RestrictToTypes))
			{
				continue;
			}
		}

		bool bAlreadyExists = false;
		{
			FString Value;
			bAlreadyExists = FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, MetaKey, Value);
		}
		if (bAlreadyExists && !Settings->bShowAddedOptions)
		{
			continue;
		}

		FDisplayedMetaOption DisplayedOption;
		DisplayedOption.bEnabled = !bAlreadyExists;
		DisplayedOption.Option = &OptionSettings;
		DisplayedMetas.FindOrAdd(Settings->GetMetaCategory(MetaKey)).Add(DisplayedOption);
	}


	DisplayedMetas.KeyStableSort([Settings](const FString& A, const FString& B)
	{
		return Settings->GetCategoryPriority(A) < Settings->GetCategoryPriority(B);
	});
	for (const auto& Category : DisplayedMetas)
	{
		const TArray<FDisplayedMetaOption>& Entries = Category.Value;

		MenuBuilder.BeginSection(*Category.Key, FText::FromString(Category.Key));

		for (const FDisplayedMetaOption& DisplayedOption : Entries)
		{
			const FMetaDataOption& Option = *DisplayedOption.Option;
			bool bEnabled = DisplayedOption.bEnabled;

			MenuBuilder.AddMenuEntry(
				FText::FromString(Option.Key),
				FText::FromString(Option.Description),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateSP(this, &FBlueprintVariableMetadataCustomization::AddMetadata, FName(*Option.Key)),
					FCanExecuteAction::CreateLambda([bEnabled]() { return bEnabled; }),
					FIsActionChecked(),
					FIsActionButtonVisible()
				)
			);

			bIsEmpty = false;
		}

		MenuBuilder.EndSection();
	}

	if (bIsEmpty)
	{
		MenuBuilder.AddMenuEntry(
			NoMetaDataToAdd, 
			FText::GetEmpty(), 
			FSlateIcon(), 
			FUIAction(
				FExecuteAction(),
				FCanExecuteAction::CreateLambda([]() { return false; }),
				FIsActionChecked(),
				FIsActionButtonVisible()
			)
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void FBlueprintVariableMetadataCustomization::AddMetadata(FName MetaDataKey)
{
	if (MetaDataKey.IsNone())
	{
		for (int32 Index = 0; ; Index++)
		{
			MetaDataKey = *FString::Printf(TEXT("MetaData_%d"), Index);

			FString Value;
			if (!FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, MetaDataKey, Value))
			{
				break;
			}
		}
	}

	const FMetaDataOption& MetaOption = GetDefault<UBlueprintVariableMetadataSettings>()->GetOption(MetaDataKey);

	FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, MetaDataKey, MetaOption.DefaultValue);
}

void FBlueprintVariableMetadataCustomization::RemoveMetadata(FName MetaDataKey)
{
	FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, MetaDataKey);
}

bool FBlueprintVariableMetadataCustomization::VerifyMetaKey(const FText& NewKeyName, FText& ErrorMessage)
{
	FName NewKey = *NewKeyName.ToString();
	if (NewKey.IsNone())
	{
		ErrorMessage = LOCTEXT("VerifyMetaKey_Empty", "Key cannot be empty");
		return false;
	}

	if (GetDefault<UBlueprintVariableMetadataSettings>()->IsRestricted(NewKey))
	{
		ErrorMessage = LOCTEXT("VerifyMetaKey_Restricted", "Key name is resticted");
		return false;
	}

	return true;
}

void FBlueprintVariableMetadataCustomization::MetaKeyChanged(const FText& NewKeyName, ETextCommit::Type ChangeType, FName OldKeyName)
{
	FName NewKey = *NewKeyName.ToString();

	FString OldValue;
	if (!NewKey.IsNone() && NewKey != OldKeyName && FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, OldKeyName, OldValue))
	{
		if (!GetDefault<UBlueprintVariableMetadataSettings>()->IsRestricted(NewKey))
		{
			FBlueprintEditorUtils::RemoveBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, OldKeyName);

			FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, NewKey, OldValue);
		}		
	}
}

void FBlueprintVariableMetadataCustomization::MetaValueChanged(const FText& NewValueName, ETextCommit::Type ChangeType, FName KeyName)
{
	FString NewValue = NewValueName.ToString();

	FString OldValue;
	if (FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, KeyName, OldValue) && OldValue != NewValue)
	{
		FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, KeyName, NewValue);
	}
}


TArray<FName> FBlueprintVariableMetadataCustomization::GetPropertyInfo(const FProperty* Property)
{
	TArray<FName> Info;

	if (Property)
	{
		FName Typename;
		if (GetPropertyTypeName(Property, Typename))
		{
			Info.Add(Typename);
		}

		if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
		{
			if (GetPropertyTypeName(ArrayProperty->Inner, Typename))
			{
				Info.Add(Typename);
			}
		}
		else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
		{
			if (GetPropertyTypeName(SetProperty->ElementProp, Typename))
			{
				Info.Add(Typename);
			}
		}
		else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
		{
			if (GetPropertyTypeName(MapProperty->KeyProp, Typename))
			{
				Info.Add(Typename);
			}
			if (GetPropertyTypeName(MapProperty->ValueProp, Typename))
			{
				Info.Add(Typename);
			}
		}
	}


	return Info;
}

bool FBlueprintVariableMetadataCustomization::GetPropertyTypeName(const FProperty* Property, FName& PropertyType)
{
	if (Property)
	{
		if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
		{
			if (StructProperty->Struct)
			{
				PropertyType = StructProperty->Struct->GetFName();
				return true;
			}
		}
		else if (Property->GetClass())
		{
			PropertyType = Property->GetClass()->GetFName();
			return true;
		}
	}

	PropertyType = NAME_None;
	return false;
}


bool FBlueprintVariableMetadataCustomization::IsPropertyAllowed(const FProperty* TargetProperty, const FString& Filter)
{
	if (Filter.IsEmpty())
	{
		return true;
	}

	if (!TargetProperty)
	{
		return false;
	}

	TArray<FName> AllowedClasses;
	{
		TArray<FString> Arr;
		Filter.ParseIntoArray(Arr, TEXT(","));
		for (FString Str : Arr)
		{
			AllowedClasses.Add(FName(Str.TrimStartAndEnd()));
		}
	}

	if (AllowedClasses.Num() == 0)
	{
		return true;
	}
	
	TArray<FName> Info = GetPropertyInfo(TargetProperty);

	for (const FName& PropClass : Info)
	{
		if (AllowedClasses.Contains(PropClass))
		{
			return true;
		}
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
