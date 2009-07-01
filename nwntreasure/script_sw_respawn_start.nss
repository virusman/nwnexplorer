//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** BEGIN SHROUDWORLD RESPAWN CODE ****
//::
//::///////////////////////////////////////////////////////////////////////////

// Repsawn styles

int RESPAWN_STYLE_NONE          = 0;
int RESPAWN_STYLE_RESPAWN       = 1;
int RESPAWN_STYLE_RESET         = 2;

void GenerateTreasure (int nTreasure, object oA, object oT);
void GeneratePlaceable (int nType, location loc, float fDelay = 0.0f, 
    int nSingleShot = 0, string sProfile = "");
void GenerateEncounter (int nType, location loc, float fDelay = 0.0f, 
    int nSingleShot = 0, string sProfile = "");
int TreasureTableNameToNumber (string sName);
int EncounterTableNameToNumber (string sName);
int PlaceableTableNameToNumber (string sName);

//-----------------------------------------------------------------------------
//
// @func Test for a match with the profile name
//
// @parm string | sName | Name to test
//
// @parm string | sLeft | Left side
//
// @parm string | sRight | Right size
//
// @rdesc TRUE if there is a match
//
//-----------------------------------------------------------------------------

int MatchPartialProfileName (string sName, string sLeft, string sRight)
{
    int nLeft = GetStringLength (sLeft);
    int nRight = GetStringLength (sRight);
    int nName = GetStringLength (sName);
    if (nLeft + nRight > nName)
        return FALSE;
    if (nLeft > 0 && GetStringLeft (sName, nLeft) != sLeft)
        return FALSE;
    if (nRight > 0 && GetStringRight (sName, nRight) != sRight)
        return FALSE;
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// @func Remove all inventory from the container
//
// @parm object | oContainer | Object to be emptied
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void DestroyAllInventory (object oContainer)
{

    //
    // Delete my current treasure (otherwise, it stacks up)
    //

    object oTreasure = GetFirstItemInInventory (oContainer);
    while (GetIsObjectValid (oTreasure))
    {
        DestroyObject (oTreasure);
        oTreasure = GetNextItemInInventory (oContainer);
    }
}

//-----------------------------------------------------------------------------
//
// @func Destroy the container and all of its inventory
//
// @parm object | oContainer | Container to destroy
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void DestroyContainerAndInventory (object oContainer = OBJECT_SELF)
{
    DestroyAllInventory (oContainer);
    DestroyObject (oContainer, 0.0f);
}

//-----------------------------------------------------------------------------
//
// @func Create a new respawn from a template
//
// @parm string | sTemplate | Template name
//
// @parm int | nObjectType | Object type
//
// @parm location | locSpawn | Location to create the container
//
// @parm int | nSingleShot | If true, set the no respawn flag
//
// @parm string | sProfile | Treasure/Respawn profile
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CreateProfileSpawn (string sTemplate, int nObjectType,
    location locSpawn, int nSingleShot, string sProfile)
{

    //
    // The following block will delete any remaining treasure bad under
    // a placeable that is about to respawn
    //

    if (nObjectType == OBJECT_TYPE_PLACEABLE)
    {

        //
        // First step, since GetNearestObjectToLocation doesn't return
        // objects directly at the point, then we must offset the position
        // slightly.
        //

        object oArea = GetAreaFromLocation (locSpawn);
        vector vPosition = GetPositionFromLocation (locSpawn);
        float fFacing = GetFacingFromLocation (locSpawn);
        vPosition = vPosition + Vector (0.1, 0.1, 0.0);
        location locTemp = Location (oArea, vPosition, fFacing);

        //
        // Delete all the placeables directly over me.
        //

        int nNdx = 1;
        object oBag = GetNearestObjectToLocation (
            OBJECT_TYPE_PLACEABLE, locTemp, nNdx);
        float fDistance = 0.0f;
        while (GetIsObjectValid (oBag) && fDistance < 1.0f)
        {
            fDistance = GetDistanceBetweenLocations (
                GetLocation (oBag), locSpawn);
            if (fDistance < 0.1f)
            {
                DestroyContainerAndInventory (oBag);
            }
            nNdx++;
            oBag = GetNearestObjectToLocation (
                OBJECT_TYPE_PLACEABLE, locTemp, nNdx);
        }
    }

    //
    // Create the new container.
    //

    object oNew = CreateObject (nObjectType, sTemplate, locSpawn, FALSE);
    
    //
    // Set the norespawn flag
    //
    
    if (nSingleShot != 0)
        SetLocalInt (oNew, "SW_RS_NORESPAWN", 1);

    //
    // Set the name of the last template used
    //

    if (sProfile != "")
        SetLocalString (oNew, "SW_RS_LAST_PROFILE", sProfile);
}

//-----------------------------------------------------------------------------
//
// @func Create a new respawn from a template
//
// @parm fDelay | Delay time
//
// @parm string | sTemplate | Template name
//
// @parm int | nObjectType | Object type
//
// @parm location | locSpawn | Location to create the container
//
// @parm int | nSingleShot | If true, set the no respawn flag
//
// @parm string | sProfile | Treasure/Respawn profile
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void CreateDelayProfileSpawn (float fDelay, string sTemplate, 
    int nObjectType, location locSpawn, int nSingleShot, 
    string sProfile)
{
    g_nSomethingSpawned = TRUE;
    if (fDelay <= 0.0f)
    {
        CreateProfileSpawn (sTemplate, nObjectType, 
            locSpawn, nSingleShot, sProfile);
    }
    else
    {
        AssignCommand (GetModule (), DelayCommand (fDelay,
            CreateProfileSpawn (sTemplate, nObjectType, 
            locSpawn, nSingleShot, sProfile)));
    }
}

//-----------------------------------------------------------------------------
//
// @func Get the default respawn time
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

int GetDefaultRespawnTime ()
{

    //
    // Get the area respawn time
    //

    object oArea = GetArea (OBJECT_SELF);
    if (GetIsObjectValid (oArea))
    {
        int nRespawn = GetLocalInt (oArea, "SW_RS_RESPAWN_DEFTIME");
        if (nRespawn > 0)
            return nRespawn;
    }

    //
    // Try the module
    //

    object oModule = GetModule ();
    if (GetIsObjectValid (oModule))
    {
        int nRespawn = GetLocalInt (oModule, "SW_RS_RESPAWN_DEFTIME");
        if (nRespawn > 0)
            return nRespawn;
    }

    //
    // Just use a generic respawn time
    //

    return 30;
}

//-----------------------------------------------------------------------------
//
// @func Generate respawn 
//
// @parm int | nRespawnStyle | Style of the respawn
//
// @parm int | nRespawnTime | Time to wait for respawn
//
// @parm string | sRespawnSet | Respawn set
//
// @parm string | sTreaesureSet | Treasure set
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void GenerateProfileRespawn (int nRespawnStyle, int nRespawnTime,
    string sRespawnSet, string sTreasureSet)
{

    //
    // If we are an encounter, then do nothing
    //

    if (GetIsEncounterCreature ())
        return;

    //
    // If the norespawn flag is set, don't
    //

    if (GetLocalInt (OBJECT_SELF, "SW_RS_NORESPAWN"))
        return;
        
    //
    // Get the object type
    //

    int nObjectType = GetObjectType (OBJECT_SELF);

    //
    // If this is an NPC, then test for NPC disabled
    //
    
    if (nObjectType == OBJECT_TYPE_CREATURE)
    {
        object oArea = GetArea (OBJECT_SELF);
        if (GetLocalInt (oArea, "SW_RS_NPC_NORESPAWN") != 0)
            return;
        if (GetLocalInt (oArea, "SW_RS_NPC_RESPAWN") == 0)
        {
            if (GetLocalInt (GetModule (), "SW_RS_NPC_NORESPAWN") != 0)
                return;
        }
    }

    //
    // Do respawn
    //

    if (nRespawnStyle == RESPAWN_STYLE_RESPAWN)
    {

        //
        // Generate the profile name
        //

        string sProfile = sTreasureSet + "__" + sRespawnSet;

        //
        // Get the respawn times
        //

        float fTime = IntToFloat (nRespawnTime);
        if (fTime == 0.0f)
            fTime = IntToFloat (GetDefaultRespawnTime ());
        float fVanishTime = fTime - 2.0f;
        if (fVanishTime < 0.0f)
            fVanishTime = fTime / 2.0f;

        //
        // Get the location
        //

        location locSpawn = GetLocalLocation (
            OBJECT_SELF, "SW_RS_INITIAL_SPAWNPOINT");
        if (!GetIsObjectValid (GetAreaFromLocation (locSpawn)))
            locSpawn = GetLocation (OBJECT_SELF);

        //
        // Create!
        //

        DelayCommand (fVanishTime, DestroyContainerAndInventory ());
        if (nObjectType == OBJECT_TYPE_PLACEABLE)
        {
            int nT = PlaceableTableNameToNumber (sRespawnSet);
            if (nT >= 0)
            {
                GeneratePlaceable (nT, locSpawn, fTime, 0, sProfile);
            }
            else
            {
                CreateDelayProfileSpawn (fTime, sRespawnSet, 
                    nObjectType, locSpawn, 0, sProfile);
            }
        }
        else if (nObjectType == OBJECT_TYPE_CREATURE)
        {
            int nT = EncounterTableNameToNumber (sRespawnSet);
            if (nT >= 0)
            {
                GenerateEncounter (nT, locSpawn, fTime, 0, sProfile);
            }
            else
            {
                CreateDelayProfileSpawn (fTime, sRespawnSet, 
                    nObjectType, locSpawn, 0, sProfile);
            }
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Do death/open processing
//
// @parm object | oOpener | He who opened the container
//
// @parm int | nDamage | Damage (0 for none)
//
// @parm string | sMonster | Monster to spawn or "" for none.
//
// @parm string | sSound | Sound to play when spawning a monster or
//      "" for no sound.
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void GenerateProfileDeathOpen (object oOpener, int nDamage,
    string sMonster, string sSound)
{
    //
    // If we have a monster
    //

    if (sMonster != "")
    {

        //
        // Summons the monster through the Summon() function
        //

        location locSpawn = GetLocation (OBJECT_SELF);
        int nT = EncounterTableNameToNumber (sMonster);
        if (nT >= 0)
        {
            GenerateEncounter (nT, locSpawn, 0.0f, 1, "");
        }
        else
        {
            CreateDelayProfileSpawn (0.0f, sMonster, 
                OBJECT_TYPE_CREATURE, locSpawn, 1, "");
            g_nSomethingSpawned = TRUE;
        }
    }
    else
        g_nSomethingSpawned = TRUE;
    
    //
    // If something was created (if requested)
    //
    
    if (g_nSomethingSpawned != 0)
    {
    
        //
        // Play the sound
        //

        if (sSound != "")
            PlaySound (sSound);
        
        //
        // If we have damage, apply the damage
        //

        if (nDamage)
        {
            ApplyEffectToObject (DURATION_TYPE_INSTANT,
                EffectDamage (nDamage), oOpener);
        }
    }
}

//-----------------------------------------------------------------------------
//
// @func Do treasure generation
//
// @parm string | sTreasureSet | Treasure set
//
// @parm string | sTreasureSetAlias | Alias
//
// @parm object | oA | Attacker
//
// @parm object | oT | Target
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void GenerateProfileTreasure (string sTreasureSet, 
    string sTreasureSetAlias, object oA, object oT)
{
    int nT = -1;
    if (sTreasureSetAlias != "")
        nT = TreasureTableNameToNumber (sTreasureSetAlias);
    if (nT == -1)
        nT = TreasureTableNameToNumber (sTreasureSet);
    if (nT == -1)
    {
        string s = GetLocalString (OBJECT_SELF, "SW_RS_DEFAULT_PROFILE");
        nT = TreasureTableNameToNumber (s);
    }
    GenerateTreasure (nT, oA, oT);
}

//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** END SHROUDWORLD RESPAWN CODE ****
//::
//::///////////////////////////////////////////////////////////////////////////
