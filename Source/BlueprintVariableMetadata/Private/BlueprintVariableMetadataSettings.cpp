// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.


#include "BlueprintVariableMetadataSettings.h"



UBlueprintVariableMetadataSettings::UBlueprintVariableMetadataSettings()
{
	SystemRestrictedOptions = TSet<FName>(
	{
		TEXT("Tooltip"), 
		TEXT("ExposeOnSpawn"), 
		TEXT("MakeEditWidget"), 
		TEXT("BlueprintPrivate"), 
		TEXT("Bitmask"),
		TEXT("BitmaskEnum"), 
		TEXT("DeprecationMessage")
	});

	DefaultOptions.Reset();


	//General utility
	AddDefaultMeta(TEXT("DisplayAfter"), true,  TEXT("PropName"),  
		TEXT(""), 
		TEXT("Indicates that the property should be displayed immediately after the property named in the metadata."));
	AddDefaultMeta(TEXT("DisplayPriority"), true,  TEXT("0"),  
		TEXT(""), 
		TEXT("The relative order within its category that the property should be displayed in where lower values are sorted first..\nIf used in conjunction with DisplayAfter,  specifies the priority relative to other properties with same DisplayAfter specifier."));
	AddDefaultMeta(TEXT("DisplayThumbnail"), true,  TEXT("true"),  
		TEXT(""), 
		TEXT("Indicates that the property is an asset type and it should display the thumbnail of the selected asset."));
	AddDefaultMeta(TEXT("EditCondition"), true,  TEXT(""),  
		TEXT(""), 
		TEXT("Specifies a boolean property that is used to indicate whether editing of this property is disabled."));
	AddDefaultMeta(TEXT("EditConditionHides"), true,  TEXT(""),  
		TEXT(""), 
		TEXT("Specifies a boolean property that is used to indicate whether this property should be hidden when the EditCondition evaluates to false.\nRequires an EditCondition to be set."));
	AddDefaultMeta(TEXT("InlineEditConditionToggle"),  true,  TEXT(""),  
		TEXT("BoolProperty"), 
		TEXT("Signifies that the bool property is only displayed inline as an edit condition toggle in other properties, and should not be shown on its own row."));
	AddDefaultMeta(TEXT("NoResetToDefault"), true,  TEXT(""),  
		TEXT(""), 
		TEXT("Property wont have a 'reset to default' button when displayed in property windows"));
	AddDefaultMeta(TEXT("ShowOnlyInnerProperties"), true,  TEXT(""),  
		TEXT(""), 
		TEXT("Used by struct properties. Indicates that the inner properties will not be shown inside an expandable struct, but promoted up a level."));
	
		
	//Numeric		
	AddDefaultMeta(TEXT("AllowPreserveRatio"), true,  TEXT(""),  
		TEXT("Vector"), 
		TEXT("Used for FVector properties.\nIt causes a ratio lock to be added when displaying this property in details panels."));
	AddDefaultMeta(TEXT("ArrayClamp"), true,  TEXT("ArrayName"),  
		TEXT("IntProperty, Int64Property"), 
		TEXT("Used for integer properties.\nClamps the valid values that can be entered in the UI to be between 0 and the length of the array specified."));
	AddDefaultMeta(TEXT("Multiple"), true,  TEXT(""),  
		TEXT("ByteProperty, DoubleProperty, IntProperty, int64Property"), 
		TEXT("Used for numeric properties.\nStipulates that the value must be a multiple of the metadata value."));
	AddDefaultMeta(TEXT("Delta"), true,  TEXT(""),  
		TEXT("ByteProperty, DoubleProperty, IntProperty, int64Property"), 
		TEXT("Used for float and integer properties.\nSpecifies the step size when the slider is used (e.g. for Delta = 10,  the value will scroll among steps of 10)."));
	AddDefaultMeta(TEXT("NoSpinbox"), true,  TEXT(""),  
		TEXT("ByteProperty, DoubleProperty, IntProperty, int64Property"), 
		TEXT("Used for integer and float properties.\nIndicates that the spin box element of the number editing widget should not be displayed."));
	AddDefaultMeta(TEXT("SliderExponent"), true,  TEXT(""),  
		TEXT("ByteProperty, DoubleProperty, IntProperty, int64Property"), 
		TEXT("Used by numeric properties.\nIndicates how rapidly the value will grow when moving an unbounded slider."));
	
		

		
	//Containers		
	AddDefaultMeta(TEXT("EditFixedOrder"), true,  TEXT(""),  
		TEXT("ArrayProperty"), 
		TEXT("Keeps the elements of an array from being reordered by dragging "));
	AddDefaultMeta(TEXT("NoElementDuplicate"), true,  TEXT(""),  
		TEXT("ArrayProperty"), 
		TEXT("Used for array properties. Indicates that the duplicate icon should not be shown for entries of this array in the property panel."));
	AddDefaultMeta(TEXT("TitleProperty"), true,  TEXT("FuncName"),  
		TEXT("ArrayProperty, SetProperty, MapProperty"), 
		TEXT("Used by arrays of structs.\nIndicates a single property inside of the struct that should be used as a title summary when the array entry is collapsed."));
	
		

		
	// Strings		
	AddDefaultMeta(TEXT("MaxLength"), true,  TEXT(""),  
		TEXT("NameProperty, StrProperty, TextProperty"), 
		TEXT("Used for FString and FText properties.\nIndicates the maximum length of the value that can be edited,  or <=0 for unlimited."));
	AddDefaultMeta(TEXT("PasswordField"), true,  TEXT(""),  
		TEXT("NameProperty, StrProperty, TextProperty"), 
		TEXT("Used for FString and FText properties.\nIndicates that the edit field is a secret field (e.g a password) and entered text will be replaced with dots. Do not use this as your only security measure.  The property data is still stored as plain text. "));
	AddDefaultMeta(TEXT("GetOptions"), true,  TEXT(""), 
		TEXT("NameProperty, StrProperty"), 
		TEXT("Causes FString and FName properties to have a limited set of options generated dynamically, e.g. meta=(GetOptions=\"FuncName\").\nSupports external static function references via \"Module.Class.Function\" syntax.\n\nUFUNCTION()\nTArray<FString> FuncName() const; // Always return string array even if FName property."));


	//Class and objects utility
	AddDefaultMeta(TEXT("AllowAbstract"), true,  TEXT(""), 
		TEXT("ClassProperty, SoftClassPath"), 
		TEXT("Used for Subclass and SoftClass properties.\nIndicates whether abstract class types should be shown in the class picker"));
	AddDefaultMeta(TEXT("AllowedClasses"), true,  TEXT(""),  
		TEXT("ClassProperty, ObjectProperty, SoftObjectPath, SoftClassPath, PrimaryAssetId"), 
		TEXT("Used for FSoftObjectPath,  ComponentReference and UClass properties.\nComma delimited list that indicates the class type(s) of assets to be displayed in the asset picker(FSoftObjectPath) or component picker or class viewer (UClass)."));
	AddDefaultMeta(TEXT("DisallowedClasses"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath, ObjectProperty, SoftObjectPath, PrimaryAssetId"), 
		TEXT("Used for FSoftObjectPath,  ActorComponentReference and UClass properties.\nComma delimited list that indicates the class type(s) of assets that will NOT be displayed in the asset picker (FSoftObjectPath) or component picker or class viewer (UClass)."));
	AddDefaultMeta(TEXT("ExactClass"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath, ObjectProperty, SoftObjectPath"), 
		TEXT("Used for FSoftObjectPath properties in conjunction with AllowedClasses.\nIndicates whether only the exact classes specified in AllowedClasses can be used or whether subclasses are valid."));
	AddDefaultMeta(TEXT("MetaClass"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath"), 
		TEXT("Used FSoftClassPath properties.\nIndicates the parent class that the class picker will use when filtering which classes to display."));
	AddDefaultMeta(TEXT("MustImplement"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath"), 
		TEXT("Used for Subclass and SoftClass properties.\nIndicates the selected class must implement a specific interface"));

		
	AddDefaultMeta(TEXT("HideViewOptions"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath"), 
		TEXT("Used for Subclass and SoftClass properties.\nSpecifies to hide the ability to change view options in the class picker"));
	AddDefaultMeta(TEXT("ShowTreeView"), true,  TEXT(""),  
		TEXT("ClassProperty, SoftClassPath"), 
		TEXT("Used for Subclass and SoftClass properties.\nShows the picker as a tree view instead of as a list"));
	
		
	AddDefaultMeta(TEXT("RequiredAssetDataTags"), true,  TEXT(""),  
		TEXT("ObjectProperty, SoftObjectPath"), 
		TEXT("Used by asset properties.\nFilters out asset that do not have matching data registry tags, RowStructure=NameOfDataTableRow could be used to restrict data tables."));
	AddDefaultMeta(TEXT("DisallowedAssetDataTags"), true,  TEXT(""),  
		TEXT("ObjectProperty, SoftObjectPath"), 
		TEXT("Used by asset properties.\nFilters out assets containing the specified asset registry tags."));
	
	AddDefaultMeta(TEXT("AllowedTypes"), true,  TEXT("AssetType"),  
		TEXT("PrimaryAssetId"), 
		TEXT("Comma seperated PrimaryAssetType list.\nLimits available to select PrimaryAssetId"));

// 	AddDefaultMeta(TEXT("ForceShowEngineContent"), true,  TEXT(""),  
// 		TEXT("ObjectProperty, SoftObjectPath"), 
// 		TEXT("Used by asset properties.\nIndicates that the asset pickers should always show engine content"));
// 	AddDefaultMeta(TEXT("ForceShowPluginContent"), true,  TEXT(""),  
// 		TEXT("ObjectProperty, SoftObjectPath"), 
// 		TEXT("Used by asset properties.\nIndicates that the asset pickers should always show plugin content"));
	
		

		
	// Directories		
	AddDefaultMeta(TEXT("FilePathFilter"), true,  TEXT(""),  
		TEXT("FilePath"), 
		TEXT("Used by FFilePath properties. Indicates the path filter to display in the file picker."));
	AddDefaultMeta(TEXT("RelativeToGameDir"), true,  TEXT(""),  
		TEXT("FilePath"), 
		TEXT(" Used by FFilePath properties. Indicates that the FilePicker dialog will output a path relative to the game directory when setting the property. An absolute path will be used when outside the game directory."));
	AddDefaultMeta(TEXT("ContentDir"), true,  TEXT(""),  
		TEXT("DirectoryPath"), 
		TEXT("Used by FDirectoryPath properties. Indicates that the path will be picked using the Slate-style directory picker inside the game Content dir."));
	AddDefaultMeta(TEXT("LongPackageName"), true,  TEXT(""),  
		TEXT("DirectoryPath"), 
		TEXT("Used by FDirectoryPath properties.  Converts the path to a long package name"));
	AddDefaultMeta(TEXT("RelativePath"), true,  TEXT(""),  
		TEXT("DirectoryPath"), 
		TEXT("Used by FDirectoryPath properties. Indicates that the directory dialog will output a relative path when setting the property."));
	AddDefaultMeta(TEXT("RelativeToGameContentDir"), true,  TEXT(""),  
		TEXT("DirectoryPath"), 
		TEXT("Used by FDirectoryPath properties. Indicates that the directory dialog will output a path relative to the game content directory when setting the property."));
	
		

		
	// Misc		
	AddDefaultMeta(TEXT("HideAlphaChannel"), true,  TEXT(""),  
		TEXT("Color, LinearColor"), 
		TEXT("Used for FColor and FLinearColor properties. Indicates that the Alpha property should be hidden when displaying the property widget in the details."));
	AddDefaultMeta(TEXT("Categories"), true,  TEXT("TagName"),  
		TEXT("GameplayTag, GameplayTagContainer"), 
		TEXT("Limit tag selection to specified tag categories"));

	
}

void UBlueprintVariableMetadataSettings::PostInitProperties()
{
	Super::PostInitProperties();

	DefaultOptions.KeyStableSort(FNameLexicalLess());
	MetaDataOptions.KeyStableSort(FNameLexicalLess());
}

#if WITH_EDITOR
void UBlueprintVariableMetadataSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

void UBlueprintVariableMetadataSettings::AddDefaultMeta(FName Key, bool bEnabled /*= true*/, FString DefaultValue /*= TEXT("")*/, FString RestrictToTypes /*= TEXT("")*/, FString Description /*= TEXT("")*/)
{
	DefaultOptions.Add(Key, FMetaDataOption(bEnabled, DefaultValue, RestrictToTypes, Description));
}

FMetaDataOption UBlueprintVariableMetadataSettings::GetOption(FName Key) const
{
	const FMetaDataOption* Option = MetaDataOptions.Find(Key);
	if (Option == nullptr)
	{
		Option = DefaultOptions.Find(Key);
	}
	return Option ? *Option : FMetaDataOption();
}

bool UBlueprintVariableMetadataSettings::IsRestricted(FName Key) const
{
	return SystemRestrictedOptions.Contains(Key) || UserRestrictedOptions.Contains(Key);
}
