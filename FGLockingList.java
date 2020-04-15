import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArrayList;

// define our own node class to maintain
// both the edges arraylist as well as the deleted marker
class Node<T> {

    ArrayList<T> edges;
    boolean deleted;

    // custom constructor that allocates the edges sublist
    // and marks the delete flag as false
    public Node(T val) {
        this.edges = new ArrayList<T>();
        this.edges.add(val);
        deleted = false;
    }

    // sets the deleted marker of the node to true
    public void delete(T v) {
        deleted = true;
    }

    // returns the logical deletion status of the node
    public boolean isDeleted(T v) {
        return deleted;
    }

    // gets the value of the node
    public T getValue() {
        return this.edges.get(0);
    }
}

// fine grain locking list
public class FGLockingList<T> {

    // using a CopyOnWriteArrayList because it is thread safe
    // and because our list size is finite and known, the overhead of rewriting
    // is overshadowed by the efficiency of allowing traversals
    CopyOnWriteArrayList<Node<T>> list;

    public FGLockingList() {
        list = new CopyOnWriteArrayList<>();
    }

    // add to the array
    public void add(T val) {
        list.add(new Node<T>(val));
    }

    // determines the index of the vertex
    // to make sure that it is in the list
    public int indexOf(T val) {

        for (int i = 0; i < list.size(); i++) {
            if (list.get(i).getValue() == val)
                return i;
        }
        return -1;
    }

    // verifies that the list index is present
    public boolean contains(T val) {
        return indexOf(val) != -1;
    }

    // returns the value
    public T get(int ind){
        return list.get(ind).getValue();
    }

    public Node<T> getNodeAtIndex(int ind) {
        return list.get(ind);
    }

    // loops through to return a value
    // with the CopyOnWriteArray
    // and verifies the logical status of the node
    public Node<T> getNode(T val) {
        for (int i = 0; i < this.list.size(); i++) {
            Node<T> node = this.list.get(i);
            if (node.getValue().equals(val) && !node.deleted) {
                return node;
            }
        }
        return null;
    }

    // removes from list
    public boolean remove(T val){
        int remInd = indexOf(val);

        if(remInd != -1){
            list.remove(remInd);
            return true;
        }

        return false;
    }

    public Node<T> remove(int index){

        if(index>=0 && index<size()){
            return list.remove(index);
        }

        return null;
    }

    public Node<T> removeNode(int index){

        if(index>=0 && index<size()){
            return list.remove(index);
        }

        return null;
    }

    public boolean removeNode(T val){
        int remInd = indexOf(val);

        if(remInd != -1){
            list.remove(remInd);
            return true;
        }

        return false;
    }

    public int size() {
        return list.size();
    }


}
