import javafx.util.Pair;

import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArrayList;

class Node<T>{

    ArrayList<T> edges;
    boolean deleted;

    public Node(T val)
    {
        this.edges = new ArrayList<T>();
        this.edges.add(val);
        deleted = false;
    }

    public void delete(T v)
    {
        deleted = true;
    }

    public boolean isDeleted(T v)
    {
        return deleted;
    }

    public T getValue() {
        return this.edges.get(0);
    }
}

public class FGLockingList<T>{

    CopyOnWriteArrayList<Node<T>> list;

    public FGLockingList(){
        list = new CopyOnWriteArrayList<>();
    }

    public void add(T val) {
        list.add(new Node<T>(val));
    }

    public int indexOf(T val) {

        for (int i = 0; i < list.size(); i++) {
            if (list.get(i).getValue() == val)
                return i;
        }

        return -1;

    }

    public boolean contains(T val) {
        return indexOf(val) != -1;
    }

    public T get(int ind){
        return list.get(ind).getValue();
    }

    public Node<T> getNode(int ind){
        return list.get(ind);
    }

    public Pair<Node<T>, Integer> getNodeAndIndex(T val) {
        for (int i = 0; i < list.size(); i++) {
            Node<T> node = list.get(i);
            if (node.getValue() == val) {
                return new Pair(node, i);
            }
        }
        return null;
    }

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
