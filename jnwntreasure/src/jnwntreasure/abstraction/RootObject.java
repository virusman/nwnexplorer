package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import jnwntreasure.abstraction.enum.TableType;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.Object;

/**
 * TBD
 */
public class RootObject extends Object
{

    /**
     * TBD
     */ 
    int tableType;

    /**
     * TBD
     */
    public RootObject(int tt)
    {
	super(ObjectType.Root);
	tableType = tt;
    }

    /**
     * TBD
     */	
    public String toString()
    {
	return (TableType.tableType[tableType] + " Tables");
    }

    /**
     * TBD
     */ 
    public int getObjectType()
    {
	return ObjectType.Root;
    }

    /**
     * TBD
     */ 
    public int getTableType()
    {
	return tableType;
    }
}
