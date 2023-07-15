// Copyright (C) Vasily Bulgakov. 2023. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class BlueprintVariableMetadata : ModuleRules
{
	public BlueprintVariableMetadata(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
	        new string[]
	        {
	        	"Core",
                "CoreUObject",
                "Slate",
                "SlateCore",

                "DeveloperSettings",

                "UnrealEd",
                "Kismet",
                "PropertyEditor",
	        }
        );
	}
}
