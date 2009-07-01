package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */

import java.io.BufferedWriter;
import java.io.IOException;
import javax.swing.JOptionPane;
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.TableItemObject;

/**
 * TBD 
 */
public class PlaceableObject extends TableItemObject
{
    /**
     * TBD 
     */
    public PlaceableObject()
    {
    }

    /**
     * TBD 
     */
    public int getImage()
    {
	return ObjectImage.Placeable;
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
	if(super.getName().length() != 0)
	{
	    String output = "CreateDelayProfileSpawn (g_fDelay,\"";
		   output += super.getName();
	    output += "\", OBJECT_TYPE_PLACEABLE, g_loc, g_nSingleShot, g_sProfile), \r\n)";

	    try
	    {
		bw.write(output);
	    }
	    catch (IOException ioe)
	    {
		JOptionPane.showMessageDialog(null,
			    "Error writing PlaceableObject routine body",
			    "writeRoutineBody error",
			    JOptionPane.ERROR_MESSAGE
					      );
	    }
	}
    }
			    
    /**
     * TBD 
     */
    public int getObjectType()
    {
	return ObjectType.Placeable;
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
}
