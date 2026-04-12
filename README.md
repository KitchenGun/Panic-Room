# Panic Room

Unreal Engine 5.5 기반 멀티플레이어 FPS 게임 프로젝트입니다.

## 개요

**Panic Room**은 Steam 온라인 세션을 활용한 멀티플레이어 1인칭 슈팅 게임입니다. Gameplay Ability System(GAS)을 기반으로 전투, 체력, 사망/리스폰 시스템이 구현되어 있습니다.

## 기술 스택

- **엔진**: Unreal Engine 5.5
- **언어**: C++, Blueprint
- **네트워크**: Steam Online Subsystem (Advanced Sessions)
- **핵심 플러그인**:
  - Gameplay Ability System (GAS)
  - Enhanced Input System
  - Online Subsystem Steam
  - Advanced Sessions / Advanced Steam Sessions

## 프로젝트 구조

```
Source/Panic_Room/
├── Ability/            # 게임플레이 어빌리티 (GA_Basic, GA_Pistol, GA_Death, DA_StartUpDataBase)
├── ActorComponent/     # 액터 컴포넌트 (CombatComponent, WeaponComponent, PickUpComponent)
├── Attribute/          # GAS 어트리뷰트 (Health, MaxHealth, Damage)
├── Character/          # 캐릭터 및 플레이어 스테이트 (Steam 연동 포함)
├── Framework/          # 게임모드, 게임스테이트, 게임인스턴스, 입력 시스템
└── Weapon/             # 무기 시스템 (BasicWeapon - 소켓 부착, 라인트레이스 사격)
```

## 주요 시스템

### 전투 시스템
- GAS 기반 어빌리티 활성화 (입력 트리거, 이벤트 반응, 스폰 시 자동 활성화)
- `CombatComponent`를 통한 무기 장착/관리 (GameplayTag 기반 무기 맵)
- 라인트레이스 기반 사격 및 GameplayEffect를 통한 데미지 적용

### 캐릭터 생명주기
- 체력이 0에 도달하면 `GA_Death` 어빌리티가 이벤트로 트리거
- 서버 권한(Server-Only)으로 사망 처리
- 설정 가능한 딜레이 후 리스폰 및 어트리뷰트 초기화

### 멀티플레이
- `PlayerState`가 ASC와 어트리뷰트를 소유하여 네트워크 리플리케이션 보장
- Steam 플레이어 이름 및 아바타 비동기 로딩
- `GameInstance`를 통한 로비 호스팅 및 세션 관리

### 입력 시스템
- Enhanced Input 기반 데이터 주도 입력 설정 (`DA_InputConfig`)
- 네이티브 입력(이동, 시점, 점프)과 어빌리티 입력(사격) 분리 바인딩

## 빌드 및 실행

### 요구사항
- Unreal Engine 5.5
- Visual Studio 2022
- Steam 클라이언트

### 빌드
1. `Panic_Room.uproject`를 우클릭하여 **Generate Visual Studio project files** 선택
2. `Panic_Room.sln`을 Visual Studio에서 열기
3. `Development Editor` 구성으로 빌드

### 실행
- 에디터에서 **Standalone Game** 또는 **멀티플레이어 테스트**로 실행
- Steam App ID: `480` (Spacewar 테스트용)
