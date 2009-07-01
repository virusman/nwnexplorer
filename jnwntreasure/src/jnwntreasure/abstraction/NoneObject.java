package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.TableItemObject;
import java.io.BufferedWriter;

/**
 * TBD
 */
public class NoneObject extends TableItemObject
{
    /**
     * TBD
     */
    public NoneObject()
    {
	super.setName("None");
    }

    /**
     * TBD
     */
    public int getImage()
    {
	return ObjectImage.None;
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
	return ObjectType.None;
    }

    /**
     * TBD
     */
    public boolean canEdit()
    {
	return true;
    }
}
