// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#include "BlueprintVariableMetadataModule.h"
#include "Modules/ModuleManager.h"
#include "BlueprintEditorModule.h"

#include "BlueprintVariableMetadataSettings.h"

#include "BlueprintVariableMetadataCustomization.h"
#include "MetaDataOptionCustomization.h"


DEFINE_LOG_CATEGORY(LogBlueprintVariableMetadata);


#define LOCTEXT_NAMESPACE "BlueprintVariableMetadata"

class FBlueprintVariableMetadataModule : public IBlueprintVariableMetadataModule
{
public:

    virtual void StartupModule() override
    {
        FBlueprintEditorModule& BlueprintEditorModule = FModuleManager::LoadModuleChecked<FBlueprintEditorModule>("Kismet");
        RegisteredVariableCustomization = BlueprintEditorModule.RegisterVariableCustomization(FProperty::StaticClass(), FOnGetVariableCustomizationInstance::CreateStatic(&FBlueprintVariableMetadataCustomization::MakeInstance));
   
        FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
        PropertyModule.RegisterCustomPropertyTypeLayout(FMetaDataOption::StaticStruct()->GetFName(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMetaDataOptionCustomization::MakeInstance));
    }

    virtual void ShutdownModule() override
    {
        FBlueprintEditorModule* BlueprintEditorModule = FModuleManager::GetModulePtr<FBlueprintEditorModule>("Kismet");
        if (BlueprintEditorModule)
        {
            BlueprintEditorModule->UnregisterVariableCustomization(FProperty::StaticClass(), RegisteredVariableCustomization);
        }

        if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
        {
            PropertyModule->UnregisterCustomPropertyTypeLayout(FMetaDataOption::StaticStruct()->GetFName());
        }
    }

private:
    FDelegateHandle RegisteredVariableCustomization;
};


IMPLEMENT_MODULE(FBlueprintVariableMetadataModule, BlueprintVariableMetadata);

#undef LOCTEXT_NAMESPACE
