#ifdef WORKBENCH
modded class SCR_CampaignMilitaryBaseComponent
{
 protected int m_CSH_TestMeshes;
 protected int m_CSH_TestPhysics;
 protected int m_CSH_TestFlags;
 protected void CSH_TestVisit(IEntity entity)
 {
  if (entity.GetVObject()) m_CSH_TestMeshes++;
  if (entity.GetPhysics()) m_CSH_TestPhysics++;
  if ((entity.GetFlags() & EntityFlags.TRACEABLE) || !(entity.GetFlags() & EntityFlags.EDITOR_ONLY)) m_CSH_TestFlags++;
  IEntity child = entity.GetChildren();
  while (child) { CSH_TestVisit(child); child = child.GetSibling(); }
 }
 string CSH_TestSnapshot(IEntity owner)
 {
  m_CSH_TestMeshes = 0;
  m_CSH_TestPhysics = 0;
  m_CSH_TestFlags = 0;
  int badTransform;
  vector expected[4];
  Math3D.AnglesToMatrix(owner.GetYawPitchRoll(), expected);
  expected[3] = owner.GetOrigin();
  foreach (SCR_BasePreviewEntity preview : m_CSH_Previews)
  {
   CSH_TestVisit(preview);
   vector actual[4];
   preview.GetWorldTransform(actual);
   for (int i = 0; i < 4; i++)
    if (vector.Distance(expected[i], actual[i]) > 0.001) badTransform++;
  }
  return string.Format("meshes=%1,physics=%2,badFlags=%3,badTransform=%4", m_CSH_TestMeshes, m_CSH_TestPhysics, m_CSH_TestFlags, badTransform);
 }
}
#endif