#ifdef WORKBENCH
//! Only used by Conflict Spawn Helpers; other editor previews are unaffected.
class ME_CSH_PreviewEntityClass : SCR_BasePreviewEntityClass
{
}

class ME_CSH_PreviewEntity : SCR_BasePreviewEntity
{
 protected static const ResourceName CSH_SIGN_MATERIAL = "{FF26FD6B6D4EDF7E}Assets/Editor/ConflictSpawnHelpers/SignHighlight.emat";

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
  if (entry.m_Shape != EPreviewEntityShape.MESH || !GetVObject() || !CSH_IsSupplySign(entry.m_EntitySource))
   return;

  Resource material = Resource.Load(CSH_SIGN_MATERIAL);
  if (material && material.IsValid())
   SetPreviewObject(GetVObject(), CSH_SIGN_MATERIAL);
 }
}
#endif