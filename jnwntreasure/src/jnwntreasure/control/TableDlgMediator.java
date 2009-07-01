/*
 * TableDlgMediator.java
 *
 * Created on April 7, 2004, 10:57 PM
 */

package jnwntreasure.control;

import java.awt.event.ActionListener;
import java.awt.event.ItemListener;
import java.awt.event.ActionEvent;
import java.awt.event.ItemEvent;
import javax.swing.JOptionPane;
import javax.swing.SpinnerNumberModel;
import javax.swing.DefaultComboBoxModel;

import jnwntreasure.abstraction.TableObject;
import jnwntreasure.abstraction.enum.TableCommand;
import jnwntreasure.abstraction.enum.TableType;
import jnwntreasure.control.JNWNTFrameMediator;
import jnwntreasure.presentation.TableDlg;

/**
 *
 * @author  jasonmaz
 */

/**
 * Purpose : To handle the functioning of a TableDlg and the 
 *           interaction between it and the rest of the app
 * Method  : Be the EventListener for the events that the TableDlg
 *           creates.
 */
public class TableDlgMediator implements ActionListener, ItemListener
{
    /** The TableDlg for which this class is the Mediator */
    private TableDlg tableDlg;
    /** The TableObject that this Mediator will help to create */
    private TableObject tableObj;
    /** 
     *  The JNWNTFrameMediator that will provide services to help
     *  complete the TableObject
     */ 
    private JNWNTFrameMediator jnwntfm;
    
    /** Creates a new instance of TableDlgMediator */
    public TableDlgMediator(JNWNTFrameMediator j) 
    {
	// A model through which we can control the JSpinner in the
        // the TableDlg
	SpinnerNumberModel snm = new SpinnerNumberModel(new Integer(100), 
						        new Integer(100), 		
							new Integer(1000), 			
							new Integer(1));
	
	
	// A model through which we can control the JComboBox in the
	// the TableDlg
	DefaultComboBoxModel dcbm = new DefaultComboBoxModel();
	// Add items to the the JComboBox   
	dcbm.insertElementAt(TableCommand.tableCommand[1], 0);
	dcbm.insertElementAt(TableCommand.tableCommand[2], 1);
	
	jnwntfm = j;
	// Create a TableDlg to create a TableObject 
        tableDlg = new TableDlg(null, true);
	// Set the model for the JSpinner   
        tableDlg.getTabNumSpinner().setModel(snm);
        // Set the model for the JComboBox 
	tableDlg.getModeComboBox().setModel(dcbm);
        tableDlg.getModeComboBox().setSelectedIndex(0);
	// Add EventListeners for the widgets in the Dialog
        tableDlg.getOK().addActionListener(this);
        tableDlg.getCancel().addActionListener(this);
        tableDlg.getGlobalTabCheck().addItemListener(this);
	// Set the enabled states for Table Number JSpinner 
        tableDlg.getTabNumSpinner().setEnabled(false);
        tableDlg.getTabNumLabel().setEnabled(false);
        tableDlg.show();
    }
    
    /** 
     * Purpose : The method that gets called when the user
     *           presses a button
     * Method  : Figure out which button was pressed, and act
     *           accordingly
     */
    public void actionPerformed(ActionEvent ae) 
    {
	// Close the dialog if the user clicks the cancel button */
        if(ae.getSource() == tableDlg.getCancel())
        {
            tableDlg.hide();
            tableDlg.dispose();
        }

	//Attempt to construct the table if the user presses the OK
	//button.  Then, close the dialog.if the construction went
	//smoothly.
        if(ae.getSource() == tableDlg.getOK() && constructTable())
        {
            tableDlg.hide();
            tableDlg.dispose();
        }
    }
    
    /** 
     * Purpose : The method that gets called when the user selects
     *           the checkbox in the TableDlg
     * Method  : Enable the JLabel and JSpinner for the global
     *           table variable.
     */
    public void itemStateChanged(ItemEvent itemEvent) 
    {
        tableDlg.getTabNumSpinner().setEnabled(true);
        tableDlg.getTabNumLabel().setEnabled(true);
    }
    
    /**
     * Purpose : Construct the TableObject that the TableDlg
     *           represents
     * Method  : Check to make sure the entered values are resonable
     *           and use them to create the TableObject
     * @return boolean
     */
    public boolean constructTable()
    {
	Integer temp;

	//if the name field is blank, complain
        if(tableDlg.getNameField().getText().equals(""))
        {
            JOptionPane.showMessageDialog(null,
                                "Name Empty",
                                "Please fill in the Name field",
                                JOptionPane.ERROR_MESSAGE);
            
            return false;
        }
        
        // if the user has selected the global tab JCheckBox
        if(tableDlg.getGlobalTabCheck().isSelected())
	{
        	temp=(Integer)tableDlg.getTabNumSpinner().getValue();

		//complain if the user has assigned a global index
		//that is not unique
		if(jnwntfm.duplicateGlobal(temp.intValue()))
		{
            		JOptionPane.showMessageDialog(null,
                                "Duplicate Global ID",
                                "Please select a unique Global ID",
                                JOptionPane.ERROR_MESSAGE);

			return false;
		}
	}
	//otherwise create a TableObject 
        tableObj = new TableObject();
	//and assigning values to its variables
	tableObj.setName(tableDlg.getNameField().getText());
	tableObj.setGlobal(true);
	temp = (Integer)tableDlg.getTabNumSpinner().getValue();
	tableObj.setGlobalID(temp.intValue());	
	tableObj.setTableCommand(tableDlg.getModeComboBox().getSelectedIndex() + 1);
	tableObj.setTableType(TableType.Placeable);

	return true;
        
    }
    
    /** Purpose : To provide the created TableObject to the
     *            JNWNTFrameMediator which is placed in the 
     *            JTable that makes up the main GUI.
     *  Method  : Return the member variable
     * @return jnwntreasure.abstraction.TableObject
     */
    public TableObject getTable()
    {
        return tableObj;
    }
    
}
