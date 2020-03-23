import java.util.ArrayList;
import java.util.Iterator;

public class AdjacencyList<T> {
    // technically the correct plural is "vertices" but that looks confusing!
    private ArrayList<ArrayList<T>> vertexes;

    public AdjacencyList() {
        this.vertexes = new ArrayList<>();
    }

    public AdjacencyList(ArrayList<ArrayList<T>> vertexes) {
        this.vertexes = vertexes;
    }

    public boolean insertVertex(T val) {
        if (find(val)) {
            return false;
        } else {
            ArrayList<T> vertex = new ArrayList<T>();
            vertex.add(val);
            vertexes.add(vertex);
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
            ArrayList<T> vertexFrom = vertexes.get(i);
            ArrayList<T> vertexTo = vertexes.get(j);
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
            ArrayList<T> vertexFrom = vertexes.get(i);
            ArrayList<T> vertexTo = vertexes.get(j);
            // can only delete an edge if it exists
            return vertexFrom.remove(to) && vertexTo.remove(from);
        }
    }

    public boolean find(T val) {
        return indexOf(val) > -1;
    }

    // helper method to delete all
    private void deleteAllEdges(T toDelete) {
        for (ArrayList<T> vertex : vertexes) {
            if (!vertex.get(0).equals(toDelete)) {
                // we don't need the return value here, it will just remove the edge if it
                // exists
                vertex.remove(toDelete);
            }
        }
    }

    private int indexOf(T val) {
        int i = -1;
        boolean match = false;
        Iterator<ArrayList<T>> iterator = vertexes.iterator();
        while (iterator.hasNext()) {
            i++;
            if (iterator.next().get(0).equals(val)) {
                match = true;
                break;
            }
        }
        return match ? i : -1;
    }

    public void print() {
        if (vertexes.size() == 0) {
            System.out.println("The adjacency list is currently empty.");
            return;
        }
        for (ArrayList<T> vertex : vertexes) {
            System.out.print(vertex.get(0));
            if (vertex.size() > 1) {
                System.out.print(": [");
                for (int i = 1; i < vertex.size(); i++) {
                    System.out.print(i < vertex.size() - 1 ? vertex.get(i) + ", " : vertex.get(i));
                }
                System.out.print("]");
            }
            System.out.println();
        }
    }

    // public static void main(String[] args) {
    // AdjacencyList list = new AdjacencyList();
    // list.insertVertex(5);
    // list.insertVertex(4);
    // list.insertVertex(12);
    // list.insertEdge(5, 4);
    // list.insertEdge(5, 12);
    // list.print();
    // System.out.println();
    // list.deleteEdge(5, 4);
    // list.insertVertex(6);
    // list.insertEdge(6, 4);
    // list.print();
    // System.out.println();
    // list.deleteVertex(12);
    // list.print();
    // System.out.println();
    // list.deleteVertex(4);
    // list.print();
    // System.out.println();
    // list.deleteVertex(5);
    // list.insertEdge(5, 3);
    // list.deleteVertex(3);
    // list.print();
    // }
}