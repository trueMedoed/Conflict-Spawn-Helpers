# How Conflict Spawn Helpers works

[Русская версия](HOW_IT_WORKS_RU.md)

## The problem it solves

The mod's author described the original problem as follows:

> In vanilla, the point is just a small, barely visible sphere with an arrow. There is no indication of its dimensions.

In World Editor, this marker shows a point's position and direction, but does not explain how much space the objects will occupy when Conflict starts. A base will receive a composition with a tent; a supply source will receive storage and other objects. Before the mission starts, its author can accidentally place scenery, a fence, or a building directly where those objects will appear.

The position of the sign used to open the supply storage interface also matters. A small sphere with an arrow makes it difficult to tell where the sign will be and which way it will face. Even when the composition itself fits, access to the sign can be blocked accidentally.

Conflict Spawn Helpers shows the future composition while you edit: the hologram helps you judge its placement, and the highlighted sign makes its position and orientation easier to see. Checking intersections and leaving a clear approach remain the mission author's responsibility: the mod does not yet detect obstacles automatically.

## What mission makers see

The helpers support ConflictBase_MOB, ConflictControlPoint, and ConflictSourceBase_T1Harbor/T2Harbor/T3Harbor.

Holograms appear alongside the point, showing the composition elements included in its preview data: tents, storage, shelters, crates, and other objects. The previews follow the marker's position and rotation. The construction-supply storage sign uses a solid bright orange material so it stands out among the translucent objects.

Plugins → Conflict Spawn Helpers provides two commands:

- **Toggle Conflict spawn previews** — show or hide the helpers.
- **Cycle Conflict preview faction (Auto/US/USSR/FIA)** — switch the composition variant for inspection without changing the mission's faction assignments.

## Where the composition comes from

When Conflict starts, the game selects a composition based on the base type and its faction. The helper reads building settings from the current world's factions: SERVICE_HQ for ordinary bases and SERVICE_SOURCE_BASE for supply sources. The mod does not maintain manually drawn copies of tents and storage structures.

In Auto mode, an ordinary point's default faction is checked first. If none is assigned, the helper uses the INDFOR faction configured in the current Conflict GameMode, following the vanilla initialization of ordinary bases. This is not necessarily FIA: a mission can assign another faction to that role. A CampaignFactionManager with configured campaign factions is required to obtain the compositions.

If a single variant cannot be resolved, the helper displays the available unique compositions from the world's factions. You can inspect them individually using the manual selector. This set helps with space planning, but does not reproduce every random decision or fallback used by runtime spawning.

## Why HQ candidates have a combined outline

Main base ownership may only be determined when the mission starts. For HQ candidates, Auto therefore shows one composition with the largest enclosing rectangle by area, plus a yellow rectangular outline enclosing the preview geometry of all available variants.

This matters because the largest composition does not necessarily cover all the others: a crate or shelter in another variant may extend farther to one side. The combined boundary accounts for these protrusions, while a single hologram avoids overlapping several tents.

The outline moves and rotates with the point. Its caption warns that the final composition may occupy less space. If a candidate is configured to be disabled when it is not selected as HQ, no composition may appear at all. Selecting a faction manually shows only that variant, without the combined outline.

## How the helpers are created

The helper uses the prefab's standard preview data and creates temporary geometry through SCR_BasePreviewEntity. It does not spawn a full gameplay composition just to display a preview. Custom preview prefabs are omitted and marked as partial.

The custom ME_CSH_PreviewEntity class identifies the sign through its prefab ancestry. Only that sign's preview receives SignHighlight.emat, which provides the solid orange fill. Other signs and previews created by unrelated tools are therefore not recolored. Unrecognized custom signs retain the normal material.

The helpers exist only in the editor, have no physics, and are not saved as mission objects. They are cleared when a point is deleted or when you enter Game mode, and restored when you return to editing. Conflict remains responsible for spawning gameplay objects and assigning base ownership.

## Things to keep in mind

- **Terrain alignment is not simulated yet.** The game additionally snaps objects to the surface. On slopes, individual elements may have different heights and tilts from those shown in the hologram.
- Preview geometry can be simplified. The HQ outline encloses the available preview geometry and is not an exact collision boundary.
- Highlighting helps you locate the sign, but does not check whether interaction is available or whether the approach is clear.
- Manual selection uses the US/USSR/FIA keys; Auto can read custom campaign factions. Compatibility with other mods has not been separately verified.

A practical workflow is to place a point, inspect the hologram and the sign's direction, leave clear space around it, then check the actual composition and interaction in Game mode.

## Examples and further details

- [unnamed.ent test world](../ME_Conflict_Spawn_Helpers/unnamed.ent) — sample points and faction-specific Harbor variants; a Workbench demonstration, not a complete playable mission.
- [Spawn research](SPAWN_RESEARCH.md) (Russian) — game source code, resources, and the technical basis of the implementation.
- [Validation results](VALIDATION.md) (Russian) — compilation, engine diagnostics, and checks confirmed by the user.
- [README](../README.md) (Russian) — getting started and current project status.