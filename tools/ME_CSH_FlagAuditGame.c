#ifdef WORKBENCH
modded class ME_CSH_PreviewEntity
{
 static int s_CSH_TestFlagMaterials;
 static int s_CSH_TestWrongTargets;
 override protected void SetPreviewObject(VObject mesh, ResourceName material)
 {
  super.SetPreviewObject(mesh, material);
  if (material.EndsWith("FlagFaction.emat"))
  {
   s_CSH_TestFlagMaterials++;
   if (!m_CSH_IsFlag) s_CSH_TestWrongTargets++;
  }
 }

 string CSH_TestFaction() { return m_CSH_FactionKey; }

 int CSH_TestFlagColors(int expected, out int wrong)
 {
  int count;
  if (m_CSH_IsFlag)
  {
   count++;
   ParametricMaterialInstanceComponent colorComponent = ParametricMaterialInstanceComponent.Cast(FindComponent(ParametricMaterialInstanceComponent));
   if (!colorComponent || colorComponent.GetColor() != expected)
    wrong++;
  }
  array<SCR_BasePreviewEntity> children = GetPreviewChildren();
  if (!children) return count;
  foreach (SCR_BasePreviewEntity child : children)
  {
   ME_CSH_PreviewEntity preview = ME_CSH_PreviewEntity.Cast(child);
   if (!preview) continue;
   int childWrong;
   count += preview.CSH_TestFlagColors(expected, childWrong);
   wrong += childWrong;
  }
  return count;
 }
}

modded class SCR_CampaignMilitaryBaseComponent
{
 string CSH_TestSelectedFactions()
 {
  string result;
  foreach (SCR_BasePreviewEntity preview : m_CSH_Previews)
   result += ME_CSH_PreviewEntity.Cast(preview).CSH_TestFaction() + ",";
  return result;
 }

 int CSH_TestColorState(out int wrong, out int cloths)
 {
  int colored;
  FactionManager manager = GetGame().GetFactionManager();
  foreach (SCR_BasePreviewEntity entity : m_CSH_Previews)
  {
   ME_CSH_PreviewEntity preview = ME_CSH_PreviewEntity.Cast(entity);
   if (!preview) { wrong++; continue; }
   string key = preview.CSH_TestFaction();
   int expected = Color.White.PackToInt();
   if (!key.IsEmpty())
   {
    Faction faction;
    if (manager) faction = manager.GetFactionByKey(key);
    if (!faction) { wrong++; continue; }
    expected = faction.GetFactionColor().PackToInt();
   }
   int localWrong;
   int count = preview.CSH_TestFlagColors(expected, localWrong);
   cloths += count;
   wrong += localWrong;
   if (!key.IsEmpty()) colored += count;
  }
  return colored;
 }
}
#endif
