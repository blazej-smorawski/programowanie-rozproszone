package Classes;

import Classes.Utils.dNode;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

public class Agent implements Runnable {
    public double cumulative_time_spent;
    public long enter_last_queue;

    private final List<Queue> queues;
    private final List<Node> nodes;
    private Node node;
    private Node end;

    public Agent(Node node, Node end, List<Node> nodes, List<Queue> queues) {
        this.node = node;
        this.end = end;
        this.nodes = nodes;
        this.queues = queues;
    }

    @Override
    public void run() {
        long start = System.nanoTime();
        while(node!=end) {
            Queue next_to_go = getNextOnPathToEnd();
            try {
                next_to_go.enter(this);
                node = next_to_go.to;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        long time = System.nanoTime()-start;
        cumulative_time_spent = (float)time/(float)1000000;
    }

    public Queue getNextOnPathToEnd() {
        Map<Node, dNode> q = new HashMap<>();
        Map<Node, dNode> c = new HashMap<>();
        for (Node n : nodes) {
            if (n == node) {
                q.put(n, new dNode(n, 0, null));
            } else {
                q.put(n, new dNode(n, Long.MAX_VALUE, null));
            }
        }

        while (!q.isEmpty()) {
            Node u = end;
            for (Node n : q.keySet()) {
                if (q.get(n).mili_time < q.get(u).mili_time) {
                    u = n;
                }
            }
            dNode du = q.get(u);
            if (u == end) {
                break;
            }
            c.put(u,du);
            q.remove(u);

            List<Queue> queues_from_u = new ArrayList<>();
            for (Queue qu : queues) {
                if (qu.from == u) {
                    queues_from_u.add(qu);
                }
            }
            for (Queue qu : queues_from_u) {
                dNode v = q.get(qu.to);
                if (v != null) {
                    long alt = du.mili_time + qu.average_time;
                    if (alt < v.mili_time) {
                        v.mili_time = alt;
                        v.queue_to_the_node = qu;
                    }
                }
            }
        }

        q.putAll(c);
        Node prev = end;
        Queue queue = null;
        do {
            queue = q.get(prev).queue_to_the_node;
            prev = queue.from;
        } while (queue.from != node);
        return queue;
    }
}
