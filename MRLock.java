import java.util.BitSet;
import java.util.concurrent.atomic.AtomicInteger;

public class MRLock {
    Cell buffer[];
    int mask;
    AtomicInteger head;
    AtomicInteger tail;

    void init(int size) {
        this.buffer = new Cell[size];
        this.mask = size - 1;
        this.head = new AtomicInteger(0);
        this.tail = new AtomicInteger(0);

        for (int i = 0; i < size; i++) {
            this.buffer[i] = new Cell();
            this.buffer[i].bits = new BitSet(size);
            // initialize bitset to all 1s
            BitSet s = this.buffer[i].bits;
            for (int j = 0; j < size; j++) {
                s.set(j);
            }
            // initialize seq
            this.buffer[i].seq.set(i);
        }
    }

    void uninit() {
        this.buffer = new Cell[]{};
    }

    int acquire_lock(BitSet r) {
        Cell c;
        int pos;

        // enqueue our request
        for (;;) {
            pos = this.tail.get();
            c = this.buffer[pos & this.mask];
            Integer seq = c.seq.get();
            int diff = seq - pos;
            if (diff == 0) {
                if (this.tail.compareAndSet(pos, pos + 1)) {
                    break;
                }
            }
        }

        c.bits = r;
        c.seq.set(pos + 1);

        // spin while conflicts exist
        int spin_pos = this.head.get();
        while (spin_pos != pos) {
            BitSet conflictBitset = (BitSet)this.buffer[spin_pos & this.mask].bits.clone();
            conflictBitset.and(r);
            if (pos + this.buffer[spin_pos & this.mask].seq.get() > this.mask ||
                        !(conflictBitset.cardinality() > 0)) {
                spin_pos++;
            }
        }

        return pos;
    }

    void release_lock(int h) {
        this.buffer[h & this.mask].bits.clear();
        int pos = this.head.get();
        while (this.buffer[pos & this.mask].bits.cardinality() == 0) {
            Cell c = this.buffer[pos & this.mask];
            int seq = c.seq.get();
            int diff = seq - (pos + 1);

            if (diff == 0) {
                if (this.head.compareAndSet(pos, pos + 1)) {
                    for (int i = 0; i < this.buffer.length; i++) {
                        c.bits.set(i);
                    }
                    c.seq.set(pos + this.mask + 1);
                }
            }
            pos = this.head.get();
        }
    }
}

class Cell {
    AtomicInteger seq;
    BitSet bits;

    public Cell() {
        this.seq = new AtomicInteger(0);
        this.bits = new BitSet();
    }

    public Cell(AtomicInteger seq, BitSet bits) {
        this.seq = seq;
        this.bits = bits;
    }

    public String toString() {
        return "Sequence number: " + seq.get() + "\n"
                + "Bits: " + bits.toString();
    }
}