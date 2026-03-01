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
            "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils",  // 온라인 서브시스템 관련 모듈
            "AdvancedSessions",
            "UMG", "Slate", "SlateCore",        // UI 관련 모듈
            "GameplayAbilities", "GameplayTags", "GameplayTasks"    // GAS 관련 모듈
        });

        PublicIncludePaths.AddRange(new string[]
        {
            "Panic_Room",                   // 기본 모듈 경로
            "Panic_Room/Character",         // 캐릭터 폴더를 기본 include 경로로 추가
            "Panic_Room/ActorComponent",
            "Panic_Room/Framework"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
