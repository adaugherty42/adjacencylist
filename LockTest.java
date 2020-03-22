import java.util.ArrayList;
import java.util.BitSet;

public class LockTest {
    static int NUM_THREADS = 4;
    static int NUM_RESOURCES = 32;
    static int NUM_ITERATIONS = 10_000;

    public static void main(String[] args) {
        // Grab args if they exist
        if (args[1] != null) {
            NUM_THREADS = Integer.parseInt(args[1]);
        }
        if (args[2] != null) {
            NUM_RESOURCES = Integer.parseInt(args[2]);
        }
        if (args[3] != null) {
            NUM_ITERATIONS = Integer.parseInt(args[3]);
        }

        MRLock lock = new MRLock();
        // This ResourceAllocator will be passed to the spawned threads, which will use it to
        // create Lockable objects for their requests.
        ResourceAllocator resourceAlloc = new ResourceAllocator(lock, NUM_RESOURCES);

        // Create and start threads
        ArrayList<Thread> threads = new ArrayList<Thread>(NUM_THREADS);
        for (int i = 0; i < NUM_THREADS; i++) {
            threads.add(new Thread(new LockThread()));
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
    }
}

class LockThread implements Runnable {

    @Override
    public void run() {

    }
}