# Исследование спавна Conflict

Источник: установленная stable-игра 1.8.0.13, PAK-архивы `Arma Reforger/addons/data`. Проверено 24–25.09.2026. Экспериментальная ветка не использовалась.

## Целевые маркеры

Все находятся в `Prefabs/Systems/MilitaryBase/`:

- `ConflictBase_MOB.et`: наследует ConflictBase_Base; CanBeHQ и DisableWhenUnusedAsHQ включены.
- `ConflictControlPoint.et`: наследует ConflictBase_Base; IsControlPoint включён.
- `ConflictSourceBase_T1Harbor.et`, `T2Harbor.et`, `T3Harbor.et`: наследуют ConflictSourceBase; отличаются параметрами снабжения и патрулями. В ConflictSourceBase компонент заменён на SCR_CampaignSourceBaseComponent, наследника SCR_CampaignMilitaryBaseComponent, тип SOURCE_BASE.

## Выбор композиции

`scripts/Game/Components/Locations/SCR_CampaignMilitaryBaseComponent.c`, OnAllBasesInitialized (2329–2373): после инициализации баз, на авторитетной стороне и при отсутствии уже созданной композиции выбирает SERVICE_HQ для BASE или SERVICE_SOURCE_BASE для SOURCE_BASE. Получает prefab через GetCampaignFaction().GetBuildingPrefab(label). Если prefab отсутствует, выбирает между фракциями BLUFOR/OPFOR GameMode случайно. Спавн отложен на 1000 мс.

`scripts/Game/Faction/SCR_CampaignFaction.c`, GetBuildingPrefab (186–201): SERVICE_HQ → m_BaseBuildingHQ; SERVICE_SOURCE_BASE → m_sBaseBuildingSourceBase.

| Фракция | HQ | Источник припасов |
| --- | --- | --- |
| US | `{16AF2FE47A6C297B}…/E_Headquarters_S_Conflict_US_01.et` | `{836D5475DD7C8819}…/E_SourceBase_S_US_01.et` |
| USSR | `{D70BF977E6D6300C}…/E_Headquarters_S_Conflict_USSR_01.et` | `{C42E98A6F37D15E1}…/E_SourceBase_S_USSR_01.et` |
| FIA | `{16E06E20F4641F8A}…/E_Headquarters_S_Conflict_FIA_01.et` | `{4F9FE516AEBE56E4}…/E_SourceBase_S_FIA_01.et` |

Общий префикс путей: `PrefabsEditable/Auto/Compositions/Slotted/SlotFlatSmall/`. Это справочные значения, код получает ресурсы из текущих фракций сцены, а не использует этот список как жёсткий каталог.

## Положение

SpawnBuilding (2601–2650): world position и yaw/pitch/roll берутся у маркера; затем композиция привязывается к поверхности через SCR_TerrainHelper.SnapToTerrain, а editable-композиция обновляется через SetTransformWithChildren. Габариты и дополнительные элементы принадлежат prefab-композиции, не самому маркеру.

Первый прототип использует исходную позицию и поворот. Повторение terrain alignment и сравнение детальных дочерних трансформаций — отдельная незавершённая часть задачи.

## Preview

Штатные `scripts/Game/Entities/Preview/SCR_PrefabPreviewEntity.c` и `SCR_BasePreviewEntity.c` позволяют получить иерархию preview-элементов, включая editor links, затем создать её на основе класса SCR_BasePreviewEntity и материала `{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat`. Игровой prefab целиком для подсказки не инстанцируется. Custom preview-prefab отключены; их пропуск отражается в подписи.

## Инструменты

Найден локальный npm-пакет `enfusion-mcp` 0.6.1. Его reader на этой установке искал архивы прямо в addons вместо addons/data и прибавлял DATA offset к уже абсолютным смещениям файлов. Для исследования использован временный адаптер: список архивов из addons/data, абсолютные смещения. Проверка на ConflictBase_MOB показала корректное начало `GenericEntity` только при абсолютном смещении. Установленный пакет не изменялся.

Внешние справочные проекты:

- https://github.com/BohemiaInteractive/Arma-Reforger-Script-Diff
- https://github.com/EnfusionMCP/EnfusionMCP
- https://github.com/burn0ut7/Reforger-Codex-Agent-Skill/tree/main/skills

Онлайн-исходник может отличаться от установленной игры; реализация сверена с локальными скриптами и компилятором.

## Начальная фракция обычной базы

25.09.2026: проверен локальный SCR_CampaignMilitaryBaseManager.c, InitializeBases (около строки 604). Если текущий владелец отсутствует, игра использует GetFaction(true), затем GetFactionByEnum(SCR_ECampaignFaction.INDFOR) из текущего SCR_GameModeCampaign. Preview теперь использует тот же fallback для точек без CanBeHQ, читая настройки без SetFaction и без запуска InitializeBases. INDFOR не обязательно FIA: ключ задаёт миссия. В CTI_Campaign_HQC_Arland.ent присутствуют SCR_GameModeCampaign (GameMode_CampaignHQC) и SCR_CampaignFactionManager.

HQ обрабатываются отдельно в SetHQFactions: назначение BLUFOR/OPFOR зависит от выбранной пары и исходных владельцев. Помощник пока сохраняет набор возможных композиций для CanBeHQ и не имитирует случайный выбор штабов.
