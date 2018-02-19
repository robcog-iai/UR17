// Some copyright should be here...

using UnrealBuildTool;

public class UGame : ModuleRules
{
	public UGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UTags" });

        /**
        PublicIncludePaths.AddRange(
			new string[] {
				"UGame/Public"
				
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"UGame/Private",
				
				// ... add other private include paths required here ...
			}
			);		
		*/

	}
}
