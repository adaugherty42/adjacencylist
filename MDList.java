public class MDList {
    static final int dim = 16;
    static final int N = 10_000;
    MDNode head;

    public MDList() {
        head = new MDNode(0, 0, dim);
    }

    public int[] keyToCoord(int key) {
        int k[] = new int[dim];
        int basis = (int) Math.ceil(Math.pow(N, 1/dim));
        int quotient = key;

        for (int i = dim - 1; i >= 0; i--) {
            k[i] = quotient % basis;
            quotient = quotient / basis;
        }

        return k;
    }
}

class MDNode<T> {
    int key;
    int k[];
    T val;
    MDNode child[];

    public MDNode(int key, T val, int dim) {
        this.key = key;
        this.val = val;
        this.k = new int[dim];
        this.child = new MDNode[dim];
    }
}