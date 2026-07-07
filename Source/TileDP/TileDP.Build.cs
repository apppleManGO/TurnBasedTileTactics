// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TileDP : ModuleRules
{
	public TileDP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
