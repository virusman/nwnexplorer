/*
 * TableItemDlgMediator.java
 *
 * Created on May 1, 2004, 3:57 PM
 */

package jnwntreasure.control;

import java.awt.Dimension;
import java.awt.event.ActionListener;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JPanel;

import jnwntreasure.presentation.NonePanel;
import jnwntreasure.abstraction.NoneObject;
import jnwntreasure.presentation.PlaceablePanel;
import jnwntreasure.abstraction.PlaceableObject;
import jnwntreasure.presentation.TableRefPanel;
import jnwntreasure.abstraction.TableRefObject;
import jnwntreasure.presentation.TableItemDlg;
import jnwntreasure.control.JNWNTFrameMediator;
import jnwntreasure.abstraction.TableItemObject;
import jnwntreasure.abstraction.enum.ItemAction;

/**
 *
 * @author  Jason Mazzotta
 */

/**
 * The purpose this class is to handle the events that a TableItemDlg
 * creates and the interaction between a TabldItemDlg and the rest of the
 * application.
 */
public class TableItemDlgMediator implements ActionListener
{
    /** The TableItemObject to be created */
    TableItemObject tio;
    /** The Mediator for the main app */
    JNWNTFrameMediator jnwntfm;
    /** The Dialog through which the TableItemObject will be created */
    TableItemDlg tid;
    /** The model through which we will control the JComboBox for the
     *  TableItemDlg
     */
    DefaultComboBoxModel dcbm;
    /**
     * A reference to the dynamic panel that the JComboBox is 
     * controlling
     */
    JPanel currentPanel;
    /** A hack ( I guess), to make sure that we don't try to remove
     *  widgets that haven't been created yet 
     */
    boolean firstTime;
    
    /** Creates a new instance of TableItemDlgMediator */
    public TableItemDlgMediator(JNWNTFrameMediator j) 
    {
        jnwntfm = j;
        firstTime = true;
	//Create a TableItemDialog
        tid = new TableItemDlg(null, true);
	//Create a model for the JComboBox
        dcbm = new DefaultComboBoxModel();
	//Add items to the JCombBox model
        dcbm.insertElementAt(jnwntreasure.abstraction.enum.ItemAction.ItemAction[jnwntreasure.abstraction.enum.ItemAction.NONE], 
                    dcbm.getSize());
        dcbm.insertElementAt(jnwntreasure.abstraction.enum.ItemAction.ItemAction[jnwntreasure.abstraction.enum.ItemAction.PLACEABLE], 
                    dcbm.getSize());
        dcbm.insertElementAt(jnwntreasure.abstraction.enum.ItemAction.ItemAction[jnwntreasure.abstraction.enum.ItemAction.TABLE_MOVE], 
                    dcbm.getSize());

	//Set listeners for the widgets 
	tid.getItemTypeCombo().addActionListener(this);
        tid.getOKButton().addActionListener(this);
        tid.getCancelButton().addActionListener(this);
	//Set up the JComboBox
        tid.getItemTypeCombo().setModel(dcbm);
        tid.getItemTypeCombo().setEditable(false);        
        tid.getItemTypeCombo().setSelectedIndex(0);
	tid.show();
    }
    
    /**
     * Purpose : The method that gets called when the JComboBox or 
     *           JButtons are selected
     * Method  : React based on which widget was selected
     */
    public void actionPerformed(java.awt.event.ActionEvent ae) 
    {
	   // Close the dialog if the user clicks Cancel
           if(ae.getSource() == tid.getCancelButton())
	   {
	       tid.hide();
	       tid.dispose();
               return;
           }
	   //Construct an item if the user clicks the OK button
           if(ae.getSource() == tid.getOKButton())
               constructItem();
	   //Change the panel in the TableItemDlg if the user
           //Makes a selection from the JComboBox
           if(ae.getSource() == tid.getItemTypeCombo())
               setCurrentPanel();
    }
    
    /**
     * Purpose : Switch the currentPanel depending on the user
     *           selection
     * Method  : Add and remove JPanels to the ContentPane of the
     *           the dialog.
     */
    public void setCurrentPanel()
    {
        java.awt.GridBagConstraints gridBagConstraints;
       
	// If this it the first time the user has made a selection
        // we don't want to remove items that haven't been added yet
        if(firstTime)
        {
            firstTime = false;
        }
	//otherwise remove the currentPanel and the OK and Cancel buttons
        else
        {
            tid.getContentPane().remove(currentPanel);
            tid.getContentPane().remove(tid.getOKButton());
            tid.getContentPane().remove(tid.getCancelButton());
            
        }
	 
	System.out.println(Integer.toString(tid.getItemTypeCombo().getSelectedIndex()));
        
	//Check which item was selected from the JComboBox
        //and create currentPanel accordingly
        switch(tid.getItemTypeCombo().getSelectedIndex())
        {
            case 0:
                currentPanel = new NonePanel();
		initNonePanel();
                break;
            case 1:
                currentPanel = new PlaceablePanel();
		initPlaceablePanel();
                break;
            case 2:
                currentPanel = new TableRefPanel();
		initTableRefPanel();
                break;
        }

	//this forces the encapsulating Dialog to remain the
	//same shape all the time
	currentPanel.setPreferredSize(new Dimension(320, 200));
        
	//add the current panel
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.weightx = 1.0;
        gridBagConstraints.weighty = .8;
        tid.getContentPane().add(currentPanel, gridBagConstraints);
        
	//add the OK button
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.weightx = 0.2;
        gridBagConstraints.weighty = 0.1;
        tid.getContentPane().add(tid.getOKButton(), gridBagConstraints);

	//add the Cancel button
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.weightx = 0.2;
        gridBagConstraints.weighty = 0.1;
        tid.getContentPane().add(tid.getCancelButton(), gridBagConstraints);

	//force the dialog to show the new JPanel
	tid.pack();
	tid.getContentPane().repaint();
    }
	
    /**
     * TBD 
     */
    public void initNonePanel()
    {
	
    }

    /**
     * TBD
     */
    public void initPlaceablePanel()
    {
	
    }

    /**
     * TBD
     */
    public void initTableRefPanel()
    {
	
    }
    
    /**
     * TBD
     */
    public void constructItem()
    {
        
    }
    
    /**
     * TBD   
     * @return jnwntreasure.abstraction.TableItemObject
     */
    public TableItemObject getTableItem()
    {
    	return null;    
    }
}
