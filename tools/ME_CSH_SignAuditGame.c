#ifdef WORKBENCH
// Temporary diagnostic extension. Name ZZ_CSH_SignAuditGame.c when copied.
modded class ME_CSH_PreviewEntity
{
 static int s_CSH_TestSignMaterials;
 override protected void SetPreviewObject(VObject mesh, ResourceName material)
 {
  super.SetPreviewObject(mesh, material);
  if (material == CSH_SIGN_MATERIAL && mesh)
   s_CSH_TestSignMaterials++;
 }
}
#endif