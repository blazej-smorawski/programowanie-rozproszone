package Classes;

import java.util.ArrayList;

public class Queue {
    // Final
    public final Node from;
    public final Node to;
    public final long mili_time;
    private final int max_capacity;
    // Runtime
    public long average_time;
    private int capacity;
    private int leave_count;
    private final ArrayList<Long> times;

    public Queue(Node from, Node to, long time, int max_capacity) {
        this.from = from;
        this.to = to;
        this.mili_time = time;
        this.max_capacity = max_capacity;

        this.capacity = 0;
        this.average_time = time;
        this.leave_count = 0;
        this.times = new ArrayList<>();
    }

    public void enter(Agent agent) throws InterruptedException {
        agent.enter_last_queue = System.nanoTime();
        synchronized (this){
            if (capacity == max_capacity) {
                wait();
            }
            capacity++;
        }
        Access(agent);
    }

    private void Access(Agent agent) throws InterruptedException {
        Thread.sleep(mili_time);
        Leave(agent);
    }

    public void Leave(Agent agent) {
        synchronized (this) {
            capacity--;
            notify();
            leave_count++;
            times.add(System.currentTimeMillis()-agent.enter_last_queue);

            average_time = 0;
            for (long t : times) {
                average_time += t;
            }
            average_time /= times.size();
        }
    }
}
