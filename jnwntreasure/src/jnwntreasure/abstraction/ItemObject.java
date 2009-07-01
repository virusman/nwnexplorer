package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.TableItemObject;
import java.io.BufferedWriter;
import java.io.IOException;
import javax.swing.JOptionPane;

/**
 * TBD
 */
public class ItemObject extends TableItemObject
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
    private int strRes;
    /**
     * TBD
     */
    private int index;
    /**
     * TBD
     */
    private int digits;

    /**
     * TBD
     */
    public ItemObject()
    {
	
    }

    /**
     * TBD
     */
    public int getImage()
    {
	return ObjectImage.Item;
    }
    
    /**
     * TBD
     */
    public String getFullName()
    {
	String ret = new String();
	
	return ret;
    }

    /**
     * TBD
     */
    public void writeRoutineBody(BufferedWriter bw)
    {
	try
        {
	    bw.write("Need to add.");
	}
	catch(IOException ioe)
	{
	    JOptionPane.showMessageDialog(null,
			    "Error writing Item Object routine body",
			    "writeRoutineBody error",
			    JOptionPane.ERROR_MESSAGE
			    );

	}
    }

    /**
     * TBD
     */
    public int getObjectType()
    {
	return ObjectType.Item;
    }
    
    /**
     * TBD
     */
    public boolean compare(Object rhs)
    {
	return false;
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
	range = r;
    }
}
