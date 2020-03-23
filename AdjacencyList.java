import java.util.ArrayList;
import java.util.Iterator;

public class AdjacencyList<T> {
    // technically the correct plural is "vertices" but that looks confusing!
    private FGLockingList<T> vertexes;

    public AdjacencyList() {
        this.vertexes = new FGLockingList<>();
    }

    public AdjacencyList(FGLockingList<T> vertexes) {
        this.vertexes = vertexes;
    }

    public boolean insertVertex(T val) {
        if (find(val)) {
            return false;
        } else {
            vertexes.add(val);
            return true;
        }
    }

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

    public boolean find(T val) {
        return indexOf(val) > -1;
    }

    // helper method to delete all
    private void deleteAllEdges(T toDelete) {
        for (Node<T> vertex : vertexes.list) {
            if (!vertex.edges.get(0).equals(toDelete)) {
                // we don't need the return value here, it will just remove the edge if it
                // exists
                vertex.edges.remove(toDelete);
            }
        }
    }

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

    public Node<T> getNode(T val) {
        return this.vertexes.getNode(val);
    }

    public int size() {
        return this.vertexes.size();
    }

    public void print() {
        if (vertexes.size() == 0) {
            System.out.println("The adjacency list is currently empty.");
            return;
        }
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

     public static void main(String[] args) {
         AdjacencyList list = new AdjacencyList();
         list.insertVertex(5);
         list.insertVertex(4);
         list.insertVertex(12);
         list.insertEdge(5, 4);
         list.insertEdge(5, 12);
         list.print();
         System.out.println();
         list.deleteEdge(5, 4);
         list.insertVertex(6);
         list.insertEdge(6, 4);
         list.print();
         System.out.println();
         list.deleteVertex(12);
         list.print();
         System.out.println();
         list.deleteVertex(4);
         list.print();
         System.out.println();
         list.deleteVertex(5);
         list.insertEdge(5, 3);
         list.deleteVertex(3);
         list.print();
     }
}