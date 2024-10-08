// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FeatureWarehouse : ModuleRules
{
	public FeatureWarehouse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "UMG", "AnimGraphRuntime" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PrivateIncludePaths.Add("FeatureWarehouse");
		PrivateIncludePaths.Add("FeatureWarehouse/Items");
		PrivateIncludePaths.Add("FeatureWarehouse/Items/Weapons");

        // 전처리기 매크로 정의
        PublicDefinitions.Add("GAME_TESTING=1");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
