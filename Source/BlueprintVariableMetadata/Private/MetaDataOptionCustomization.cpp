// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#include "MetaDataOptionCustomization.h"
#include "BlueprintVariableMetadataSettings.h"

#include <IDetailChildrenBuilder.h>

#define LOCTEXT_NAMESPACE "MetaDataOptionCustomization"


void FMetaDataOptionCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> EnabledHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, bEnabled));
	TSharedPtr<IPropertyHandle> DefaultValueHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, DefaultValue));
	TSharedPtr<IPropertyHandle> RestrictToTypes = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, RestrictToTypes));

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 0, 0)
		[
			EnabledHandle->CreatePropertyNameWidget()
		]
		+ SHorizontalBox::Slot().AutoWidth().Padding(2.0f, 0, 5.0f, 0)
		[
			EnabledHandle->CreatePropertyValueWidget()
		]
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
	ChildBuilder.AddProperty(PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FMetaDataOption, Description)).ToSharedRef());
}

#undef LOCTEXT_NAMESPACE