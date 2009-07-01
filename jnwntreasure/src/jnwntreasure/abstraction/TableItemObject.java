package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */

import jnwntreasure.abstraction.Object;
import jnwntreasure.abstraction.enum.ObjectType;
/**
 * TBD
 */
public class TableItemObject extends jnwntreasure.abstraction.Object
{
    /**
     * TBD
     */
    private TableObject table;
    /**
     * TBD
     */
    private int chance;

    /**
     * TBD
     */
    public TableItemObject()
    {
	super(ObjectType.Item);
	chance = 0;
	table = null;
    }

    /**
     * TBD
     */
    public void setDefaultChance()
    {
	if(table == null)
	    return;

	chance = table.getDefaultChance();
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
    public TableObject getTable()
    {
	return table;
    }

    /**
     * TBD
     */
    public void setTable(TableObject t)
    {
	table = t;
    }

    /**
     * TBD
     */
    public int getChance()
    {
	return chance;
    }

    /**
     * TBD
     */
    public void setChance(int c)
    {
	chance = c;
    }
}
