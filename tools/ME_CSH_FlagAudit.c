// Temporary CLI diagnostic. Never include this file in a published addon.
[WorkbenchPluginAttribute(name: "[ME] CSH flag audit", wbModules: {"WorldEditor"})]
class ME_CSH_FlagAuditPlugin : WorkbenchPlugin
{
 override void RunCommandline()
 {
  WorldEditor editor = Workbench.GetModule(WorldEditor);
  if (!editor || !editor.SetOpenedResource("worlds/MP/MpTest/MpTest_Basic.ent")) { Print("[CSH_FLAG_TEST] FAIL open"); return; }
  WorldEditorAPI api = editor.GetApi();
  int before = api.GetEditorEntityCount();
  IEntity manager = GetGame().SpawnEntityPrefabLocal(Resource.Load("Prefabs/MP/Campaign/CampaignFactionManager.et"), api.GetWorld());
  FactionManager factionManager = GetGame().GetFactionManager();
  array<string> families = {"Headquarters_S_Conflict", "SourceBase_S"};
  array<string> factions = {"US", "USSR", "FIA"};
  int failures;
  array<string> roots = {"Prefabs/Compositions/Slotted/SlotFlatSmall/", "PrefabsEditable/Auto/Compositions/Slotted/SlotFlatSmall/E_"};
  foreach (string rootPath : roots)
  {
  foreach (string family : families)
  {
   foreach (string faction : factions)
   {
    Resource resource = Resource.Load(rootPath + family + "_" + faction + "_01.et");
    array<ref SCR_BasePreviewEntry> entries = {};
    SCR_PrefabPreviewEntity.GetPreviewEntries(SCR_BaseContainerTools.FindEntitySource(resource), entries);
    foreach (SCR_BasePreviewEntry entry : entries)
     if (entry.m_Shape == EPreviewEntityShape.PREFAB) { entry.m_Shape = EPreviewEntityShape.MESH; entry.m_Mesh = ResourceName.Empty; }
    ME_CSH_PreviewEntity preview = ME_CSH_PreviewEntity.Cast(SCR_BasePreviewEntity.SpawnPreview(entries, "{698891CB96CFF3B9}Prefabs/Editor/ME_CSH_PreviewEntity.et", api.GetWorld(), null, "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", EPreviewEntityFlag.IGNORE_TERRAIN));
    if (!preview) { failures++; continue; }
    Faction resolved = factionManager.GetFactionByKey(faction);
    int expected = resolved.GetFactionColor().PackToInt();
    ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials = 0;
    ME_CSH_PreviewEntity.s_CSH_TestWrongTargets = 0;
    preview.CSH_SetFaction(faction, expected);
    int wrong;
    int cloths = preview.CSH_TestFlagColors(expected, wrong);
    bool valid = cloths == 1 && wrong == 0 && ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials == 1 && ME_CSH_PreviewEntity.s_CSH_TestWrongTargets == 0;
    if (!valid) failures++;
    PrintFormat("[CSH_FLAG_TEST] %1/%2 valid=%3 cloths=%4 wrong=%5 materialCount=%6 color=%7", rootPath + family, faction, valid, cloths, wrong, ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials, expected);
    preview.CSH_SetFaction("", 0);
    int neutralWrong;
    int neutralCloths = preview.CSH_TestFlagColors(Color.White.PackToInt(), neutralWrong);
    if (neutralCloths != 1 || neutralWrong != 0) failures++;
    preview.CSH_SetFaction("CUSTOM_UNKNOWN", 0);
    int unknownWrong;
    preview.CSH_TestFlagColors(Color.White.PackToInt(), unknownWrong);
    if (unknownWrong != 0) failures++;
    delete preview;
   }
  }
  }
  array<string> markers = {"ConflictBase_MOB", "ConflictControlPoint", "ConflictSourceBase_T1Harbor", "ConflictSourceBase_T2Harbor", "ConflictSourceBase_T3Harbor"};
  foreach (string markerName : markers)
  {
   IEntity marker = GetGame().SpawnEntityPrefabLocal(Resource.Load("Prefabs/Systems/MilitaryBase/" + markerName + ".et"), api.GetWorld());
   SCR_CampaignMilitaryBaseComponent base = SCR_CampaignMilitaryBaseComponent.Cast(marker.FindComponent(SCR_CampaignMilitaryBaseComponent));
   foreach (string key : factions)
   {
    ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials = 0;
    SCR_CampaignMilitaryBaseComponent.s_CSH_FactionFilter = key;
    base._WB_AfterWorldUpdate(marker, 1.1);
    string selected = base.CSH_TestSelectedFactions();
    int wrong;
    int cloths;
    int colored = base.CSH_TestColorState(wrong, cloths);
    if (selected != key + "," || wrong != 0 || colored != 1 || cloths != 1 || ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials != colored) failures++;
    PrintFormat("[CSH_FLAG_TEST] %1/%2 selected=%3 colored=%4 cloths=%5 wrong=%6", markerName, key, selected, colored, cloths, wrong);
   }
   ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials = 0;
   SCR_CampaignMilitaryBaseComponent.s_CSH_FactionFilter = "";
   base._WB_AfterWorldUpdate(marker, 1.1);
   int autoWrong;
   int autoCloths;
   int autoColored = base.CSH_TestColorState(autoWrong, autoCloths);
   if (autoWrong != 0 || autoCloths == 0 || ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials != autoColored) failures++;
   if (base.CanBeHQ() && autoColored != 0) failures++;
   if (!base.CanBeHQ() && autoColored == 0) failures++;
   PrintFormat("[CSH_FLAG_TEST] %1/Auto colored=%2 cloths=%3 wrong=%4", markerName, autoColored, autoCloths, autoWrong);
   if (markerName == "ConflictBase_MOB" && base.CSH_GetPreviewCount() != 1) failures++;
   base.CSH_Clear();
   SCR_EntityHelper.DeleteEntityAndChildren(marker);
  }
  if (manager) SCR_EntityHelper.DeleteEntityAndChildren(manager);
  if (before != api.GetEditorEntityCount()) failures++;
  PrintFormat("[CSH_FLAG_TEST] failures=%1 sources=%2/%3", failures, before, api.GetEditorEntityCount());
  if (!editor.SetOpenedResource("{00A3173F794B3AA0}unnamed.ent")) { Print("[CSH_FLAG_WORLD] FAIL open"); return; }
  api = editor.GetApi();
  SCR_CampaignMilitaryBaseComponent.CSH_ClearAll();
  int checked;
  int worldFailures;
  int worldSources = api.GetEditorEntityCount();
  for (int i = 0; i < worldSources; i++)
  {
   IEntity entity = api.SourceToEntity(api.GetEditorEntity(i));
   if (!entity) continue;
   SCR_CampaignMilitaryBaseComponent point = SCR_CampaignMilitaryBaseComponent.Cast(entity.FindComponent(SCR_CampaignMilitaryBaseComponent));
   if (!point) continue;
   ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials = 0;
   point._WB_AfterWorldUpdate(entity, 1.1);
   checked++;
   int worldWrong;
   int worldCloths;
   int worldColored = point.CSH_TestColorState(worldWrong, worldCloths);
   if (worldWrong != 0 || worldCloths == 0 || ME_CSH_PreviewEntity.s_CSH_TestFlagMaterials != worldColored) worldFailures++;
   if (point.CanBeHQ() && worldColored != 0) worldFailures++;
   if (!point.CanBeHQ() && worldColored != 1) worldFailures++;
   PrintFormat("[CSH_FLAG_WORLD] %1: selected=%2 colored=%3 cloths=%4 wrong=%5", entity.GetOrigin(), point.CSH_TestSelectedFactions(), worldColored, worldCloths, worldWrong);
  }
  PrintFormat("[CSH_FLAG_WORLD] checked=%1 failures=%2 sources=%3/%4 (not saved)", checked, worldFailures, worldSources, api.GetEditorEntityCount());
 }
}
