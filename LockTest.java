import java.util.ArrayList;
import java.util.Vector;

// our main driver program
public class LockTest {
    static int NUM_THREADS = 4;
    static int NUM_RESOURCES = 64;
    static int NUM_ITERATIONS = 300;

    public static void main(String[] args) {
        // Grab args if they exist in the format
        // java LockTest <NUM_THREADS> <NUM_RESOURCES> <NUM_ITERATIONS>
        if (args.length > 1) {
            NUM_THREADS = Integer.parseInt(args[1]);
        }
        if (args.length > 2) {
            NUM_RESOURCES = Integer.parseInt(args[2]);
        }
        if (args.length > 3) {
            NUM_ITERATIONS = Integer.parseInt(args[3]);
        }

        // initialize the new list and the MRLOCK
        AdjacencyList<Integer> adjList = new AdjacencyList<>();
        MRLock lock = new MRLock();

        // This ResourceAllocator will be passed to the spawned threads, which will use it to
        // create Lockable objects for their requests.
        ResourceAllocator resourceAlloc = new ResourceAllocator(lock, NUM_RESOURCES);

        // Prepopulate list with a bunch of vertexes
        // to make it such that operations aren't running into an empty list constantly
        for (int i = 0; i < NUM_RESOURCES / 2; i++) {
            adjList.insertVertex(i);
        }

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

        // print the list at the end
        adjList.print();
    }
}

// thread class to run our driver program
class LockThread implements Runnable {
    AdjacencyList<Integer> adjList;
    ResourceAllocator resourceAlloc;
    int numIterations;
    int numResources;

    // allocating memory
    public LockThread(AdjacencyList<Integer> adjList, ResourceAllocator resourceAlloc, int numIterations) {
        this.adjList = adjList;
        this.resourceAlloc = resourceAlloc;
        this.numIterations = numIterations;
        this.numResources = this.resourceAlloc.lock.buffer.length;
    }

    // for each thread, run a certain subset of operations
    // for our testing purposes, we divided runs into vertex, edge, and "both" types
    @Override
    public void run() {
        for (int i = 0; i < numIterations; i++) {
            Vector<Integer> resources = new Vector<>(7);
            ArrayList<Node> nodes = new ArrayList<Node>();
            // For each iteration, we need to generate a random list of resources to do operations on
            while(resources.size()<3) {

                // get random value for the node
                int rand = (int)(Math.random() * numResources);

                // see if it's in list
                Node node = adjList.getNode(rand);

                // if it isn't, add index to resources vector and Pair to our pairs list
                if (node != null) {
                    resources.add((Integer)node.getValue());
                    nodes.add(node);
                } else {
                    continue;
                }

            }

            // Create lockable object
            MRLockable lockable = resourceAlloc.createLockable(resources);
            // Lock
            lockable.lock();

            // execute operations depending on random value
            int randOp = (int)(Math.random()*3);
            switch(randOp)
            {
                case 0: // some vertex operations
                    adjList.insertVertex(resources.get(1));
                    adjList.deleteVertex(resources.get(0));
                    adjList.find(resources.get(2));
                    break;

                case 1: // some edge operations
                    adjList.insertEdge(resources.get(1), resources.get(2));
                    adjList.insertEdge(resources.get(0), resources.get(1));
                    adjList.deleteEdge(resources.get(1), resources.get(2));
                    break;

                case 2: // both
                    adjList.insertEdge(resources.get(1), resources.get(2));
                    adjList.insertVertex(resources.get(0));
                    adjList.deleteEdge(resources.get(0), resources.get(1));
                    break;

            }
            lockable.unlock();
        }
    }
}