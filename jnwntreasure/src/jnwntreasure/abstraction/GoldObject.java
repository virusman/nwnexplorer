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
public class GoldObject extends TableItemObject
{

    /**
     * TBD
     */
    private int die;
    /**
     * TBD
     */
    private int count;
    /**
     * TBD
     */
    private float multiplier;

    /**
     * TBD
     */
    public GoldObject()
    {
	super.setName("Gold");
	die = 6;
	count = 1;
	multiplier = 1.0f;
    }

    /**
     * TBD
     */
    public int getImage()
    {
	return ObjectImage.Gold;
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
	String output = new String();

	try
	{
	    bw.write("MyCreateGold");
	}
	catch(IOException ioe)
	{
	    JOptionPane.showMessageDialog(null,
			    "Error writing GoldObject routine body.",
			    "writeRoutineBody Error",
			    JOptionPane.ERROR_MESSAGE
			    );
	}
    }

    /**
     * TBD
     */
    public int getObjectType()
    {
	return ObjectType.Gold;
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
    public int getDie()
    {
	return die;
    }

    /**
     * TBD
     */
    public void setDie(int d)
    {
	die = d;
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
    public float getMultiplier()
    {
	return multiplier;
    }

    /**
     * TBD
     */
    public void setMultiplier(float m)
    {
	multiplier = m;
    }

}
