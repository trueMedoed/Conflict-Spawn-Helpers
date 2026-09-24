// Temporary development handler; not distributed with the addon.
class ME_CSH_AuditRequest : JsonApiStruct
{
 string action;
 void ME_CSH_AuditRequest() { RegV("action"); }
}
class ME_CSH_AuditResponse : JsonApiStruct
{
 string result;
 void ME_CSH_AuditResponse() { RegV("result"); }
}
class ME_CSH_Audit : NetApiHandler
{
 override JsonApiStruct GetRequest() { return new ME_CSH_AuditRequest(); }
 override JsonApiStruct GetResponse(JsonApiStruct request)
 {
  ME_CSH_AuditResponse response = new ME_CSH_AuditResponse();
  ME_CSH_AuditRequest input = ME_CSH_AuditRequest.Cast(request);
  WorldEditor editor = Workbench.GetModule(WorldEditor);
  if (!editor) { response.result = "NO_EDITOR"; return response; }
  if (input.action == "open")
  {
   response.result = editor.SetOpenedResource("worlds/MP/MpTest/MpTest_Basic.ent").ToString();
   return response;
  }
  WorldEditorAPI api = editor.GetApi();
  if (!api || !api.GetWorld()) { response.result = "NO_WORLD"; return response; }
  int countBefore = api.GetEditorEntityCount();
  IEntity manager;
  if (!GetGame().GetFactionManager())
   manager = GetGame().SpawnEntityPrefabLocal(Resource.Load("Prefabs/MP/Campaign/CampaignFactionManager.et"), api.GetWorld());
  array<string> paths = {"ConflictBase_MOB", "ConflictControlPoint", "ConflictSourceBase_T1Harbor", "ConflictSourceBase_T2Harbor", "ConflictSourceBase_T3Harbor"};
  foreach (string name : paths)
  {
   EntitySpawnParams params = new EntitySpawnParams();
   params.TransformMode = ETransformMode.WORLD;
   params.Transform[3] = Vector(128, 10, 128);
   IEntity marker = GetGame().SpawnEntityPrefabLocal(Resource.Load("Prefabs/Systems/MilitaryBase/" + name + ".et"), api.GetWorld(), params);
   if (!marker) { response.result += name + "=SPAWN_FAILED; "; continue; }
   SCR_CampaignMilitaryBaseComponent base = SCR_CampaignMilitaryBaseComponent.Cast(marker.FindComponent(SCR_CampaignMilitaryBaseComponent));
   if (base)
   {
    base._WB_AfterWorldUpdate(marker, 1.1);
    int first = base.CSH_GetPreviewCount();
    marker.SetOrigin(Vector(138, 10, 138));
    marker.SetYawPitchRoll(Vector(45, 0, 0));
    base._WB_AfterWorldUpdate(marker, 1.1);
    int moved = base.CSH_GetPreviewCount();
    string snapshot = base.CSH_TestSnapshot(marker);
    array<string> variants = {"US", "USSR", "FIA"};
    foreach (string variant : variants)
    {
     SCR_CampaignMilitaryBaseComponent.s_CSH_FactionFilter = variant;
     base._WB_AfterWorldUpdate(marker, 1.1);
     response.result += string.Format("%1/%2=%3; ", name, variant, base.CSH_GetPreviewCount());
    }
    SCR_CampaignMilitaryBaseComponent.s_CSH_FactionFilter = "";
    SCR_CampaignMilitaryBaseComponent.s_CSH_Enabled = false;
    base._WB_AfterWorldUpdate(marker, 1.1);
    response.result += string.Format("%1/disabled=%2; ", name, base.CSH_GetPreviewCount());
    SCR_CampaignMilitaryBaseComponent.s_CSH_Enabled = true;
    base._WB_AfterWorldUpdate(marker, 1.1);
    response.result += string.Format("%1/re-enabled=%2; %3; ", name, base.CSH_GetPreviewCount(), snapshot);
    base.CSH_Clear();
    response.result += string.Format("%1=%2,moved=%3,cleared=%4; ", name, first, moved, base.CSH_GetPreviewCount());
   }
   else response.result += name + "=NO_COMPONENT; ";
   SCR_EntityHelper.DeleteEntityAndChildren(marker);
  }
  if (manager) SCR_EntityHelper.DeleteEntityAndChildren(manager);
  response.result += string.Format("editorSources=%1/%2", countBefore, api.GetEditorEntityCount());
  return response;
 }
}