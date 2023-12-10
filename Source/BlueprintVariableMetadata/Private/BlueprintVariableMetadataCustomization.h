// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/WeakFieldPtr.h"

class IDetailLayoutBuilder;
class IBlueprintEditor;
class UBlueprint;

class FBlueprintVariableMetadataCustomization : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedPtr<IDetailCustomization> MakeInstance(TSharedPtr<IBlueprintEditor> InBlueprintEditor);

	FBlueprintVariableMetadataCustomization(TSharedPtr<IBlueprintEditor> InBlueprintEditor, UBlueprint* Blueprint);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;


private:
	TSharedRef<SWidget> GetAddMetaDataDropdown();	

	void AddMetadata(FName MetaDataKey);
	void RemoveMetadata(FName MetaDataKey);

	bool VerifyMetaKey(const FText& NewKeyName, FText& ErrorMessage);
	void MetaKeyChanged(const FText& NewKeyName, ETextCommit::Type ChangeType, FName OldKeyName);
	void MetaValueChanged(const FText& NewValueName, ETextCommit::Type ChangeType, FName KeyName);

private:
	static TArray<FName> GetPropertyInfo(const FProperty* Property);
	static bool GetPropertyTypeName(const FProperty* Property, FName& PropertyType);

	static bool IsPropertyAllowed(const FProperty* TargetProperty, const FString& Filter);

private:
	TWeakPtr<IBlueprintEditor> BlueprintEditorPtr;
	TWeakObjectPtr<UBlueprint> BlueprintPtr;
	TWeakFieldPtr<FProperty> PropertyBeingCustomized;

	FName VariableName;
};
