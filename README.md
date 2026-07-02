# TurnBasedTileTactics

Unreal Engine 5.4 · C++ 기반 턴제 타일 전략 게임 (좀비 디펜스)

## 소개
플레이어와 몬스터가 번갈아 턴을 진행하며, 타일 단위로 이동/공격하는 턴제 전략 게임입니다.
Blueprint 로직에 의존하지 않고 게임플레이 핵심 로직(이동, 전투, 턴 관리, AI)을 C++로 직접 구현했습니다.

## 플레이 영상
> (여기에 GIF 또는 유튜브 링크 삽입 예정)

## 조작
- 이동: 상/하/좌/우 인접 타일로 이동 (타일 하이라이트 클릭 또는 방향 입력)
- 공격: 공격 모드 진입 후 방향 타일 선택 → 해당 방향에 몬스터가 있으면 공격

## 주요 구현 포인트
- **턴 상태 관리**: `AMyGameModeBase`가 `EGameTurn`(PlayerTurn/MonsterTurn) enum으로 턴 흐름을 중앙에서 관리
- **타일 판정**: 플레이어 위치 기준 상하좌우 방향 벡터로 이동/공격 가능 타일을 계산하고, `SphereOverlapActors`로 점유 여부 판정
- **좀비 AI**: `AZombieAIController`가 플레이어 방향으로 이동, 인접 시 공격 로직 수행
- **애니메이션 연동**: `UMyPlayer_anim`, `UZombie_Anim`(AnimInstance)으로 이동/공격/피격 상태를 애니메이션 블루프린트에 전달
- **UI 연동**: `UEnd_UserWidget`으로 게임 종료(승/패) 화면 처리

## 트러블슈팅
- **`-Wshadow` 빌드 실패**: `AMyPlayer::ShowMovableTiles()` / `ShowAttackTiles()` 내부 루프 지역변수 `TargetLocation`이 클래스 멤버 `TargetLocation`(이동 목표 좌표 저장용)과 이름이 겹쳐 UE5의 `-Werror=shadow` 규칙에 걸려 빌드가 실패했습니다. 루프 지역변수를 `TileLocation`으로 이름을 분리해 해결했습니다.
- **턴 동기화**: 스폰된 좀비가 여러 마리일 때 각자 턴 종료를 호출하면 첫 번째로 행동을 마친 좀비가 플레이어 턴을 앞당겨 시작시킬 수 있는 구조적 이슈가 있어, 개선 작업 중입니다 (아래 TODO 참고).

## 기술 스택
- Unreal Engine 5.4
- C++ (Gameplay Framework: Character / AIController / GameModeBase / AnimInstance)

## 빌드 방법
1. Unreal Engine 5.4 설치
2. `TileDP.uproject` 우클릭 → **Generate Xcode Project Files** (Mac) / **Generate Visual Studio Project Files** (Windows)
3. `.uproject` 실행 (최초 실행 시 C++ 모듈 자동 빌드)

> 이 저장소는 코드/설정(`Source/`, `Config/`, `TileDP.uproject`)만 포함합니다.
> 레벨, 머티리얼, 메시 등 `Content/` 에셋은 마켓플레이스 라이선스 문제로 포함하지 않았습니다.

## TODO
- [ ] 물리 오버랩 기반 타일 판정을 논리적 2D 그리드로 전환
- [ ] 좀비 턴 종료를 "생존 좀비 수 == 행동 완료 수" 기준으로 동기화
- [ ] 이동 함수(4방향) 공통 함수로 통합
- [ ] 매직 넘버(타일 크기, 스폰 높이 등) 상수화
