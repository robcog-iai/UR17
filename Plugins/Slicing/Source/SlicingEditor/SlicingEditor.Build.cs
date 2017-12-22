// Copyright 2017, Institute for Artificial Intelligence

using UnrealBuildTool;

public class SlicingEditor: ModuleRules
{
	public SlicingEditor(ReadOnlyTargetRules Target): base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                "SlicingEditor/Public"
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"SlicingEditor/Private",
				// ... add other private include paths required here ...
			}
		);

        // Public dependencies that are statically linked
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "StaticMeshEditor",
                "SlicingLogic"
			}
		);

        // Private dependencies that are statically linked
        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
			}
		);
		
		// Dependencies that are dynamically loaded
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
