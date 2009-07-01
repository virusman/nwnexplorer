package jnwntreasure.abstraction;

/**
 * @author Jason Mazzotta
 */
import java.io.BufferedWriter;
import jnwntreasure.abstraction.enum.ObjectImage;
import jnwntreasure.abstraction.enum.ObjectType;
import jnwntreasure.abstraction.enum.RespawnType;
import jnwntreasure.abstraction.TableItemObject;

/** 
 * TBD
 */
public class ProfileObject extends TableItemObject
{
    /** 
     * TBD
     */
    private int respawnType;
    /** 
     * TBD
     */
    private int respawnTime;
    /** 
     * TBD
     */
    private String respawnName;
    /** 
     * TBD
     */
    private String deathOpenSpawn;
    /** 
     * TBD
     */
    private String deathOpenSound;
    /** 
     * TBD
     */
    private int deathOpenDamage;
    /** 
     * TBD
     */
    private String treasureTable;

    /** 
     * TBD
     */
    public ProfileObject()
    {
	respawnType = RespawnType.None;
	respawnTime = 5*60;
	deathOpenDamage = 0;
    }

    /** 
     * TBD
     */
    public boolean writeProfileTest(BufferedWriter bw, boolean first)
    {
	return false;
    }

    /** 
     * TBD
     */
    public void getDescription(String name, int maxText)
    {
       
    }

    /** 
     * TBD
     */
    public int getImage()
    {
	return ObjectImage.Profile;
    }

    /** 
     * TBD
     */
    public String getFullName()
    {
	return "Hello";
    }

    /** 
     * TBD
     */
    public void writeRoutineBody(BufferedWriter bw)
    {
	return;
    }

    /** 
     * TBD
     */
    public int getObjectType()
    {
	return ObjectType.Profile;
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
    public int getRespawnType()
    {
	return respawnType;
    }

    /** 
     * TBD
     */
    public void setRespawnType(int rt)
    {
	respawnType = rt;
    }

    /** 
     * TBD
     */
    public int getRespawnTime()
    {
	return respawnTime;
    }

    /** 
     * TBD
     */
    public void setRespawnTime(int rt)
    {
	respawnTime = rt;
    }

    /** 
     * TBD
     */
    public String getRespawnName()
    {
	return respawnName;
    }

    /** 
     * TBD
     */
    public void setRespawnName(String rn)
    {
	respawnName = rn;
    }

    /** 
     * TBD
     */
    public String getDeathOpenSpawn()
    {
	return deathOpenSpawn;
    }

    /** 
     * TBD
     */
    public void setDeathOpenSpawn(String dos)
    {
	deathOpenSpawn = dos;
    }

    /** 
     * TBD
     */
    public String getDeathOpenSound()
    {
	return deathOpenSound;
    }

    /** 
     * TBD
     */
    public void setDeathOpenSound(String dos)
    {
	deathOpenSound = dos;
    }

    /** 
     * TBD
     */
    public int getDeathOpenDamage()
    {
	return deathOpenDamage;
    }

    /** 
     * TBD
     */
    public void setDeathOpenDamage(int dod)
    {
	deathOpenDamage = dod;
    }

    /** 
     * TBD
     */
    public String getTreasureTable()
    {
	return treasureTable;
    }

    /** 
     * TBD
     */
    public void setTreasureTable (String tt)
    {
	treasureTable = tt;
    }
}
