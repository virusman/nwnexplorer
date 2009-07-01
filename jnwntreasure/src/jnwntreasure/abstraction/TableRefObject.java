package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import jnwntreasure.abstraction.TableObject;
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.TableItemObject;

import java.io.BufferedWriter;

/**
 * TBD
 */
public class TableRefObject extends TableItemObject
{
/**
 * TBD
 */
    private int count;
/**
 * TBD
 */
    private int range;
/**
 * TBD
 */
    private int modifier;
/**
 * TBD
 */
    private int specific;

/**
 * TBD
 */
    public TableRefObject()
    {
	count = 1;
	range = 0;
	modifier = -1;
	specific = -1;
    }

/**
 * TBD
 */
    public TableObject getTableObject()
    {
	return null;
    }

/**
 * TBD
 */
    public int getImage()
    {
	return ObjectImage.Table;
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
    public String getFullName()
    {
	return super.getName();
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
	return ObjectType.Table_Ref;
    }

/**
 * TBD
 */
    public boolean compare(Object rhs)
    {
	if(rhs.getObjectType() != ObjectType.Table_Ref)
	    return false;

	TableRefObject SRhs = (TableRefObject)rhs;

	if(super.getName().equals(SRhs.getName()))
	    return false;
	
	return (count == SRhs.getCount() &&
		range == SRhs.getRange() &&
		modifier == SRhs.getModifier() &&
		specific == SRhs.getSpecific());
	    
    }

/**
 * TBD
 */
    public boolean canEdit()
    {
	return true;
    }

/**
 * TBD
 */
    public boolean canTableEdit()
    {
	return true;
    }

/**
 * TBD
 */
    public boolean canNew()
    {
	return true;
    }

/**
 * TBD
 */
    public void changeTableName(TableObject table,
				String oldName, String newName)
    {
	if(super.getName().equals(oldName))
	    super.setName(newName);
    }

/**
 * TBD
 */
    public void collectUsedTables()
    {
    }

/**
 * TBD
 */
    public TableObject getTableRef()
    {
	return null;
    }

/**
 * TBD
 */
    public boolean delete()
    {
	return false;
    }

/**
 * TBD
 */
    public int getCount()
    {
	return count;
    }

/**
 * TBD
 */
    public void setCount(int c)
    {
	count = c;
    }

/**
 * TBD
 */
    public int getRange()
    {
	return range;
    }

/**
 * TBD
 */
    public void setRange(int r)
    {
	range =r;
    }

/**
 * TBD
 */
    public int getModifier()
    {
	return modifier;
    }

/**
 * TBD
 */
    public void setModifier(int m)
    {
	modifier = m;
    }

/**
 * TBD
 */
    public int getSpecific()
    {
	return specific;
    }

/**
 * TBD
 */
    public void setSpecific(int s)
    {
	specific = s;
    }

/**
 * TBD
 */
    public void writeRoutineBodyCall(BufferedWriter bw,
				     TableObject table)
    {
	
    }
}
