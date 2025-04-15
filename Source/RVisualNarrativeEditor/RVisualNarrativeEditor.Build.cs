using UnrealBuildTool;

public class RVisualNarrativeEditor : ModuleRules
{
	public RVisualNarrativeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RVisualNarrative",
				"RVisualNarrativeCore",

				"Slate",
				"SlateCore",
				"Settings",
				"UnrealEd",
				"AIGraph",
				"Kismet",
				"EditorStyle",
				"GraphEditor",
				"BlueprintGraph",
				"PropertyEditor",
				"ToolMenus",
				"InputCore",
				"ApplicationCore",
				"AssetTools",
				"RVisualNarrativeCrossVersion",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects"
			}
		);
	}
}