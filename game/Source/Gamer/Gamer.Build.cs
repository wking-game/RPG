// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Gamer : ModuleRules
{
	public Gamer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		// AIModule + NavigationSystem: villager pathfinding (docs/01_entscheidungen.md, Wegfindung).
		PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "NavigationSystem" });

		// Json: game data tables in Content/Data/*.json (docs/06_datenformat.md).
		PrivateDependencyModuleNames.AddRange(new string[] { "Json" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
