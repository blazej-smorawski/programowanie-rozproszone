import Classes.Agent;
import Classes.Node;
import Classes.Queue;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class main {
    public static void main(String[] args) {
        int n = 1000;
        List<Node> nodes = Arrays.asList(
                new Node("A"),
                new Node("Y"),
                new Node("X"),
                new Node("B")
        );
        List<Queue> queues = Arrays.asList(
                new Queue(nodes.get(0), nodes.get(1), 50,2),
                new Queue(nodes.get(1), nodes.get(0), 50,2),
                new Queue(nodes.get(0), nodes.get(2), 10,4),
                new Queue(nodes.get(2), nodes.get(1), 10,4),
                new Queue(nodes.get(1), nodes.get(3), 10,4),
                new Queue(nodes.get(3), nodes.get(1), 10,4),
                new Queue(nodes.get(2), nodes.get(3), 50,2),
                new Queue(nodes.get(3), nodes.get(2), 50,2)
                //new Queue(nodes.get(1), nodes.get(2), 10,4)
        );

        List<Agent> agents = new ArrayList<>(n);
        List<Thread> threads = new ArrayList<>(n);
        for(int i=0;i<n;i++) {
            Agent a = new Agent(nodes.get(0), nodes.get(3), nodes, queues);
            Thread nt = new Thread(a);
            agents.add(a);
            threads.add(nt);
            nt.start();
        }
        for(Thread t:threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        double sum = 0;
        for(Agent a:agents) {
            //System.out.println("Time: " + a.cumulative_time_spent);
            sum += a.cumulative_time_spent;
        }
        System.out.println("Average Time: " + sum/n);
    }
}
