package jnwntreasure.abstraction;

/** 
 * @author Jason Mazzotta
 */

import java.io.BufferedWriter;
import jnwntreasure.abstraction.enum.ObjectType;

/**
 * TBD 
 */
public class Object
{
    /**
     * TBD
     */
    private int objectType;
    /**
     * TBD
     */
    private String name;

    /**
     * TBD
     */
    public Object(int ot)
    {
	objectType = ot;	
    }

    /**
     * TBD
     */
    public int getObjectType()
    {
	return objectType;
    }

    /**
     * TBD
     */
    public void setName(String n)
    {
	name = n;
    }

    /**
     * TBD
     */
    public String getName()
    {
	return name;
    }
}
