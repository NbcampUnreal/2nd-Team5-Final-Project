# STILL LOADING
![프로젝트 커버 이미지](https://github.com/user-attachments/assets/05db7d03-49a1-484b-8728-8e656f62bf02)

## 🎮 프로젝트 개요

**STILL LOADING**은 "It Takes Two"에서 영감을 받은 **메타픽션 서스펜스 어드벤처 게임**입니다. 플레이어의 선택에 따라 새로운 플레이 방식과 미니게임을 경험하며, 게임이 진행될수록 **흑백 2D에서 실사에 가까운 3D로 진화**하는 독특한 시각적 진화를 특징으로 합니다.

### 핵심 메시지
> **"선택의 자유와 그에 따르는 책임"**

게임 세계는 미완성된 상태로 시작되며, 주인공이 버그성 오브젝트에 손을 대면서 예측 불가능한 방향으로 이야기가 전개됩니다.

## 아웃 링크
- **[공식 티저 (Short ver)](https://www.youtube.com/watch?v=JNQG27PmR0Y&feature=youtu.be)**
- **[공식 트레일러 (Full ver)](https://youtu.be/i0PMi3yc9Z4)**
- **[데모 버전 다운로드](https://github.com/NbcampUnreal/2nd-Team5-Final-Project/releases/tag/v0.1.0-demo)**

> ### **저희 게임에 대해 기술이나 더 관심이 있으신 분들은 [브로셔](https://www.notion.so/teamsparta/2-5-STILL-LOADING-21c2dc3ef51480c48cbed4cc24e4184f) 를 방문해주세요!**
 
## 팀 구성

| 이름 | 역할 | 담당 기능 |
|------|------|-----------|
| **안장훈** | 리더, 메인테이너, 프로그래밍, 사운드디렉터 | Gimmick, Objective, Rendering, SubSystem |
| **황찬호** | 부리더, 애니메이션, 프로그래밍 | BattleSystem, Character, Monster |
| **윤재성** | 애니메이션, 프로그래밍 | Boss AI, Cinematic, Gimmick |
| **이도현** | 기획, 프로그래밍 | Cinematic, Gimmick, LevelDesign |
| **조진서** | 디자이너, 프로그래밍 | Gimmick, LevelDesign |
| **최은정** | 디자이너, 애니메이션 | Effect, LevelDesign, UI |
| **최재욱** | 기획, 디자이너, 프로그래밍 | Cinematic, Gimmick, Objective, SubSystem, UI |

## 🛠️ 핵심 기술 시스템

### 렌더링 시스템
- **픽셀아트 스타일 렌더링**: HLSL 기반 픽셀화 셰이더로 레트로 감성 구현
- **카툰 스타일 렌더링**: Cel Shading과 라플라시안 필터 기반 아웃라인
- **2D 게임 시스템**: 3D 환경에서 레트로 2D 게임 경험 제공

### 캐릭터 시스템
- **Player Character**: 게임플레이 태그 기반 상태 시스템으로 유연한 캐릭터 제어
- **Character Components**: 전투, 이동, 감지 등 모듈화된 컴포넌트 구조
- **몬스터 AI**: 다중 계층 상태 머신과 군체 알고리즘 적용

### 게임플레이 시스템
- **Objective 시스템**: 5계층 아키텍처로 목표 자동화 관리
- **상호작용 & 대화 시스템**: 통합된 상호작용 오브젝트와 대화 UI 연동
- **레벨 이동 및 챕터 관리**: 챕터별 독립적인 게임 진행 관리

### 관리 시스템
- **UI 관리 시스템**: HUD 기반 LevelWidget과 서브시스템 기반 AdditiveWidget 이원화
- **사운드 관리 시스템**: 카테고리별·챕터별 통합 사운드 관리
- **텍스트 풀 시스템**: 다국어 지원과 챕터별 텍스트 관리
- **게임 설정 관리**: 사운드, 그래픽, 언어, 조작키 통합 관리

## 🔄 개발 방법론

### 마일스톤 기반 개발
- 전체 개발 기간을 **4개 마일스톤**으로 분할
- 메인 이슈 → 서브 이슈 구조로 작업 세분화
- **"1 이슈 - 1 브랜치 - 1 Pull Request"** 원칙 준수

### 브랜치 전략
- **Rebase 기반 병합**으로 선형 커밋 히스토리 유지
- **GitHub Actions 기반 컨플릭트 방지 시스템** 구축
- 라벨 시스템을 통한 명확한 작업 분류

## 🚀 추후 도전 계획

### 시네마틱 페이셜 캡쳐
- 실제 얼굴 움직임을 3D 캐릭터에 적용
- 성우 더빙과 연동한 생동감 있는 시네마틱 제작

### 스팀 API 연동
- **도전 과제**: 비공개 도전 과제로 숨겨진 컨텐츠 안내
- **스팀 Cloud**: 여러 기기 간 진행 상황 동기화


## 📝 개발 환경

- **Engine**: Unreal Engine 5
- **Language**: C++, Blueprint
- **Version Control**: Git + GitHub Actions
- **Project Management**: GitHub Issues & Milestones
