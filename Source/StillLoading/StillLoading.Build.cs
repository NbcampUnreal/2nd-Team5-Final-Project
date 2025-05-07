// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StillLoading : ModuleRules
{
	public StillLoading(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateIncludePaths.Add("StillLoading");
	}
}
