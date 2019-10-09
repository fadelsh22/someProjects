import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 * The CommunicationsMonitor class represents the graph G
 * built to answer infection queries.
 *
 * @author Katelyn Lamison 
 * @author Fadel Alshammasi
 * @author Gian
 */
public class CommunicationsMonitor {

	public HashMap<Integer, List<ComputerNode>> adj;
	public ArrayList<Triple> triples;
	public boolean invoked  = false;


	/**
	 * Constructor with no parameters
	 */
	public CommunicationsMonitor() {
		this.adj = new HashMap<Integer, List<ComputerNode>>();
		this.triples = new ArrayList<Triple>();
	}

	/**
	 * Takes as input two integers c1, c2, and a timestamp. This triple represents the fact that the computers with IDs
	 * c1 and c2 have communicated at the given timestamp. This method should run in O(1) time. Any invocation of this
	 * method after createGraph() is called will be ignored.
	 *
	 * @param c1        First ComputerNode in the communication pair.
	 * @param c2        Second ComputerNode in the communication pair.
	 * @param timestamp Time the communication took place.
	 */
	public void addCommunication(int c1, int c2, int timestamp) {
		if(invoked)
			return;
		Triple t = new Triple (c1, c2, timestamp);
		triples.add(t);
	}

	/**
	 * Constructs the data structure as specified in the Section 2. This method should run in O(n + m log m) time.
	 */
	public void createGraph() {
		// TOTAL TIME: O(m + mlogm)

		// takes O(mlogm)
		this.invoked=true;
		Collections.sort(triples, new TripleComparator());

		// for loop is m times, each loop is O(1)
		for (Triple t : triples) {
			ComputerNode one = new ComputerNode(t.ci, t.timestamp);
			ComputerNode two = new ComputerNode(t.cj, t.timestamp);

			if (adj.get(t.ci) == null)
			{
				adj.put(t.ci, new LinkedList<ComputerNode>());
				adj.get(t.ci).add(one);
			}
			else
			{
				LinkedList<ComputerNode> list = (LinkedList<ComputerNode>) adj.get(t.ci);
				ComputerNode last = list.getLast();
				if (last.getTimestamp() != one.getTimestamp())
				{
					// node is not already in list, so we add it.
					adj.get(t.getCi()).add(one);
					last.edges.add(one);
				}
			}

			if (adj.get(t.cj) == null)
			{
				adj.put(t.cj, new LinkedList<ComputerNode>());
				adj.get(t.cj).add(two);
			}
			else
			{
				LinkedList<ComputerNode> list = (LinkedList<ComputerNode>) adj.get(t.cj);
				ComputerNode last = list.getLast();
				if (last.getTimestamp() != two.getTimestamp())
				{
					// node is not already in list, so we add it.
					adj.get(t.getCj()).add(two);
					last.edges.add(two);
				}
			}

			// now all of our nodes are added & hash map is maintained.
			LinkedList<ComputerNode> list1 = (LinkedList<ComputerNode>) adj.get(t.ci);
			ComputerNode ci = list1.getLast();

			LinkedList<ComputerNode> list2 = (LinkedList<ComputerNode>) adj.get(t.cj);
			ComputerNode cj = list2.getLast();

			ci.edges.add(cj);
			cj.edges.add(ci);
		}   	
	}

	/**
	 * Determines whether computer c2 could be infected by time y if computer c1 was infected at time x. If so, the
	 * method returns an ordered list of ComputerNode objects that represents the transmission sequence. This sequence
	 * is a path in graph G. The first ComputerNode object on the path will correspond to c1. Similarly, the last
	 * ComputerNode object on the path will correspond to c2. If c2 cannot be infected, return null.
	 * <p>
	 * Example 3. In Example 1, an infection path would be (C1, 4), (C2, 4), (C2, 8), (C4, 8), (C3, 8)
	 * <p>
	 * This method can assume that it will be called only after createGraph() and that x <= y. This method must run in
	 * O(m) time. This method can also be called multiple times with different inputs once the graph is constructed
	 * (i.e., once createGraph() has been invoked).
	 *
	 * @param c1 ComputerNode object to represent the Computer that is hypothetically infected at time x.
	 * @param c2 ComputerNode object to represent the Computer to be tested for possible infection if c1 was infected.
	 * @param x  Time c1 was hypothetically infected.
	 * @param y  Time c2 is being tested for being infected.
	 * @return List of the path in the graph (infection path) if one exists, null otherwise.
	 */
	public List<ComputerNode> queryInfection(int c1, int c2, int x, int y) {
		// TOTAL TIME: O(m)
		// going through list is O(m) & BFS is O(2n + m)
		// O(2n + 2m) --> O(n + m) --> O(m)
		
		ComputerNode infected = new ComputerNode(c1, x);
		ComputerNode maybe = new ComputerNode(c2, y);
		LinkedList<ComputerNode> list = (LinkedList<ComputerNode>) adj.get(c1);

		// If this computer is not in the hash map, return null
		if (list == null)
			return null;

		// If no computer in hash map is greater or equal, return null
		if (list.getLast().getTimestamp() < infected.getTimestamp())
			return null;

		for (ComputerNode cn: list) {
			if (cn.getTimestamp() >= infected.getTimestamp()) {
				infected = cn;
				break;
			}
		}

		// Run BFS to find the path from our infected to our maybe
		LinkedList<ComputerNode> reach = (LinkedList<ComputerNode>) BFS(infected, maybe);  		
		return reach;
	}

	public LinkedList<ComputerNode>  BFS(ComputerNode infected, ComputerNode maybe) {
		// Add nodes as we discover them
		HashSet<ComputerNode> discovered = new HashSet<ComputerNode>();
		Queue<ComputerNode> q = new LinkedList <ComputerNode>();
		//LinkedList<ComputerNode> path = new LinkedList<ComputerNode>();

		infected.pred = null;
		//path.add(infected);
		q.offer(infected);

		while(!q.isEmpty()) {
			// u is front of the queue
			ComputerNode u = q.element();
			for (ComputerNode v : u.edges) {
				if(!discovered.contains(v)) {
					// so v is NOT discovered, so add it to the hash set
					discovered.add(v);
					v.pred = u;
					//path.add(v);
					q.offer(v);

					if (v.ID == maybe.ID && v.timestamp <= maybe.timestamp)
					{
						LinkedList<ComputerNode> ret = new LinkedList<ComputerNode>();
						ComputerNode n = v;
						while (n != null)
						{
							ret.addFirst(n);
							n = n.pred;
						}
						return ret;
					}

				}

			}
			q.poll();
			discovered.add(u);	
		}
		// never found the computer we were looking, return null 
		return null;
	}

	/**
	 * Returns a HashMap that represents the mapping between an Integer and a list of ComputerNode objects. The Integer
	 * represents the ID of some computer Ci, while the list consists of pairs (Ci, t1),(Ci, t2),..., (Ci, tk),
	 * represented by ComputerNode objects, that specify that Ci has communicated with other computers at times
	 * t1, t2,...,tk. The list for each computer must be ordered by time; i.e., t1\<t2\<...\<tk.
	 *
	 * @return HashMap representing the mapping of an Integer and ComputerNode objects.
	 */
	public HashMap<Integer, List<ComputerNode>> getComputerMapping() {
		return adj;
	}

	/**
	 * Returns the list of ComputerNode objects associated with computer c by performing a lookup in the mapping.
	 *
	 * @param c ID of computer
	 * @return ComputerNode objects associated with c.
	 */
	public List<ComputerNode> getComputerMapping(int c) {
		return adj.get(c);
	}


}
