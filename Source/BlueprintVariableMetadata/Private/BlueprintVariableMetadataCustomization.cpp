// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#include "BlueprintVariableMetadataCustomization.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
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

	
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailLayout.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	if (ObjectsBeingCustomized.Num() > 0)
	{
		UPropertyWrapper* PropertyWrapper = Cast<UPropertyWrapper>(ObjectsBeingCustomized[0].Get());
		PropertyBeingCustomized = PropertyWrapper ? PropertyWrapper->GetProperty() : nullptr;
		if (PropertyBeingCustomized.IsValid())
		{			
			VariableName = PropertyBeingCustomized->GetFName();
			int32 VariableIndex = FBlueprintEditorUtils::FindNewVariableIndex(BlueprintPtr.Get(), VariableName);
            if (VariableIndex != INDEX_NONE)
            {
				const UBlueprintVariableMetadataSettings* Settings = GetDefault<UBlueprintVariableMetadataSettings>();

				FText ControlPanelHeaderText = LOCTEXT("ControlPanel_Header", "Metadata");
				const FText ControlPanelRowText = LOCTEXT("ControlPanel", "MetaData Control Panel");				
				const FText ControlPanelHeaderTooltipText = LOCTEXT("ControlPanel_Tooltip", "Variable meta data\nUse with caution!\nConsult with ObjectMacros.h for available options");
				const FText ControlPanelAddText = LOCTEXT("ControlPanel_Add", "Add");
				const FText MetadataRowText = LOCTEXT("MetadataRow", "Metadata");


				if (Settings->bDisplayPropertyType)
				{
					FString PropertyClassName;
					if (FStructProperty* StructProperty = CastField<FStructProperty>(PropertyBeingCustomized.Get()))
					{
						PropertyClassName = StructProperty->Struct ? StructProperty->Struct->GetName() : TEXT("None");
					}
					else
					{
						PropertyClassName = PropertyBeingCustomized->GetClass()->GetName();
					}

					ControlPanelHeaderText = FText::FromString(FString::Printf(TEXT("%s (%s)"), *ControlPanelHeaderText.ToString(), *PropertyClassName));
				}		
				

				IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Variable");

				Category.AddCustomRow(ControlPanelRowText, true)
					.NameContent()
					[
						SNew(STextBlock)
						.ToolTip(FSlateApplicationBase::Get().MakeToolTip(ControlPanelHeaderTooltipText))
						.Text(ControlPanelHeaderText)
						.Font(IDetailLayoutBuilder::GetDetailFontBold())						
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
			
				

				
            	for(const FBPVariableMetaDataEntry& MetaDataEntry : BlueprintPtr->NewVariables[VariableIndex].MetaDataArray)
            	{
					const FName MetaDataKey = MetaDataEntry.DataKey;

					bool bIsRestricted = Settings->IsRestricted(MetaDataKey);					
					if (bIsRestricted && !Settings->bShowRestrictedOptions)
					{
						continue;
					}
					bool bIsKeyReadOnly = Settings->bDisableCustomOption || Settings->MetaDataOptions.Contains(MetaDataKey) || Settings->DefaultOptions.Contains(MetaDataKey);


					// Metadata description text
					const FText TooltipText = FText::FromString(Settings->GetOption(MetaDataKey).Description);

					TSharedPtr<SWidget> KeyWidget;
					if (bIsRestricted || bIsKeyReadOnly)
					{
						KeyWidget = SNew(STextBlock)
							.Font(IDetailLayoutBuilder::GetDetailFont())
							.Text(FText::FromName(MetaDataKey))
							.ToolTipText(TooltipText);
					}
					else 
					{
						KeyWidget = SNew(SEditableTextBox)							
							.Font(IDetailLayoutBuilder::GetDetailFont())
							.Text(FText::FromName(MetaDataKey))
							.ToolTipText(TooltipText)
							.OnVerifyTextChanged(FOnVerifyTextChanged::CreateSP(this, &FBlueprintVariableMetadataCustomization::VerifyMetaKey))
							.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FBlueprintVariableMetadataCustomization::MetaKeyChanged, MetaDataKey))
							.IsReadOnly(bIsRestricted || bIsKeyReadOnly)
							.SelectAllTextWhenFocused(true)
							.ClearKeyboardFocusOnCommit(false)
							.SelectAllTextOnCommit(true);
					}
            		
					Category.AddCustomRow(MetadataRowText, true)
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
								SNew(SMultiLineEditableTextBox)
								.Text(FText::FromString(MetaDataEntry.DataValue))
								.Font(IDetailLayoutBuilder::GetDetailFont())
								.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FBlueprintVariableMetadataCustomization::MetaValueChanged, MetaDataKey))
								.IsReadOnly(bIsRestricted)
								.ModiferKeyForNewLine(EModifierKey::Shift)
								.SelectAllTextWhenFocused(false)
								.ClearKeyboardFocusOnCommit(false)								
								.SelectAllTextOnCommit(false)
								.WrapTextAt(200.0f)
							]
							+SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								PropertyCustomizationHelpers::MakeRemoveButton(
									FSimpleDelegate::CreateSP(this, &FBlueprintVariableMetadataCustomization::RemoveMetadata, MetaDataKey),
									FText::GetEmpty(), 
									!bIsRestricted)
							]
						];
            	}
            }
		}
	}
}

TSharedRef<SWidget> FBlueprintVariableMetadataCustomization::GetAddMetaDataDropdown()
{
	const FText MetadataAddTooltipText = LOCTEXT("MetadataAdd_Tooltip", "Adds new meta data");
	const FText NoMetaDataToAdd = LOCTEXT("MetadataAdd_NoOptions", "Nothing to add");

	FMenuBuilder MenuBuilder(true, nullptr);

	const UBlueprintVariableMetadataSettings* Settings = GetDefault<UBlueprintVariableMetadataSettings>();

	bool bIsEmpty = true;
	if (!Settings->bDisableCustomOption)
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


	TMap<FName, FMetaDataOption> AllOptions = Settings->MetaDataOptions;
	for (const auto& OptionPair : Settings->DefaultOptions)
	{
		if (!AllOptions.Contains(OptionPair.Key))
		{
			AllOptions.Add(OptionPair.Key, OptionPair.Value);
		}
	}

	for (const auto& OptionPair : AllOptions)
	{
		const FName Option = OptionPair.Key;
		const FMetaDataOption& OptionSettings = OptionPair.Value;

		if (Option.IsNone())
		{
			continue;
		}

		if (!OptionSettings.bEnabled)
		{
			continue;
		}

		if (!IsPropertyAllowed(OptionSettings.RestrictToTypes))
		{
			continue;
		}

		bool bAlreadyExists = false;
		{
			FString Value;
			bAlreadyExists = FBlueprintEditorUtils::GetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, Option, Value);
		}
		if (bAlreadyExists && !Settings->bShowAddedOptions)
		{
			continue;
		}

		bIsEmpty = false;
		MenuBuilder.AddMenuEntry(
			FText::FromName(Option),
			FText::FromString(OptionSettings.Description),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateSP(this, &FBlueprintVariableMetadataCustomization::AddMetadata, Option),
				FCanExecuteAction::CreateLambda([bAlreadyExists]() { return !bAlreadyExists; }),
				FIsActionChecked(),
				FIsActionButtonVisible()
			)
		);
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


bool FBlueprintVariableMetadataCustomization::IsPropertyAllowed(const FString& Filter) const
{
	if (Filter.IsEmpty())
	{
		return true;
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


	FProperty* TargetProperty = PropertyBeingCustomized.Get();
	if (!TargetProperty)
	{
		return false;
	}

	auto CheckProperty = [&AllowedClasses](FProperty* Property)
	{
		if (Property)
		{
			if (AllowedClasses.Contains(Property->GetClass()->GetFName()))
			{
				return true;
			}
			else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				if (StructProperty->Struct && AllowedClasses.Contains(StructProperty->Struct->GetFName()))
				{
					return true;
				}
			}
		}		
		return false;
	};

	if (CheckProperty(TargetProperty))
	{
		return true;
	}
	else if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(TargetProperty))
	{		
		if (CheckProperty(ArrayProperty->Inner))
		{
			return true;
		}
	}
	else if (FSetProperty* SetProperty = CastField<FSetProperty>(TargetProperty))
	{
		if (CheckProperty(SetProperty->ElementProp))
		{
			return true;
		}
	}
	else if (FMapProperty* MapProperty = CastField<FMapProperty>(TargetProperty))
	{
		if (CheckProperty(MapProperty->KeyProp))
		{
			return true;
		}

		if (CheckProperty(MapProperty->ValueProp))
		{
			return true;
		}
	}

	return false;
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

	const FMetaDataOption* MetaOption = GetDefault<UBlueprintVariableMetadataSettings>()->MetaDataOptions.Find(MetaDataKey);

	FBlueprintEditorUtils::SetBlueprintVariableMetaData(BlueprintPtr.Get(), VariableName, nullptr, MetaDataKey, MetaOption ? MetaOption->DefaultValue : TEXT(""));
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

#undef LOCTEXT_NAMESPACE
