package jnwntreasure.control;

/**
 * @author Jason Mazzotta
 */

import jnwntreasure.presentation.JNWNTFrame;
import jnwntreasure.abstraction.RootObject;
import jnwntreasure.abstraction.TableObject;
import jnwntreasure.abstraction.enum.TableType;
import jnwntreasure.control.TableDlgMediator;
import jnwntreasure.control.TableItemDlgMediator;
import javax.swing.tree.TreeSelectionModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.JOptionPane;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Enumeration;

/** 
 * This class controls the interaction between the GUI
 * for the main application and all the other components
 * that it is composed of.
 */
public class JNWNTFrameMediator implements ActionListener, TreeSelectionListener
{
        /**
	 * The Frame that holds the entire application.
         * @see javax.swing.JFrame
         */	
	JNWNTFrame jnwntFrame;
        /**
         * The <code>DefaultTreeModel</code> through which
         * this class interacts with the <code>JTree</code>
	 * @see javax.swing.tree.DefaultTreeModel
	 * @see javax.swing.JTree
         */
        DefaultTreeModel tableModel;
	/** 
         * An object that handles the interaction between a dialog
         * for adding tables and the rest of the world.
         */
	TableDlgMediator tdm;
	
        /**
     	 * Purpose : Constructor
 	 * Method  : Initialize member variables
         */
	public JNWNTFrameMediator()
	{

		// create the jnwntreasure Frame
		jnwntFrame = new JNWNTFrame();

		// give it a title
		jnwntFrame.setTitle("NWN Treasure Editor");

		// initialize the contents of the Tool Bar
		setToolBarDisabled();
		addToolBarListeners();

		// initialize the contents of the Menu Bar
		setMenuBarDisabled();
		addMenuBarListeners();

		// Set the initial state of the menus and toolbar
		jnwntFrame.getExitItem().setEnabled(true);

		// initialize the contents of the Table
		initTable();
		
		// Setup the tree to only allow a single selection at a time
		jnwntFrame.getTableTree().getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

	}

	/**
	 * Purpose : display the main GUI  
  	 * Method  : call the show method for the Frame
	 * @see javax.swing.JFrame
	 */
	public void show()
	{
		jnwntFrame.show();
	}

	/** 
	 * Purpose : To add the four table type markers to the Tree,
	 *           and set its behavior.
	 * Method  : Create an (invisible) root object, add four marker
	 *           <code>RootObject</code>s, and set a default selection
	 * @see jnwntreasure.abstraction.RootObject
         */
	public void initTable()
	{
		jnwntFrame.getTableTree().addTreeSelectionListener(this);

		RootObject parentObject = new RootObject(TableType.Any);

		DefaultMutableTreeNode parentNode = 
			new DefaultMutableTreeNode(parentObject);

		// create a model for the tree, this is largely how
		// we interact with the JTree
		tableModel = new DefaultTreeModel(parentNode);

		// Set this model for the tree.
		jnwntFrame.getTableTree().setModel(tableModel);

		// Add a 'Treasure Tables' node 
		RootObject childObject = new RootObject(TableType.Treasure);
		DefaultMutableTreeNode childNode = new 
				DefaultMutableTreeNode(childObject);
		tableModel.insertNodeInto(childNode, parentNode, 
					parentNode.getChildCount());

		// Add a 'Encounter Tables' node
		childObject = new RootObject(TableType.Encounter);
		childNode = new DefaultMutableTreeNode(childObject);
		tableModel.insertNodeInto(childNode, parentNode, 
					parentNode.getChildCount());

		// Add a 'Placeable Tables' node
		childObject = new RootObject(TableType.Placeable);
		childNode = new DefaultMutableTreeNode(childObject);
		tableModel.insertNodeInto(childNode, parentNode,
						parentNode.getChildCount());

		// Add a 'Profile Tables' node
		childObject = new RootObject(TableType.Profile);
		childNode = new DefaultMutableTreeNode(childObject);
		tableModel.insertNodeInto(childNode, parentNode,
				parentNode.getChildCount());

		// Make the root invisible but make sure you 
		// can expand its children
		jnwntFrame.getTableTree().setRootVisible(false);
		jnwntFrame.getTableTree().setShowsRootHandles(true);

		// Make the marker nodes visible 
		jnwntFrame.getTableTree().scrollPathToVisible(new 
					TreePath(childNode.getPath()));

		// Set the 'Placeable Tables' as the first selection
		jnwntFrame.getTableTree().setSelectionRow(2);
		
	}

	/** 
	 * Purpose : Disable  all the Tool Bar buttons
	 * Method  : call the <code>setEnabled</code>
	 *           method for each <code>JButton</code>
	 * @see javax.swing.JButton
	 */ 
	public void setToolBarDisabled()
	{
		jnwntFrame.getOpenButton().setEnabled(false);
		jnwntFrame.getSaveButton().setEnabled(false);
		jnwntFrame.getCreateButton().setEnabled(false);
		jnwntFrame.getAfterButton().setEnabled(false);
		jnwntFrame.getPropButton().setEnabled(false);
		jnwntFrame.getTabPropButton().setEnabled(false);
		jnwntFrame.getDeleteButton().setEnabled(false);
		jnwntFrame.getUpButton().setEnabled(false);
		jnwntFrame.getDownButton().setEnabled(false);
		jnwntFrame.getTableButton().setEnabled(false);
	}

	/** 
	 * Purpose : Make sure the program can react to the selection
	 *	     of a button on the Tool Bar
	 * Method  : call the <code>addActionListener</code>
	 *           method for each <code>JButton</code>
	 */ 
	public void addToolBarListeners()
        {
		jnwntFrame.getNewButton().addActionListener(this);
		jnwntFrame.getOpenButton().addActionListener(this);
		jnwntFrame.getSaveButton().addActionListener(this);
		jnwntFrame.getCreateButton().addActionListener(this);
		jnwntFrame.getAfterButton().addActionListener(this);
		jnwntFrame.getPropButton().addActionListener(this);
		jnwntFrame.getTabPropButton().addActionListener(this);
		jnwntFrame.getDeleteButton().addActionListener(this);
		jnwntFrame.getUpButton().addActionListener(this);
		jnwntFrame.getDownButton().addActionListener(this);
		jnwntFrame.getTableButton().addActionListener(this);
	}
	
	/** 
	 * Purpose : Set all the menu bar items disabled
	 * Method  : call the <code>setEnabled</code>
	 *           method for each <code>JMenuItem</code>
  	 * @see javax.swing.JMenuItem
	 */ 
	public void setMenuBarDisabled()
	{
		jnwntFrame.getNewItem().setEnabled(false);
		jnwntFrame.getOpenItem().setEnabled(false);
		jnwntFrame.getMoveDownItem().setEnabled(false);
		jnwntFrame.getMoveUpItem().setEnabled(false);
		jnwntFrame.getSaveItem().setEnabled(false);
		jnwntFrame.getTabPropsItem().setEnabled(false);
		jnwntFrame.getDeleteItem().setEnabled(false);
		jnwntFrame.getExportItem().setEnabled(false);
		jnwntFrame.getPropsItem().setEnabled(false);
		jnwntFrame.getSaveAsItem().setEnabled(false);
		jnwntFrame.getCloseItem().setEnabled(false);
		jnwntFrame.getInsNewAftItem().setEnabled(false);
		jnwntFrame.getInsNewInItem().setEnabled(false);
		jnwntFrame.getInsTabItem().setEnabled(false);
		jnwntFrame.getAboutItem().setEnabled(false);
        
        }

	/** 
	 * Purpose : Make sure the program can react to the selection
	 *	     of a Menu Item
	 * Method  : call the <code>addActionListener</code>
	 *           method for each <code>JMenuItem</code>
	 */ 
	public void addMenuBarListeners()
	{
		jnwntFrame.getMoveUpItem().addActionListener(this);
		jnwntFrame.getMoveDownItem().addActionListener(this);
		jnwntFrame.getSaveItem().addActionListener(this);
		jnwntFrame.getTabPropsItem().addActionListener(this);
		jnwntFrame.getOpenItem().addActionListener(this);
		jnwntFrame.getDeleteItem().addActionListener(this);
		jnwntFrame.getExitItem().addActionListener(this);
		jnwntFrame.getExportItem().addActionListener(this);
		jnwntFrame.getNewItem().addActionListener(this);
		jnwntFrame.getPropsItem().addActionListener(this);
		jnwntFrame.getSaveAsItem().addActionListener(this);
		jnwntFrame.getCloseItem().addActionListener(this);
		jnwntFrame.getInsNewAftItem().addActionListener(this);
		jnwntFrame.getInsNewInItem().addActionListener(this);
		jnwntFrame.getAboutItem().addActionListener(this);
		jnwntFrame.getInsTabItem().addActionListener(this);
	}

	/**
	 * Purpose : React to the selection of a JButton or JMenuItem
	 * Method  : Do something based on the particular item that
	 *           was selected
	 */
	public void actionPerformed(ActionEvent ae)
	{
		//Exit the application
		if(ae.getSource() == jnwntFrame.getExitItem())
			System.exit(0);

		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsTabItem())
	          {
			addNewTable(getSelectedObject());	
		  }
		// Delete a tree node
		if(ae.getSource() == jnwntFrame.getDeleteButton() ||
		   ae.getSource() == jnwntFrame.getDeleteItem())
	          {
			deleteObject(getSelectedObject());	
		  }
		
		// Display the properties of a table
		if(ae.getSource() == jnwntFrame.getTabPropButton() ||
		   ae.getSource() == jnwntFrame.getTabPropsItem())
	          {
			displayTabProps(getSelectedObject());	
		  }
		// Add a new TableItem
		if(ae.getSource() == jnwntFrame.getCreateButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewItem(getSelectedObject());	
		  }
		
		// Delete the tree and start again
		if(ae.getSource() == jnwntFrame.getNewButton() ||
		   ae.getSource() == jnwntFrame.getNewItem())
	          {
			restartTree();	
		  }
		/*
		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewTable();	
		  }
		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewTable();	
		  }
		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewTable();	
		  }
		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewTable();	
		  }
		// Add a new Table
		if(ae.getSource() == jnwntFrame.getTableButton() ||
		   ae.getSource() == jnwntFrame.getInsNewInItem())
	          {
			addNewTable();	
		  }

		*/

	} 

	/**
	 * Purpose : To retrieve the object that the user selected 
	 *           in the JTree
         * Method  : Call getSelectionPath() for the Tree 
         * @return javax.swing.tree.DefaultMutableTreeNode
 	 */
	public DefaultMutableTreeNode getSelectedObject()
	{
		//Get the path from the root node
		//To the selected item
		DefaultMutableTreeNode selectedNode=null;
		TreePath selectedPath = 
			jnwntFrame.getTableTree().getSelectionPath();

		//Get the selected item
		return (DefaultMutableTreeNode)selectedPath.getLastPathComponent();

	}

	/** 
         * Purpose : Makes sure the Toolbar buttons and menu items 
         *           are in the correct state
         * Method  : Determine which item was selected and call
         *           proper helper methods
	 */
	public void valueChanged(TreeSelectionEvent tse)
	{
		DefaultMutableTreeNode dmtn = getSelectedObject();
		java.lang.Object treeNode = dmtn.getUserObject();
		RootObject ro;

		setMenuBarDisabled();
		setToolBarDisabled();

		//Turn on the insert table menu and toolbar stuff
		if(treeNode instanceof RootObject)
		{
			ro = (RootObject)treeNode;

			if(ro.getTableType() == TableType.Placeable)
			{
				jnwntFrame.getTableButton().setEnabled(true);
				jnwntFrame.getInsTabItem().setEnabled(true);
			}
			else
			{
				setMenuBarDisabled();
				setToolBarDisabled();
			}
		}
		//Turn on the TableItem menu and toolbar stuff
		if(treeNode instanceof TableObject)
		{
			jnwntFrame.getDeleteItem().setEnabled(true);
			jnwntFrame.getTabPropsItem().setEnabled(true);
			jnwntFrame.getInsNewInItem().setEnabled(true);
			jnwntFrame.getDeleteButton().setEnabled(true);
			jnwntFrame.getTabPropButton().setEnabled(true);
			jnwntFrame.getCreateButton().setEnabled(true);
			
		}

	}

	/**
	 * Purpose : Start the Script tree over
	 * Method  : Remove the TreeSelectionListener,
	 *           delete the model for the tree, and
	 *           then call the initTable method to 
	 *           start over
         */	
	public void restartTree()
	{
		int response;
		DefaultMutableTreeNode root;

		//Get the depth of the tree
		root = (DefaultMutableTreeNode)tableModel.getRoot();
		
		//If the depth is greater than 1, the user has added
		//stuff so we should ask if they want to do this
		if(root.getDepth() > 1)
		{
			response = JOptionPane.showConfirmDialog(null,
						"Do you want to restart your script?",
						"New Confirmation",
						JOptionPane.YES_NO_OPTION);
		
			if(response == JOptionPane.NO_OPTION)
				return;
		}
		//Remove the tableModel
		tableModel = null;
		jnwntFrame.getTableTree().removeTreeSelectionListener(this);
		jnwntFrame.getTableTree().setModel(tableModel);

		//Recreate the starting tree
		initTable();
	}

	/** 
	 * Purpose : To add a new Table to the Script Tree
         * Method  : Get the RootObject, use a TableDialog
	 *           to create the TableObject, and then
	 *           add the Table Object to the Table Model
         */
	public void addNewTable(DefaultMutableTreeNode dmtn)
	{
		DefaultMutableTreeNode childNode;
		//Get the Root Object to which we will add the table
		RootObject root = (RootObject) dmtn.getUserObject();
		
		//Create a Table Dialog for the user to fill out	
		tdm = new TableDlgMediator(this);

		//Get the Table Object they have created
		TableObject to = tdm.getTable();

		//They may have decided to cancel the table creation
		if(to == null)
			return;

		//Create a tree node from this TableObject
		childNode = new DefaultMutableTreeNode(to);	
		
		//Insert this node into the tree	
		tableModel.insertNodeInto(childNode, dmtn,
					  dmtn.getChildCount());

	
		//Make sure the user can see their newly added node	
		jnwntFrame.getTableTree().scrollPathToVisible(new 
					TreePath(childNode.getPath()));
	}

        /**
	 * Purpose : Called to add a new item to a table
         * Method  : Use a TableItemDlgMediator
         *
         * @see javax.swing.tree.DefaultMutableTreeNode
         * @see jnwntreasure.control.TableItemDlgMediator
         * @see jnwntreasure.presentation.TableItemDlg
         */
	public void addNewItem(DefaultMutableTreeNode dmtn)
	{
		TableItemDlgMediator tidm = new TableItemDlgMediator(this);
	}

        /**
         * Purpose : Delete any object from the trees that make up
         *           the trees from which a script is constructed
         * Method  : Remove the selected item from the Tree
         */
	public void deleteObject(DefaultMutableTreeNode dmtn)
	{
		TreeNode parent = dmtn.getParent();

		//removing an item is going to fire off some undesirable
		//events
		jnwntFrame.getTableTree().removeTreeSelectionListener(this);
		//remove the node that we have selected
		tableModel.removeNodeFromParent(dmtn);
		//start listening again
		jnwntFrame.getTableTree().addTreeSelectionListener(this);
		//set the selection to the parent of the removed node
		jnwntFrame.getTableTree().setSelectionPath(new 
					TreePath(tableModel.getPathToRoot(parent)));
	}

        /**
	 * TBD
         */
	public void displayTabProps(DefaultMutableTreeNode dmtn)
	{
		JOptionPane.showMessageDialog(null,
						"This doesn't work yet",
						"Oops",
						JOptionPane.ERROR_MESSAGE);
	}

	/**
	 * Purpose : To determine if a table that has been added to the tree 
	 *           has a unique global id
         * Method  : Check the unique ID's of all the children of selected Root Object
	 * @return boolean
	 */
	public boolean duplicateGlobal(int gID)
	{
		DefaultMutableTreeNode rootObject, tempNode;
		TableObject tempObject;
		Enumeration children;

		//Get the root Object
		rootObject = getSelectedObject();

		//Get the Root Objects Children
		children = rootObject.children();
	
		//Compare the value of the global ID for each child
		// to the one passed in
		while(children.hasMoreElements())
		{
			tempNode = (DefaultMutableTreeNode) children.nextElement();

			tempObject = (TableObject)tempNode.getUserObject();

			//Return true if there's a match
			if(tempObject.getGlobalID() == gID)
				return true;

		}
		
		return false;
	}
}
