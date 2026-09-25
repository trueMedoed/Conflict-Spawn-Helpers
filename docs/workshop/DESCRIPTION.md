# Workshop page text

Source text for the Summary and Description fields of ME_Conflict_Spawn_Helpers. Release 1.0: publication and a Workshop photo confirmed by the author on 2026-09-25. Copy only the text beneath each field heading. Preserve the literal • bullets and numbered steps when pasting.

The Russian reference translation is in [RU_DESCRIPTION.md](RU_DESCRIPTION.md). Keep both versions synchronized. Release notes are in [CHANGELOG.md](CHANGELOG.md).

## Summary

Place Conflict bases and supply sources with confidence: preview where objects will appear and which way the supply storage interaction sign will face.

## Description

Conflict Spawn Helpers shows previews of the compositions that Conflict creates at military bases and supply sources when a mission starts. Use the holograms to leave room for tents, supply storage, and other objects while building your scenario in Workbench.

Supported points
• ConflictBase_MOB
• ConflictControlPoint
• ConflictSourceBase_T1Harbor
• ConflictSourceBase_T2Harbor
• ConflictSourceBase_T3Harbor

Features
• Shows composition geometry from the world's configured campaign factions before you enter Game mode.
• Updates previews when points move or rotate.
• Auto uses an ordinary point's default faction. When none is assigned, it reads the INDFOR faction configured in the current Conflict GameMode.
• For HQ candidates, Auto shows one composition with the largest rectangular footprint, plus a yellow rectangle enclosing the preview geometry of all available variants. The caption explains that the final composition may occupy less space.
• Highlights the construction-supply sign with a solid bright orange material so it is easier to locate.
• Lets you toggle previews and cycle through Auto, US, USSR, and FIA without changing the mission's faction assignments.
• Cleans up previews when points are deleted and during Edit/Game transitions. Preview geometry has no physics and is not saved as mission objects.
• Includes unnamed.ent, a Workbench test world with base points and faction-specific Harbor examples.

How to use
1. Load the addon in Workbench and open your Conflict world in World Editor.
2. Make sure the world has a CampaignFactionManager with the required campaign factions and building compositions configured.
3. Place one of the supported points, or inspect points already present in the scenario. Previews are enabled by default for visible points.
4. Open Plugins → [ME] Conflict Spawn Helpers → Toggle Conflict spawn previews to show or hide the helpers.
5. Use Cycle Conflict preview faction (Auto/US/USSR/FIA) to inspect a specific faction's composition. Manual faction selection hides the combined HQ outline.
6. Leave space around the preview and check the actual composition in Game mode, especially on slopes.

Notes and limitations
• This is a Workbench mission-making helper. It does not change Conflict's runtime spawning or base ownership.
• Terrain alignment is not simulated. Heights and slopes of individual objects may differ from the actual spawned composition.
• Previews use the game's preview geometry, which can be simplified. Custom preview prefabs are omitted and marked as partial.
• The HQ outline is a conservative rectangle around available preview geometry, not an exact collision outline or a prediction of the selected HQ faction. A candidate configured to disappear when not selected as HQ may have no composition at all.
• If an ordinary point's faction cannot be resolved, the helper shows the available unique composition variants. Overlapping holograms can be inspected using the manual faction selector.
• Orange highlighting recognizes descendants of the standard construction-supply sign prefab. Unrecognized custom signs keep the normal preview material.
• Auto can read custom campaign factions; the manual selector uses the vanilla US, USSR, and FIA keys. Compatibility with other mods has not been verified.
• Tested with Arma Reforger Workbench 1.8.0.13. The included test world is an editor demonstration, not a complete playable Conflict scenario.
• After changing addon scripts, restart Workbench. After changing composition resources, toggling previews or reopening the world may be necessary.

Source code and test project
https://github.com/trueMedoed/Conflict-Spawn-Helpers
The repository may contain development changes not yet included in the Workshop release.