package jnwntreasure.abstraction.enum;

/** 
 * @author Jason Mazzotta
 */

/**
 * Represents a C-style enum.  This one is particularly for the
 * the type of table that is being held in the applications tree.
 */
public class TableType
{
    /**  Strings that describe the table type */
    public static final String[] tableType = {"Treasure", "Encounter",
					"Placeable", "Profile", "Any" }; 

    /**
     * The Treasure Table Type
     */
    public static final int Treasure = 0;
    
    /**
     * The Encounter Table Type 
     */
    public static final int Encounter = 1;
    
    /**
     * The Placeable Table Type
     */
    public static final int Placeable = 2;
    
    /**
     * The Profile Table Type
     */
    public static final int Profile = 3;
    
    /**
     * An anonymous table type
     */
    public static final int Any = 4;

}
