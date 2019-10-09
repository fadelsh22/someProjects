import java.util.ArrayList;
import java.util.List;

/**
 * The ComputerNode class represents the nodes of the graph G, which are pairs (Ci, t).
 *
 * @author Katelyn Lamison
 * @author Fadel Alshammasi 
 * @author Gian
 */
public class ComputerNode {

	protected int ID;
	protected int timestamp;
	public ArrayList<ComputerNode> edges;
	public ComputerNode pred;
	
	
	public ComputerNode(int ID, int timestamp)
	{
		this.ID = ID;
		this.timestamp = timestamp;
		this.edges = new ArrayList<ComputerNode>();
		this.pred = null;
	}
	
    /**
     * Returns the ID of the associated computer.
     *
     * @return Associated Computer's ID
     */
    public int getID() {
        return ID;
    }

    /**
     * Returns the timestamp associated with this node.
     *
     * @return Timestamp for the node
     */
    public int getTimestamp() {
        return timestamp;
    }

    /**
     * Returns a list of ComputerNode objects to which there is outgoing edge from this ComputerNode object.
     *
     * @return a list of ComputerNode objects that have an edge from this to the nodes in the list.
     */
    public List<ComputerNode> getOutNeighbors() {
        return edges;
    }
    
    @Override
    public String toString()
    {
    	
    	return "ID: " + this.ID + "     Timestamp: " + this.timestamp;
    }
    
    @Override
   public boolean equals(Object obj)
   {
    	if (this == obj) 
    		return true;
    	if (! (obj instanceof ComputerNode))
    		return false;
    	ComputerNode n = (ComputerNode) obj;
	   return this.timestamp == n.timestamp && n.ID == this.ID;
   }

}
