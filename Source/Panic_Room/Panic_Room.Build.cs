// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class Panic_Room : ModuleRules
{
    public Panic_Room(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils",  // 온라인 서브시스템 Steam 의존
            "AdvancedSessions",
            "UMG", "Slate", "SlateCore",        // UI 관련 의존
            "GameplayAbilities", "GameplayTags", "GameplayTasks",    // GAS 관련 의존

        });

        PublicIncludePaths.AddRange(new string[]
        {
            "Panic_Room",                   // 기본 모듈 경로
            "Panic_Room/Character",         // 캐릭터 관련 기본 include 경로로 추가
            "Panic_Room/ActorComponent",
            "Panic_Room/Framework"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Steam SDK 헤더 및 라이브러리 경로 (아바타 직접 접근용)
        string SteamworksRoot = Path.Combine(EngineDirectory,
            "Source", "ThirdParty", "Steamworks");
        if(Directory.Exists(SteamworksRoot))
        {
            string[] SteamDirs = Directory.GetDirectories(SteamworksRoot, "Steamv*");
            if(SteamDirs.Length > 0)
            {
                string SteamSDK = SteamDirs[0];
                // 헤더 경로
                PublicIncludePaths.Add(Path.Combine(SteamSDK, "sdk", "public"));
                // 링커용 라이브러리 (steam_api64.lib)
                PublicAdditionalLibraries.Add(
                    Path.Combine(SteamSDK, "sdk", "redistributable_bin", "win64", "steam_api64.lib")
                );
            }
        }
    }
}
