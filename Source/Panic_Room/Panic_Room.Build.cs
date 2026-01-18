// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Panic_Room : ModuleRules
{
    public Panic_Room(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        { 
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "OnlineSubsystem","OnlineSubsystemSteam", "OnlineSubsystemUtils",
            "UMG", "Slate", "SlateCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
