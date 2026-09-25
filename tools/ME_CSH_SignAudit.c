// Temporary diagnostic handler; copy into WorkbenchGame only for testing.
class ME_CSH_SignAuditRequest : JsonApiStruct
{
 string action;
 void ME_CSH_SignAuditRequest() { RegV("action"); }
}
class ME_CSH_SignAuditResponse : JsonApiStruct
{
 string result;
 void ME_CSH_SignAuditResponse() { RegV("result"); }
}
class ME_CSH_SignAudit : NetApiHandler
{
 override JsonApiStruct GetRequest() { return new ME_CSH_SignAuditRequest(); }
 override JsonApiStruct GetResponse(JsonApiStruct request)
 {
  ME_CSH_SignAuditResponse response = new ME_CSH_SignAuditResponse();
  WorldEditor editor = Workbench.GetModule(WorldEditor);
  if (!editor) { response.result = "NO_EDITOR"; return response; }
  if (ME_CSH_SignAuditRequest.Cast(request).action == "open")
  {
   response.result = editor.SetOpenedResource("worlds/MP/MpTest/MpTest_Basic.ent").ToString();
   return response;
  }
  WorldEditorAPI api = editor.GetApi();
  if (!api || !api.GetWorld()) { response.result = "NO_WORLD"; return response; }
  int before = api.GetEditorEntityCount();
  array<string> families = {"Headquarters_S_Conflict", "SourceBase_S"};
  array<string> factions = {"US", "USSR", "FIA"};
  foreach (string family : families)
  {
   foreach (string faction : factions)
   {
    string path = "Prefabs/Compositions/Slotted/SlotFlatSmall/" + family + "_" + faction + "_01.et";
    Resource resource = Resource.Load(path);
    array<ref SCR_BasePreviewEntry> entries = {};
    SCR_PrefabPreviewEntity.GetPreviewEntries(SCR_BaseContainerTools.FindEntitySource(resource), entries);
    foreach (SCR_BasePreviewEntry entry : entries)
    {
     if (entry.m_Shape == EPreviewEntityShape.PREFAB) { entry.m_Shape = EPreviewEntityShape.MESH; entry.m_Mesh = ResourceName.Empty; }
    }
    ME_CSH_PreviewEntity.s_CSH_TestSignMaterials = 0;
    SCR_BasePreviewEntity preview = SCR_BasePreviewEntity.SpawnPreview(entries, "ME_CSH_PreviewEntity", api.GetWorld(), null, "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", EPreviewEntityFlag.IGNORE_TERRAIN);
    response.result += string.Format("%1_%2: signs=%3,spawned=%4; ", family, faction, ME_CSH_PreviewEntity.s_CSH_TestSignMaterials, preview != null);
    if (preview) delete preview;
   }
  }
  response.result += string.Format("sources=%1/%2", before, api.GetEditorEntityCount());
  return response;
 }
}
[WorkbenchPluginAttribute(name: "CSH sign audit", wbModules: {"WorldEditor"})]
class ME_CSH_SignAuditPlugin : WorkbenchPlugin
{
 override void RunCommandline()
 {
  ME_CSH_SignAudit handler = new ME_CSH_SignAudit();
  ME_CSH_SignAuditRequest request = new ME_CSH_SignAuditRequest();
  request.action = "open";
  ME_CSH_SignAuditResponse opened = ME_CSH_SignAuditResponse.Cast(handler.GetResponse(request));
  Print("[CSH_SIGN_TEST] open=" + opened.result);
  request.action = "audit";
  ME_CSH_SignAuditResponse result = ME_CSH_SignAuditResponse.Cast(handler.GetResponse(request));
  Print("[CSH_SIGN_TEST] " + result.result);
 }
}
