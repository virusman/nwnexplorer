//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** BEGIN SHROUDWORLD RESPAWN CODE ****
//::
//::///////////////////////////////////////////////////////////////////////////

string g_sRespawnSetAlt;

//-----------------------------------------------------------------------------
//
// @func Get the treasure characteristics
//
// @parm string | sTag | Characteristics tag
//
// @rdesc TRUE if characteristics were found
//
//-----------------------------------------------------------------------------

int RunProfileInt (object oA, object oT, int nGenTreasure, 
    int nGenDeathOpen, int nGenRespawn, string sTag)
{

    //
    // Check for invalid string
    //

    if (sTag == "")
    {
        return FALSE;
    }

    //
    // Break up the parts
    //

    string sTreasureSet;
    string sRespawnSet;
    int nSep = FindSubString (sTag, "__");
    if (nSep >= 0)
    {
        sTreasureSet = GetStringLeft (sTag, nSep);
        sRespawnSet = GetStringRight (sTag, GetStringLength (sTag) - nSep - 2);
    }
    else
    {
        sTreasureSet = sTag;
        sRespawnSet = "";
    }
    sTreasureSet = GetStringLowerCase (sTreasureSet);
    sRespawnSet = GetStringLowerCase (sRespawnSet);

    //
    // Invoke the specific
    //

    int fSuccess = ProfileExecute (oA, oT, sTreasureSet, sRespawnSet,
        g_sRespawnSetAlt, nGenTreasure, nGenDeathOpen, nGenRespawn);
    if (!fSuccess)
    {
        if (sRespawnSet != "" && g_sRespawnSetAlt == "")
            g_sRespawnSetAlt = sRespawnSet;
    }
    return fSuccess;
}

//-----------------------------------------------------------------------------
//
// @func Get the sets for ourselves
//
// @rdesc TRUE if we were able to get a set
//
//-----------------------------------------------------------------------------

int RunProfile (object oA, object oT, int nGenTreasure, 
    int nGenDeathOpen, int nGenRespawn)
{

    //
    // Initialize all the variables
    
    g_sRespawnSetAlt = "";
    
    //
    // Get the treasure information
    //

    int nTrys = 0;
    int fValid;
    string sChars = GetLocalString (OBJECT_SELF, "SW_RS_LAST_PROFILE");
    fValid = RunProfileInt (oA, oT, nGenTreasure, nGenDeathOpen, nGenRespawn, sChars);
    if (!fValid)
    {
        nTrys++;
        sChars = GetTag (OBJECT_SELF);
        fValid = RunProfileInt (oA, oT, nGenTreasure, 
            nGenDeathOpen, nGenRespawn, sChars);
    }
    if (!fValid)
    {
        nTrys++;
        sChars = GetLocalString (OBJECT_SELF, "SW_RS_DEFAULT_PROFILE");
        fValid = RunProfileInt (oA, oT, nGenTreasure, 
            nGenDeathOpen, nGenRespawn, sChars);
    }
    if (!fValid)
    {
        nTrys++;
        if (GetObjectType (OBJECT_SELF) == OBJECT_TYPE_CREATURE)
            sChars = "nw_npc_low";
        else
            sChars = "nw_low";
        fValid = RunProfileInt (oA, oT, nGenTreasure, 
            nGenDeathOpen, nGenRespawn, sChars);
    }
    return fValid;
}

//-----------------------------------------------------------------------------
//
// @func Handle the opening or death of a container
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void HandleOpenDeath ()
{
    //PrintString ("OpenDeath");

    //
    // Handle the once trigger
    //

    if (GetLocalInt (OBJECT_SELF, "NW_DO_ONCE") != 0)
        return;
    object oLastOpener = GetLastOpener ();
    if (GetIsObjectValid (oLastOpener) == FALSE)
        return;
    SetLocalInt (OBJECT_SELF, "NW_DO_ONCE", 1);

    //
    // Notify everyone I am being opened
    //

    ShoutDisturbed ();

    //
    // Get my sets
    //

    RunProfile (oLastOpener, OBJECT_SELF, TRUE, TRUE, TRUE);
}

//-----------------------------------------------------------------------------
//
// @func Handle the death of an NPC
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void HandleNPCDeath ()
{
    //PrintString ("NPCDeath");

    //
    // Get my sets
    //

    RunProfile (OBJECT_SELF, OBJECT_SELF, FALSE, TRUE, TRUE);
}

//-----------------------------------------------------------------------------
//
// @func Handle the spawn of an NPC
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void HandleNPCSpawn ()
{
    //PrintString ("NPCSpawn");

    //
    // Remember my current location for later respawn
    //

    SetLocalLocation (OBJECT_SELF, "SW_RS_INITIAL_SPAWNPOINT",
        GetLocation (OBJECT_SELF));

    //
    // Get my sets
    //

    RunProfile (OBJECT_SELF, OBJECT_SELF, TRUE, FALSE, FALSE);
}

//-----------------------------------------------------------------------------
//
// @func Handle the generation of treasure
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void HandleGenerateTreasure ()
{
    //PrintString ("GenerateTreasure");

    //
    // Get the information
    //
    
    string sTreasure = GetLocalString (OBJECT_SELF, "SW_RS_GT_TREASURE");
    object oA = GetLocalObject (OBJECT_SELF, "SW_RS_GT_ATTACKER");
    object oT = GetLocalObject (OBJECT_SELF, "SW_RS_GT_TARGET");
    
    //
    // Convert to a number
    //
    
    int nTreasure = TreasureTableNameToNumber (sTreasure);
    GenerateTreasure (nTreasure, oA, oT);
    return;
}

int SW_RS_REQUEST_OPENDEATH     = 1;
int SW_RS_REQUEST_NPCDEATH      = 2;
int SW_RS_REQUEST_NPCSPAWN      = 3;
int SW_RS_REQUEST_GENERATE      = 4;

//-----------------------------------------------------------------------------
//
// @func Handle a general treasure request
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void HandleTreasureRequest ()
{
    int nRequest = GetLocalInt (OBJECT_SELF, "SW_RS_REQUEST");
    DeleteLocalInt (OBJECT_SELF, "SW_RS_REQUEST");
    if (nRequest == SW_RS_REQUEST_OPENDEATH)
    {
        HandleOpenDeath ();
    }
    else
    if (nRequest == SW_RS_REQUEST_NPCDEATH)
    {
        HandleNPCDeath ();
    }
    else
    if (nRequest == SW_RS_REQUEST_NPCSPAWN)
    {
        HandleNPCSpawn ();
    }
    else
    if (nRequest == SW_RS_REQUEST_GENERATE)
    {
        HandleGenerateTreasure ();
    }
}

//-----------------------------------------------------------------------------
//
// @func Main execution routine
//
// @rdesc None.
//
//-----------------------------------------------------------------------------

void main ()
{
    HandleTreasureRequest ();
}

//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** END SHROUDWORLD RESPAWN CODE ****
//::
//::///////////////////////////////////////////////////////////////////////////
