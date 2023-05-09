// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HideAndSeek : ModuleRules
{
	public HideAndSeek(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.AddRange(new string[]{"HideAndSeek/Private/Protobuf"});
		PublicIncludePaths.AddRange(new string[]{"HideAndSeek/Public/Protobuf"});
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","Json","JsonUtilities","HTTP" });
		PrivateDependencyModuleNames.AddRange(new string[] { "libprotobuf" });
	}
}
