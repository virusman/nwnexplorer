package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import java.io.BufferedWriter;
import jnwntreasure.abstraction.Object;
import jnwntreasure.abstraction.enum.TableType;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.enum.TableCommand;

/**
 * This class represents the main tables from which a script is built.
 */
public class TableObject extends jnwntreasure.abstraction.Object
{

    /**
     * This might not be necessary in Java.
     */
    private int tableRefCount;
    /** 
     * The command that this table represents
     */
    private int tableCommand;
    /**
     * Represents whether this table is global or not
     */
    private boolean global;
    /**
     * A unique identifier for a global table
     */
    private int globalID;
    /**
     * The type of table: Placeable, Treasure, Profile, Encounter
     */
    private int tableType;
    /**
     * This might not be necessary in Java.
     */
    private int refCount;
    
    /**
     * Purpose : Constructor
     * Method  : initialize member variables
     */
    public TableObject()
    {
	super(ObjectType.Table);
	global = false;
    }
    /**
     * TBD
     */
    public int getDefaultChance()
    {
	return 0;
    }

    /**
     * TBD
     */
    public boolean writeProfileTest(BufferedWriter bw)
    {
	return false;
    }

    /**
     * TBD
     */
    public String toString()
    {
	return (super.getName() + " - "+ TableCommand.tableCommand[tableCommand]);
    }

    /**
     * TBD
     */
    public void getDescription()
    {
    }

    /**
     * TBD
     */
    public int getImage()
    {
	return 0;
    }

    /**
     * TBD
     */
    public void computeRefCounts()
    {
    }

    /**
     * TBD
     */
    public void writeRoutineBody(BufferedWriter bw)
    {
    }

    /**
     * TBD
     */
    public int getObjectType()
    {
	return ObjectType.Table;
    }
    /**
     * TBD
     */
    public int getTableCommand()
    {
	return tableCommand;
    }
    /**
     * TBD
     */
    public void setTableCommand(int tc)
    {
	tableCommand = tc;
    }

    /**
     * TBD
     */
    public boolean getGlobal()
    {
	return global;
    }

    /**
     * TBD
     */
    public void setGlobal(boolean g)
    {
	global = g;
    }

    /**
     * TBD
     */
    public int getTableType()
    {
	return tableType;
    }

    /**
     * TBD
     */
    public void setTableType(int tt)
    {
	tableType = tt;
    }

    /**
     * TBD
     */
    public int getGlobalID()
    {
	return globalID;
    }

    /**
     * TBD
     */
    public void setGlobalID(int g)
    {
	globalID = g;
    }

    /**
     * TBD
     */
    public void addRef()
    {
	refCount++;
    }

    /**
     * TBD
     */
    public void releaseRef()
    {
	refCount--;
    }

    /**
     * TBD
     */
    public int getRefCount()
    {
	return refCount;
    }

    /**
     * TBD
     */
    public void writeRoutineBodyList()
    {
    }

}
