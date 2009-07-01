//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** BEGIN BIOWARE CODE ****
//::
//:: Copyright (c) 2001-2002 Bioware Corp.
//::
//::///////////////////////////////////////////////////////////////////////////

// * Returns the object that either last opened the container or destroyed it

object GetLastOpener()
{
    if (GetIsObjectValid(GetLastOpenedBy()) == TRUE)
    {
        dbSpeak("LastOpener: GetLastOpenedBy " + GetTag(GetLastOpenedBy()));
        return GetLastOpenedBy();
    }
    else
    if (GetIsObjectValid(GetLastKiller()) == TRUE)
    {
        dbSpeak("LastOpener: GetLastAttacker");
        return GetLastKiller();
    }
    dbSpeak("LastOpener: The Object is Invalid you weenie!");
    return OBJECT_INVALID;
}

// * Container shouts if disturbed
void ShoutDisturbed()
{
    if (GetIsDead(OBJECT_SELF) == TRUE)
    {
        object oTarget = GetFirstObjectInShape(SHAPE_SPHERE, RADIUS_SIZE_LARGE, 
            GetLocation(OBJECT_SELF), TRUE, OBJECT_TYPE_CREATURE);
        //Cycle through the targets within the spell shape until an invalid object is captured.
        while (GetIsObjectValid(oTarget))
        {
            if (GetFactionEqual(oTarget, OBJECT_SELF) == TRUE)
            {
                // * Make anyone who is a member of my faction hostile if I am violated
                object oAttacker = GetLastAttacker();
                SetIsTemporaryEnemy(oAttacker,oTarget);
                AssignCommand(oTarget, ActionAttack(oAttacker));
            }
            oTarget = GetNextObjectInShape(SHAPE_SPHERE, RADIUS_SIZE_LARGE, 
                GetLocation(OBJECT_SELF), TRUE, OBJECT_TYPE_CREATURE);
        }
    }
    else (GetIsOpen(OBJECT_SELF) == TRUE);
    {
        object oTarget = GetFirstObjectInShape(SHAPE_SPHERE, RADIUS_SIZE_LARGE, 
            GetLocation(OBJECT_SELF), TRUE, OBJECT_TYPE_CREATURE);
        //Cycle through the targets within the spell shape until an invalid object is captured.
        while (GetIsObjectValid(oTarget))
        {
            if (GetFactionEqual(oTarget, OBJECT_SELF) == TRUE)
            {
                // * Make anyone who is a member of my faction hostile if I am violated
                object oAttacker = GetLastOpener();
                SetIsTemporaryEnemy(oAttacker,oTarget);
                AssignCommand(oTarget, ActionAttack(oAttacker));

            }
            oTarget = GetNextObjectInShape(SHAPE_SPHERE, RADIUS_SIZE_LARGE, 
                GetLocation(OBJECT_SELF), TRUE, OBJECT_TYPE_CREATURE);
        }
    }
}

void GenerateLowTreasure(object oLastOpener, object oCreateOn=OBJECT_INVALID)
{
    GenerateTreasure(TREASURE_LOW, oLastOpener, oCreateOn);
}
void GenerateMediumTreasure(object oLastOpener, object oCreateOn=OBJECT_INVALID)
{
    GenerateTreasure(TREASURE_MEDIUM, oLastOpener, oCreateOn);
}
void GenerateHighTreasure(object oLastOpener, object oCreateOn=OBJECT_INVALID)
{
    GenerateTreasure(TREASURE_HIGH, oLastOpener, oCreateOn);
}
void GenerateBossTreasure(object oLastOpener, object oCreateOn=OBJECT_INVALID)
{
    GenerateTreasure(TREASURE_BOSS, oLastOpener, oCreateOn);
}
void GenerateBookTreasure(object oLastOpener, object oCreateOn=OBJECT_INVALID)
{
    GenerateTreasure(TREASURE_BOOK, oLastOpener, oCreateOn);
}

//::///////////////////////////////////////////////////////////////////////////
//:: 
//:: **** END BIOWARE CODE ****
//::
//:: Copyright (c) 2001-2002 Bioware Corp.
//::
//::///////////////////////////////////////////////////////////////////////////

