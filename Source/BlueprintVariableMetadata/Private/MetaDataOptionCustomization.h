// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <IPropertyTypeCustomization.h>


class FMetaDataOptionCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{ 
		return MakeShareable(new FMetaDataOptionCustomization); 
	}


	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

};
