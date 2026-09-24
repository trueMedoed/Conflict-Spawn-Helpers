[WorkbenchPluginAttribute(name: "Toggle Conflict spawn previews", description: "Show or hide future Conflict base compositions.", wbModules: { "WorldEditor" }, category: "Conflict Spawn Helpers")]
class ME_CSH_PreviewPlugin : WorldEditorPlugin
{
 override void Run()
 {
  SCR_CampaignMilitaryBaseComponent.CSH_Toggle();
 }

 override void OnGameModeStarted(string worldName, string gameMode, bool playFromCameraPos, vector cameraPosition, vector cameraAngles)
 {
  SCR_CampaignMilitaryBaseComponent.CSH_ClearAll();
 }

 override void OnGameModeEnded()
 {
  SCR_CampaignMilitaryBaseComponent.CSH_ClearAll();
 }
}

[WorkbenchPluginAttribute(name: "Cycle Conflict preview faction (Auto/US/USSR/FIA)", description: "Choose a planning variant without changing the mission faction.", wbModules: { "WorldEditor" }, category: "Conflict Spawn Helpers")]
class ME_CSH_FactionPreviewPlugin : WorldEditorPlugin
{
 override void Run()
 {
  SCR_CampaignMilitaryBaseComponent.CSH_CycleFaction();
 }
}