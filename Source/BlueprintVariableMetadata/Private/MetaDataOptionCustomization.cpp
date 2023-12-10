// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#include "MetaDataOptionCustomization.h"
#include "BlueprintVariableMetadataSettings.h"

#include <IDetailChildrenBuilder.h>

#define LOCTEXT_NAMESPACE "MetaDataOptionCustomization"


void FMetaDataOptionCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> EnabledHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, bEnabled));
	TSharedPtr<IPropertyHandle> KeyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, Key));
	TSharedPtr<IPropertyHandle> DefaultValueHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, DefaultValue));
	TSharedPtr<IPropertyHandle> CategoryHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, Category));
	TSharedPtr<IPropertyHandle> GroupHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, Group));
	TSharedPtr<IPropertyHandle> RestrictToTypes = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, RestrictToTypes));

	EnabledHandle->MarkHiddenByCustomization();
	KeyHandle->MarkHiddenByCustomization();
	DefaultValueHandle->MarkHiddenByCustomization();
	CategoryHandle->MarkHiddenByCustomization();
	GroupHandle->MarkHiddenByCustomization();
	RestrictToTypes->MarkHiddenByCustomization();


	HeaderRow.NameContent()
	[		
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 0, 0)
		[
			EnabledHandle->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 10.0f, 0)
		[
			EnabledHandle->CreatePropertyValueWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			SNew(SBox).MinDesiredWidth(200)
			[
				KeyHandle->CreatePropertyValueWidget()
			]			
		]
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth()
		[
			DefaultValueHandle->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			SNew(SBox).MinDesiredWidth(100)
			[
				DefaultValueHandle->CreatePropertyValueWidget()
			]			
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			CategoryHandle->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			SNew(SBox).MinDesiredWidth(100)
			[
				CategoryHandle->CreatePropertyValueWidget()
			]
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			SNew(SBox).MinDesiredWidth(100)
			[
				GroupHandle->CreatePropertyValueWidget()
			]
		]
		+ SHorizontalBox::Slot().AutoWidth()
		[
			RestrictToTypes->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			SNew(SBox).MinDesiredWidth(200)
			[
				RestrictToTypes->CreatePropertyValueWidget()
			]			
		]
	];
}

void FMetaDataOptionCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	uint32 NumChidlren = 0;
	PropertyHandle->GetNumChildren(NumChidlren);
	for (uint32 Index = 0; Index < NumChidlren; Index++)
	{
		TSharedPtr<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle(Index);
		if (!ChildHandle->IsCustomized())
		{
			ChildBuilder.AddProperty(ChildHandle.ToSharedRef());
		}
	}
}

#undef LOCTEXT_NAMESPACE