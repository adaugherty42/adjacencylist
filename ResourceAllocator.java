import java.util.BitSet;
import java.util.Vector;

// resource manager
public class ResourceAllocator {
    MRLock lock;

    // takes the lock and the number of resources
    public ResourceAllocator(MRLock lock, int numResources) {
        this.lock = lock;
        this.lock.init(numResources);
    }

    // creates a lockable object by setting the resource bitset
    // and returning the new lockable object with an initialized resource set
    MRLockable createLockable(Vector<Integer> resources) {
        BitSet resourceMask = new BitSet(lock.buffer.length);
        for (int i = 0; i < resources.size(); i++) {
            resourceMask.set(resources.get(i));
        }
        return new MRLockable(lock, resourceMask);
    }
}

// MRLockable object has the lock, lock handle, and the resource bitset mask
class MRLockable {
    MRLock lock;
    int lockHandle;
    BitSet resourceMask;

    public MRLockable(MRLock lock, BitSet resourceMask) {
        this.lock = lock;
        this.resourceMask = resourceMask;
    }

    void lock() {
        lockHandle = lock.acquire_lock(resourceMask);
    }

    void unlock() {
        lock.release_lock(lockHandle);
    }
}
