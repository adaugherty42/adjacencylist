import java.util.ArrayList;
import java.util.BitSet;
import java.util.Vector;

public class LockTest {
    static int NUM_THREADS = 4;
    static int NUM_RESOURCES = 32;
    static int NUM_ITERATIONS = 15;

    public static void main(String[] args) {
        // Grab args if they exist
        if (args.length > 1) {
            NUM_THREADS = Integer.parseInt(args[1]);
        }
        if (args.length > 2) {
            NUM_RESOURCES = Integer.parseInt(args[2]);
        }
        if (args.length > 3) {
            NUM_ITERATIONS = Integer.parseInt(args[3]);
        }

        AdjacencyList<Integer> adjList = new AdjacencyList<>();
        MRLock lock = new MRLock();
        // This ResourceAllocator will be passed to the spawned threads, which will use it to
        // create Lockable objects for their requests.
        ResourceAllocator resourceAlloc = new ResourceAllocator(lock, NUM_RESOURCES);

        // Create and start threads
        ArrayList<Thread> threads = new ArrayList<Thread>(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threads.add(new Thread(new LockThread(adjList, resourceAlloc, NUM_ITERATIONS)));
            threads.get(i).start();
        }
        // Wait for threads to join
        for (int i = 0; i < NUM_THREADS; i++) {
            try {
                threads.get(i).join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        adjList.print();
    }
}

class LockThread implements Runnable {
    AdjacencyList<Integer> adjList;
    ResourceAllocator resourceAlloc;
    int numIterations;
    int numResources;

    public LockThread(AdjacencyList<Integer> adjList, ResourceAllocator resourceAlloc, int numIterations) {
        this.adjList = adjList;
        this.resourceAlloc = resourceAlloc;
        this.numIterations = numIterations;
        this.numResources = this.resourceAlloc.lock.buffer.length;
    }

    @Override
    public void run() {
        for (int i = 0; i < numIterations; i++) {
            Vector<Integer> resources = new Vector<>(10);
            // For each iteration, we need to generate a random list of resources to do operations on
            for (int j = 0; j < 3; ) {
                int randomResource = (int)(Math.random() * numResources);
                resources.add(randomResource);
            }
            // Create lockable object
            MRLockable lockable = resourceAlloc.createLockable(resources);
            // Lock
            lockable.lock();
            adjList.insertVertex(resources.get(0));
//            adjList.insertVertex(resources.get(1));
//            adjList.insertVertex(resources.get(2));
//            adjList.insertEdge(resources.get(3), resources.get(4));
//            adjList.insertEdge(resources.get(5), resources.get(6));
//            adjList.deleteEdge(resources.get(7), resources.get(8));
//            adjList.deleteVertex(resources.get(9));
            lockable.unlock();
        }
    }
}