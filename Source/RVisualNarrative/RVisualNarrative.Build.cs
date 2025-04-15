// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RVisualNarrative : ModuleRules
{
	public RVisualNarrative(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule",
				"RVisualNarrativeCore",
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"RVisualNarrativeCrossVersion"
				// ... add private dependencies that you statically link with here ...	
			}
		);
	}
}