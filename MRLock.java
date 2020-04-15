import java.util.BitSet;
import java.util.concurrent.atomic.AtomicInteger;

// actual MRLOCK algorithm
public class MRLock {
    Cell buffer[];
    int mask;
    AtomicInteger head;
    AtomicInteger tail;

    // initialize the lock such that the head and tail
    // are atomic, the mask is set to one less than the size
    // to resolve bitwise operations
    void init(int size) {
        this.buffer = new Cell[size];
        this.mask = size - 1;
        this.head = new AtomicInteger(0);
        this.tail = new AtomicInteger(0);

        // initialize the BitSet must be done manually in Java
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

    // deallocate memory by clearing the cell
    void uninit() {
        this.buffer = new Cell[] {};
    }

    // acquire the lock
    int acquire_lock(BitSet r) {
        Cell c;
        int pos;

        // enqueue our request while spinning
        // gets a cell at the tail of the queue
        for (;;) {
            pos = this.tail.get();
            c = this.buffer[pos & this.mask];
            // sets sequence number
            // for FIFO ordering
            Integer seq = c.seq.get();
            int diff = seq - pos;
            // when difference is 0,
            // enqueue onto the tail
            if (diff == 0) {
                if (this.tail.compareAndSet(pos, pos + 1)) {
                    break;
                }
            }
        }

        // set the bits and progress in the queue
        c.bits = r;
        c.seq.set(pos + 1);

        // spin while conflicts exist
        int spin_pos = this.head.get();
        while (spin_pos != pos) {
            // uses bit masking to determine conflicts
            BitSet conflictBitset = (BitSet) this.buffer[spin_pos & this.mask].bits.clone();
            conflictBitset.and(r);
            // while waiting on its turn in the sequence or if there is conflict over resources
            // spin and wait
            if (pos + this.buffer[spin_pos & this.mask].seq.get() > this.mask || !(conflictBitset.cardinality() > 0)) {
                spin_pos++;
            }
        }

        return pos;
    }

    // release lock
    void release_lock(int h) {
        // clear resource bits to 0 to indicate no more resources are necessary
        this.buffer[h & this.mask].bits.clear();
        int pos = this.head.get();
        // continue through the queue
        while (this.buffer[pos & this.mask].bits.cardinality() == 0) {
            Cell c = this.buffer[pos & this.mask];
            int seq = c.seq.get();
            int diff = seq - (pos + 1);

            // if it is the thread's turn, execute the next instruction
            // by setting bit resources to that of the previous
            // to help other threads in the queue execute faster
            // and boost performance
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

// cell class contains the sequence number of the order it is called
// and the bitset that marks which resources the operation/thread requires
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