// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Schwerpunkt : ModuleRules
{
	public Schwerpunkt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "OnlineSubsystemSteam", "OnlineSubsystem", "UMG", "SlateCore" });
	}
}
