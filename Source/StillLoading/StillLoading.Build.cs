// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StillLoading : ModuleRules
{
	public StillLoading(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"UMG",
			"AIModule",
			"MotionWarping",
			"AnimGraphRuntime",
			"GameplayTags",
			"Niagara",
            "GeometryCollectionEngine",
            "GameplayAbilities"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PrivateIncludePaths.Add("StillLoading");
	}
}
