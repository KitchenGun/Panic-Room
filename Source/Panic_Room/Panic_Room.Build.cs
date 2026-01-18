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

        // Source 폴더 내의 모든 하위 폴더를 인클루드 경로로 잡게 함
        PublicIncludePaths.AddRange(
        new string[] {
            "Panic_Room",           // 기본 모듈 경로
            "Panic_Room/Character", // Character 폴더를 기본 include 경로로 추가
            "Panic_Room/ActorComponent",
            "Panic_Room/Framework"
        }
    );
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
