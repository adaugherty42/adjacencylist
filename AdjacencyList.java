import java.util.ArrayList;
import java.util.Iterator;

// basic adjacency list, adapted for interaction with the FGLockingList
// and MRLock classes
public class AdjacencyList<T> {
    // technically the correct plural is "vertices"
    private FGLockingList<T> vertexes;

    // an adjacency list consists of a back bone of a fine grained locking list
    public AdjacencyList() {
        this.vertexes = new FGLockingList<>();
    }

    public AdjacencyList(FGLockingList<T> vertexes) {
        this.vertexes = vertexes;
    }

    // to insert a vertex, need to verify that the vertex does not exist
    // and otherwise add
    public boolean insertVertex(T val) {
        if (find(val)) {
            return false;
        } else {
            vertexes.add(val);
            return true;
        }
    }

    // opposite logic of insert
    // need to check if the node exists
    // and if it does, then delete it
    // but first remove all of the edges connected to it
    // for instance if removing vertex 5 and 4 is connected,
    // need to remove 5 from 4's sublist
    public boolean deleteVertex(T val) {
        int i = indexOf(val);
        if (i == -1) {
            return false;
        } else {
            deleteAllEdges(val);
            vertexes.remove(i);
            return true;
        }
    }

    // inserts an edge
    // as if in an undirected graph
    // such that both endpoints add each other their sublists
    public boolean insertEdge(T from, T to) {
        int i = indexOf(from);
        int j = indexOf(to);
        // both vertexes need to already be in the adjacency list to make an edge
        // between them
        if (i == -1 || j == -1) {
            return false;
        } else {
            ArrayList<T> vertexFrom = vertexes.getNodeAtIndex(i).edges;
            ArrayList<T> vertexTo = vertexes.getNodeAtIndex(j).edges;
            // only add edge if it doesn't already exist
            if (vertexFrom.contains(to) || vertexTo.contains(from)) {
                return false;
            } else {
                vertexFrom.add(to);
                vertexTo.add(from);
                return true;
            }
        }
    }

    // deletes an edge
    // with the some conditions as the insertion
    public boolean deleteEdge(T from, T to) {
        int i = indexOf(from);
        int j = indexOf(to);
        // can only delete an edge if both vertexes are in the list to begin with
        if (i == -1 || j == -1) {
            return false;
        } else {
            ArrayList<T> vertexFrom = vertexes.getNodeAtIndex(i).edges;
            ArrayList<T> vertexTo = vertexes.getNodeAtIndex(j).edges;
            // can only delete an edge if it exists
            return vertexFrom.remove(to) && vertexTo.remove(from);
        }
    }

    // if the indexOf method returns -1,
    // it was not found in the list
    public boolean find(T val) {
        return indexOf(val) > -1;
    }

    // helper method to delete all
    // no need to return a value here
    private void deleteAllEdges(T toDelete) {
        for (Node<T> vertex : vertexes.list) {
            if (!vertex.edges.get(0).equals(toDelete)) {
                vertex.edges.remove(toDelete);
            }
        }
    }

    // finds the index of a given value in the sublist
    private int indexOf(T val) {
        int i = -1;
        boolean match = false;
        Iterator<Node<T>> iterator = vertexes.list.iterator();
        while (iterator.hasNext()) {
            i++;
            if (iterator.next().edges.get(0).equals(val)) {
                match = true;
                break;
            }
        }
        return match ? i : -1;
    }

    // defers to the FGLockingList get value
    public Node<T> getNode(T val) {
        return this.vertexes.getNode(val);
    }

    // determines the full size of the list of vertices
    public int size() {
        return this.vertexes.size();
    }

    // prints the current state of the list
    public void print() {
        if (vertexes.size() == 0) {
            System.out.println("The adjacency list is currently empty.");
            return;
        }
        // prints out list in the format
        // <VERTEX>: [<CONNECTED_VERTEX1>, <CONNECTED_VERTEX2>, ... <CONNECTED_VERTEX_N>]
        for (Node<T> vertex : vertexes.list) {
            System.out.print(vertex.edges.get(0));
            if (vertex.edges.size() > 1) {
                System.out.print(": [");
                for (int i = 1; i < vertex.edges.size(); i++) {
                    System.out.print(i < vertex.edges.size() - 1 ? vertex.edges.get(i) + ", " : vertex.edges.get(i));
                }
                System.out.print("]");
            }
            System.out.println();
        }
    }
}