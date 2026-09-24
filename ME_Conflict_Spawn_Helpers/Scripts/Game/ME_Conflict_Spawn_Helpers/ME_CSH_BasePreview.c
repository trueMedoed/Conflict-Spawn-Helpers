#ifdef WORKBENCH
//! Editor-only preview. No vanilla spawn, faction, or initialization methods are overridden.
modded class SCR_CampaignMilitaryBaseComponent
{
 protected static ref array<SCR_CampaignMilitaryBaseComponent> s_CSH_Bases = {};
 static bool s_CSH_Enabled = true;
 static string s_CSH_FactionFilter;
 protected string m_CSH_Warnings;
 protected ref array<SCR_BasePreviewEntity> m_CSH_Previews = {};
 protected ref array<ResourceName> m_CSH_Prefabs = {};
 protected string m_CSH_Caption;
 protected float m_CSH_RefreshTimer;
 protected bool m_CSH_Dirty = true;
 protected bool m_CSH_Overview;
 protected bool m_CSH_HasBounds;
 protected vector m_CSH_Min;
 protected vector m_CSH_Max;

 static void CSH_ClearAll()
 {
  foreach (SCR_CampaignMilitaryBaseComponent base : s_CSH_Bases)
  {
   if (base)
   {
    base.CSH_Clear();
    base.m_CSH_Dirty = true;
   }
  }
 }

 static void CSH_Toggle()
 {
  s_CSH_Enabled = !s_CSH_Enabled;
  CSH_ClearAll();
  PrintFormat("[ME_CSH] Preview enabled: %1", s_CSH_Enabled);
 }

 static void CSH_CycleFaction()
 {
  switch (s_CSH_FactionFilter)
  {
   case "": s_CSH_FactionFilter = "US"; break;
   case "US": s_CSH_FactionFilter = "USSR"; break;
   case "USSR": s_CSH_FactionFilter = "FIA"; break;
   default: s_CSH_FactionFilter = ""; break;
  }
  CSH_ClearAll();
  PrintFormat("[ME_CSH] Planning faction filter: '%1' (empty = automatic)", s_CSH_FactionFilter);
 }

 int CSH_GetPreviewCount()
 {
  return m_CSH_Previews.Count();
 }

 void CSH_Clear()
 {
  foreach (SCR_BasePreviewEntity preview : m_CSH_Previews)
  {
   if (preview)
    delete preview; // SCR_BasePreviewEntity destructor deletes its preview children.
  }
  m_CSH_Previews.Clear();
  m_CSH_Prefabs.Clear();
  m_CSH_HasBounds = false;
 }

 protected void CSH_AddFaction(SCR_CampaignFaction faction, EEditableEntityLabel label, array<ResourceName> prefabs)
 {
  if (!faction)
   return;
  ResourceName prefab = faction.GetBuildingPrefab(label);
  if (!prefab.IsEmpty() && !prefabs.Contains(prefab))
   prefabs.Insert(prefab);
 }

 //! Read the current world's factions; never initialize gameplay or change affiliation.
 protected void CSH_Resolve(IEntity owner, array<ResourceName> prefabs)
 {
  EEditableEntityLabel label;
  switch (GetType())
  {
   case SCR_ECampaignBaseType.BASE: label = EEditableEntityLabel.SERVICE_HQ; break;
   case SCR_ECampaignBaseType.SOURCE_BASE: label = EEditableEntityLabel.SERVICE_SOURCE_BASE; break;
   default: m_CSH_Caption = "Conflict: no automatic base building for this type"; return;
  }

  FactionManager manager = GetGame().GetFactionManager();
  if (!s_CSH_FactionFilter.IsEmpty())
  {
   if (manager)
    CSH_AddFaction(SCR_CampaignFaction.Cast(manager.GetFactionByKey(s_CSH_FactionFilter)), label, prefabs);
   m_CSH_Caption = string.Format("Conflict: planning variant %1 (not a spawn prediction)", s_CSH_FactionFilter);
   if (prefabs.IsEmpty())
    m_CSH_Caption += " / no configured composition";
   return;
  }

  SCR_FactionAffiliationComponent affiliation = SCR_FactionAffiliationComponent.Cast(owner.FindComponent(SCR_FactionAffiliationComponent));
  SCR_CampaignFaction assigned;
  if (affiliation)
   assigned = SCR_CampaignFaction.Cast(affiliation.GetDefaultAffiliatedFaction());

  // Mirror InitializeBases: an unassigned ordinary base starts with the
  // campaign's INDFOR faction. Read the mission mapping, never assume FIA.
  SCR_GameModeCampaign campaign = SCR_GameModeCampaign.GetInstance();
  if (!CanBeHQ())
  {
   if (!assigned && campaign)
    assigned = campaign.GetFactionByEnum(SCR_ECampaignFaction.INDFOR);
   CSH_AddFaction(assigned, label, prefabs);
  }
  if (!prefabs.IsEmpty())
  {
   m_CSH_Caption = string.Format("Conflict: %1 composition preview", assigned.GetFactionKey());
   return;
  }

  if (manager)
  {
   array<Faction> factions = {};
   manager.GetFactionsList(factions);
   foreach (Faction faction : factions)
    CSH_AddFaction(SCR_CampaignFaction.Cast(faction), label, prefabs);
  }
  if (prefabs.IsEmpty())
   m_CSH_Caption = "Conflict: preview unavailable - add a campaign FactionManager";
  else
   m_CSH_Caption = string.Format("Conflict: %1 possible composition(s), ownership not resolved", prefabs.Count());
  if (CanBeHQ() && !prefabs.IsEmpty())
   m_CSH_Caption = "HQ: largest preview + all-variant bounds; final faction may need less space";
  if (CanBeHQ() && DisableWhenUnusedAsHQ())
   m_CSH_Caption += " / only if selected as HQ";
 }

 //! Accumulate mesh corners in the marker's unscaled local frame.
 protected void CSH_Measure(IEntity entity, IEntity root, inout bool found, inout vector mins, inout vector maxs)
 {
  if (entity.GetVObject())
  {
   vector low, high;
   entity.GetBounds(low, high);
   vector matrix[4];
   entity.GetWorldTransform(matrix);
   for (int i = 0; i < 8; i++)
   {
    vector corner = low;
    if (i & 1) corner[0] = high[0];
    if (i & 2) corner[1] = high[1];
    if (i & 4) corner[2] = high[2];
    corner = root.CoordToLocal(corner.Multiply4(matrix));
    if (!found)
    {
     mins = corner;
     maxs = corner;
     found = true;
    }
    else
    {
     for (int axis = 0; axis < 3; axis++)
     {
      mins[axis] = Math.Min(mins[axis], corner[axis]);
      maxs[axis] = Math.Max(maxs[axis], corner[axis]);
     }
    }
   }
  }
  IEntity child = entity.GetChildren();
  while (child)
  {
   CSH_Measure(child, root, found, mins, maxs);
   child = child.GetSibling();
  }
 }

 protected void CSH_DrawBounds(vector transform[4])
 {
  if (!m_CSH_Overview || !m_CSH_HasBounds)
   return;
  vector corners[4];
  corners[0] = Vector(m_CSH_Min[0], 0.15, m_CSH_Min[2]);
  corners[1] = Vector(m_CSH_Max[0], 0.15, m_CSH_Min[2]);
  corners[2] = Vector(m_CSH_Max[0], 0.15, m_CSH_Max[2]);
  corners[3] = Vector(m_CSH_Min[0], 0.15, m_CSH_Max[2]);
  for (int i = 0; i < 4; i++)
   corners[i] = corners[i].Multiply4(transform);
  Shape.CreateLinesLoop(Color.YELLOW, ShapeFlags.ONCE | ShapeFlags.NOZBUFFER, corners, 4, 2);
 }

 protected void CSH_Rebuild(IEntity owner)
 {
  array<ResourceName> prefabs = {};
  CSH_Resolve(owner, prefabs);
  bool overview = CanBeHQ() && s_CSH_FactionFilter.IsEmpty();
  bool changed = m_CSH_Dirty || overview != m_CSH_Overview || prefabs.Count() != m_CSH_Prefabs.Count();
  if (!changed)
  {
   foreach (int index, ResourceName prefab : prefabs)
   {
    if (prefab != m_CSH_Prefabs[index])
     changed = true;
   }
  }
  if (!changed)
   return;

  CSH_Clear();
  m_CSH_Dirty = false;
  m_CSH_Overview = overview;
  SCR_BasePreviewEntity largest;
  float largestArea = -1;
  m_CSH_Warnings = "";
  foreach (ResourceName prefab : prefabs)
  {
   m_CSH_Prefabs.Insert(prefab);
   Resource resource = Resource.Load(prefab);
   if (!resource || !resource.IsValid())
   {
    m_CSH_Warnings += " / missing prefab";
    continue;
   }
   IEntitySource source = SCR_BaseContainerTools.FindEntitySource(resource);
   if (!source)
    continue;
   array<ref SCR_BasePreviewEntry> entries = {};
   SCR_PrefabPreviewEntity.GetPreviewEntries(source, entries);
   if (entries.IsEmpty())
    continue;

   // Never instantiate a prefab supplied as a custom preview. Only plain mesh entries.
   // Unsupported entries are retained as transform parents, with their visual disabled.
   bool partial;
   foreach (SCR_BasePreviewEntry entry : entries)
   {
    if (entry.m_Shape == EPreviewEntityShape.PREFAB)
    {
     entry.m_Shape = EPreviewEntityShape.MESH;
     entry.m_Mesh = ResourceName.Empty;
     partial = true;
    }
   }
   EntitySpawnParams params = new EntitySpawnParams();
   params.TransformMode = ETransformMode.WORLD;
   Math3D.AnglesToMatrix(owner.GetYawPitchRoll(), params.Transform);
   params.Transform[3] = owner.GetOrigin();
   SCR_BasePreviewEntity preview = SCR_BasePreviewEntity.SpawnPreview(entries, "SCR_BasePreviewEntity", owner.GetWorld(), params, "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat", EPreviewEntityFlag.IGNORE_TERRAIN);
   if (!preview)
   {
    m_CSH_Warnings += " / preview failed";
    continue;
   }
   preview.SetFlags(EntityFlags.EDITOR_ONLY, true);
   preview.ClearFlags(EntityFlags.TRACEABLE, true);
   if (overview)
   {
    bool found;
    vector mins, maxs;
    preview.Update();
    CSH_Measure(preview, preview, found, mins, maxs);
    if (found)
    {
     if (!m_CSH_HasBounds)
     {
      m_CSH_Min = mins;
      m_CSH_Max = maxs;
      m_CSH_HasBounds = true;
     }
     else
     {
      for (int axis = 0; axis < 3; axis++)
      {
       m_CSH_Min[axis] = Math.Min(m_CSH_Min[axis], mins[axis]);
       m_CSH_Max[axis] = Math.Max(m_CSH_Max[axis], maxs[axis]);
      }
     }
     float area = (maxs[0] - mins[0]) * (maxs[2] - mins[2]);
     if (area > largestArea)
     {
      if (largest) delete largest;
      largest = preview;
      largestArea = area;
     }
     else
      delete preview;
    }
    else
    {
     delete preview;
     m_CSH_Warnings += " / variant bounds unavailable";
    }
   }
   else
    m_CSH_Previews.Insert(preview);
   if (partial)
    m_CSH_Warnings += " / partial: custom preview omitted";
   PrintFormat("[ME_CSH] %1: %2 (%3 entries)", owner.GetOrigin(), prefab, entries.Count());
  }
  if (largest)
   m_CSH_Previews.Insert(largest);
 }

 override void _WB_OnInit(IEntity owner, inout vector mat[4], IEntitySource src)
 {
  super._WB_OnInit(owner, mat, src);
  if (!s_CSH_Bases.Contains(this))
   s_CSH_Bases.Insert(this);
  m_CSH_Dirty = true;
 }

 override int _WB_GetAfterWorldUpdateSpecs(IEntity owner, IEntitySource src)
 {
  return super._WB_GetAfterWorldUpdateSpecs(owner, src) | EEntityFrameUpdateSpecs.CALL_WHEN_ENTITY_VISIBLE;
 }

 override void _WB_AfterWorldUpdate(IEntity owner, float timeSlice)
 {
  super._WB_AfterWorldUpdate(owner, timeSlice);
  if (!SCR_Global.IsEditMode() || !s_CSH_Enabled)
  {
   if (!m_CSH_Previews.IsEmpty())
    CSH_Clear();
   return;
  }
  m_CSH_RefreshTimer += timeSlice;
  if (m_CSH_Dirty || m_CSH_RefreshTimer >= 1)
  {
   m_CSH_RefreshTimer = 0;
   CSH_Rebuild(owner);
  }
  vector transform[4];
  // Runtime SpawnBuilding uses position and yaw/pitch/roll, not marker scale.
  Math3D.AnglesToMatrix(owner.GetYawPitchRoll(), transform);
  transform[3] = owner.GetOrigin();
  foreach (SCR_BasePreviewEntity preview : m_CSH_Previews)
  {
   if (preview)
   {
    preview.SetWorldTransform(transform);
    preview.Update();
   }
  }
  CSH_DrawBounds(transform);
  vector position = owner.GetOrigin() + Vector(0, 3, 0);
  DebugTextWorldSpace.Create(owner.GetWorld(), m_CSH_Caption + m_CSH_Warnings + " / terrain alignment not simulated", DebugTextFlags.ONCE | DebugTextFlags.CENTER, position[0], position[1], position[2], 12, Color.YELLOW, Color.BLACK);
 }

 override void _WB_OnDelete(IEntity owner, IEntitySource src)
 {
  CSH_Clear();
  s_CSH_Bases.RemoveItem(this);
  super._WB_OnDelete(owner, src);
 }

 override void OnDelete(IEntity owner)
 {
  CSH_Clear();
  s_CSH_Bases.RemoveItem(this);
  super.OnDelete(owner);
 }
}
#endif
