#ifdef WORKBENCH
//! Only used by Conflict Spawn Helpers; other editor previews are unaffected.
class ME_CSH_PreviewEntityClass : SCR_BasePreviewEntityClass
{
}

class ME_CSH_PreviewEntity : SCR_BasePreviewEntity
{
 protected static const ResourceName CSH_SIGN_MATERIAL = "{FF26FD6B6D4EDF7E}Assets/Editor/ConflictSpawnHelpers/SignHighlight.emat";
 protected static const ResourceName CSH_FLAG_MATERIAL = "{9B4912FD556246EE}Assets/Editor/ConflictSpawnHelpers/FlagFaction.emat";

 protected bool m_CSH_IsFlag;
 protected string m_CSH_FactionKey;

 //! Applied per composition after spawning, never through global faction state.
 void CSH_SetFaction(string factionKey, int factionColor)
 {
  m_CSH_FactionKey = factionKey;
  if (m_CSH_IsFlag)
  {
   ResourceName material = "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat";
   if (factionColor)
    material = CSH_FLAG_MATERIAL;
   Resource resource = Resource.Load(material);
   if (resource && resource.IsValid())
   {
    SetPreviewObject(GetVObject(), material);
    ParametricMaterialInstanceComponent colorComponent = ParametricMaterialInstanceComponent.Cast(FindComponent(ParametricMaterialInstanceComponent));
    if (colorComponent)
    {
     if (factionColor)
      colorComponent.SetColor(factionColor);
     else
      colorComponent.SetColor(Color.White.PackToInt());
    }
   }
  }
  array<SCR_BasePreviewEntity> children = GetPreviewChildren();
  if (children)
  {
   foreach (SCR_BasePreviewEntity child : children)
   {
    ME_CSH_PreviewEntity preview = ME_CSH_PreviewEntity.Cast(child);
    if (preview) preview.CSH_SetFaction(factionKey, factionColor);
   }
  }
 }

 protected bool CSH_IsFlagSource(BaseContainer source)
 {
  while (source)
  {
   string resource = source.GetResourceName();
   if (resource.EndsWith("Prefabs/Props/Core/Flag_Base.et"))
    return true;
   source = source.GetAncestor();
  }
  return false;
 }

 //! Match the supply sign prefab family, including faction/custom descendants.
 //! A generic rectangle/sign mesh alone is not enough to identify a supply sign.
 protected bool CSH_IsSupplySign(BaseContainer source)
 {
  while (source)
  {
   string resource = source.GetResourceName();
   if (resource.EndsWith("Prefabs/Structures/Signs/Military/Supplies/Sign_SupplyStorage_01_ConstructionMat_base.et"))
    return true;
   source = source.GetAncestor();
  }
  return false;
 }

 override protected void EOnPreviewInit(SCR_BasePreviewEntry entry, SCR_BasePreviewEntity root)
 {
  super.EOnPreviewInit(entry, root);
  if (entry.m_Shape != EPreviewEntityShape.MESH || !GetVObject())
   return;
  // Baked editable previews omit IEntitySource for child entries.
  // Match only the two vanilla cloth models, never the pole or arbitrary signs.
  string meshPath = entry.m_Mesh;
  m_CSH_IsFlag = CSH_IsFlagSource(entry.m_EntitySource)
   || meshPath.EndsWith("Assets/Props/Fabric/Flags/Flag_1_2.xob")
   || meshPath.EndsWith("Assets/Props/Fabric/Flags/Flag_2_3.xob");
  if (!CSH_IsSupplySign(entry.m_EntitySource))
   return;

  Resource material = Resource.Load(CSH_SIGN_MATERIAL);
  if (material && material.IsValid())
   SetPreviewObject(GetVObject(), CSH_SIGN_MATERIAL);
 }
}
#endif
