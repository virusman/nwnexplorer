package jnwntreasure.abstraction;

/** 
 * @author Jason Mazzotta
 */
import javax.swing.JOptionPane;
import java.io.BufferedWriter;
import java.io.IOException;
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.TableItemObject;

/**
 * TBD
 */
public class EncounterObject extends TableItemObject
{
    
    /**
     * TBD
     */
    protected int count;
    /**
     * TBD
     */
    protected int range; 

    /**
     * TBD
     */
    public EncounterObject()
    {
	count =1;
	range = 0;
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

    /**
     * TBD
     */
    public int GetImage()
    {
	return ObjectImage.Encounter; 
    }

    /**
     * TBD
     */
    public void writeRoutineBody(BufferedWriter bw)
    {
	if(super.getName().length() != 0 )
	{
	   String output = "CreateDelayProfileSpawn (g_fDelay, \"";
	   output+=super.getName();
	   output+="\", OBJECT_TYPE_CREATURE, g_loc, g_nSingleShot, g_sProfile:\r\n";
	  
	   try
	   { 
	   	bw.write(output, 0, output.length());
	   }
	   catch(IOException ioe)
	   {
		JOptionPane.showMessageDialog(null, 
				"Error writing EncounterObject routine body.",
				"writeRoutineBody Error",
				JOptionPane.ERROR_MESSAGE
				);
	   }
	}
    }
}
