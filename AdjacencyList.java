import java.util.ArrayList;
import java.util.List;

public class AdjacencyList {

    private class Node<T> {
        T val;
        List<Node<T>> edges;

        Node(T val) {
            this.val = val;
            this.edges = new ArrayList<>();
        }
    }

    private List<Node> nodes;

    public AdjacencyList() {
        this.nodes = new ArrayList<Node>();
    }

    public AdjacencyList(ArrayList<Node> nodes) {
        this.nodes = nodes;
    }

    public boolean insertNode() {
        return false;
    }

    public boolean deleteNode() {
        return false;
    }

    public boolean insertEdge() {
        return false;
    }

    public boolean deleteEdge() {
        return false;
    }

    public boolean find() {
        return false;
    }

    public static void main(String[] args) {

    }
}
