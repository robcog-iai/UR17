// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class URealisticGraspingEditor : ModuleRules
{
	public URealisticGraspingEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"URealisticGrasping/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"URealisticGrasping/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "URealisticGrasping",
                "CoreUObject",
                "SkeletalMeshEditor",
                "SkeletonEditor",
                "StaticMeshEditor",
                "Engine",
                "InputCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
            "Projects",
            "URealisticGrasping",
            "InputCore",
            "UnrealEd",
            "LevelEditor",
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "Persona",
            "HeadMountedDisplay",
            "LevelEditor",
            "SteamVR"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
