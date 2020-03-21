import java.util.BitSet;
import java.util.concurrent.atomic.AtomicInteger;

public class MRLock {
    Cell buffer[];
    int mask;
    AtomicInteger head;
    AtomicInteger tail;

    void init(MRLock lock, int size) {
        lock.buffer = new Cell[size];
        lock.mask = size - 1;
        lock.head.set(0);
        lock.tail.set(0);

        for (int i = 0; i < size; i++) {
            // initialize bitset to all 1s
            BitSet s = lock.buffer[i].bits;
            for (int j = 0; j < s.length(); j++) {
                s.set(j);
            }
            // initialize seq
            lock.buffer[i].seq.set(i);
        }
    }

    void uninit(MRLock lock) {
        lock.buffer = new Cell[]{};
    }

    int acquire_lock(MRLock lock, BitSet r) {
        Cell c;
        int pos;

        for (;;) {
            pos = lock.tail.get();
            c = lock.buffer[pos & lock.mask];
            int seq = c.seq.get();
            int diff = seq - pos;

            if (diff == 0) {
                if (lock.tail.compareAndSet(pos, pos + 1)) {
                    break;
                }
            }
        }

        c.bits = r;
        c.seq.set(pos + 1);

        int spin = lock.head.get();
        while (spin != pos) {
            BitSet currBitset = (BitSet)lock.buffer[spin & lock.mask].bits;
            currBitset.and(r);
            if (pos - lock.buffer[spin & lock.mask].seq.get() > lock.mask ||
                        !(currBitset.cardinality() > 0)) {
                spin++;
            }
        }

        return pos;
    }

    void release_lock(MRLock lock, int h) {
        lock.buffer[h & lock.mask].bits.clear();
        int pos = lock.head.get();
        while (lock.buffer[pos & lock.mask].bits.cardinality() == 0) {
            Cell c = lock.buffer[pos & lock.mask];
            int seq = c.seq.get();
            int diff = seq - (pos + 1);

            if (diff == 0) {
                if (lock.head.compareAndSet(pos, pos + 1)) {
                    for (int i = 0; i < c.bits.length(); i++) {
                        c.bits.set(i);
                    }
                    c.seq.set(pos + lock.mask + 1);
                }
            }
        }

        pos = lock.head.get();
    }
}

class Cell {
    AtomicInteger seq;
    BitSet bits;

    public Cell(AtomicInteger seq, BitSet bits) {
        this.seq = seq;
        this.bits = bits;
    }
}