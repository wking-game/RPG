// Dedicated-Server-Target. Braucht die Source-Engine (E:\UE58), der Launcher-Build kann keine Server-Targets bauen.

using UnrealBuildTool;
using System.Collections.Generic;

public class GamerServerTarget : TargetRules
{
	public GamerServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		ExtraModuleNames.Add("Gamer");
	}
}
