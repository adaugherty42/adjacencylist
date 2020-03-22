import java.util.BitSet;
import java.util.Vector;

public class ResourceAllocator {
    MRLock lock;

    public ResourceAllocator(MRLock lock, int numResources) {
        this.lock = lock;
        this.lock.init(numResources);
    }

    MRLockable createLockable(Vector<Integer> resources) {
        BitSet resourceMask = new BitSet(lock.buffer.length);
        for (int i = 0; i < resources.size(); i++) {
            resourceMask.set(resources.get(i));
        }
        return new MRLockable(lock, resourceMask);
    }
}

class MRLockable {
    MRLock lock;
    int lockHandle;
    BitSet resourceMask;

    public MRLockable(MRLock lock, BitSet resourceMask) {
        this.lock = lock;
        this.resourceMask = resourceMask;
    }

    int lock() {
        return lock.acquire_lock(resourceMask);
    }

    void unlock() {
        lock.release_lock(lockHandle);
    }
}
