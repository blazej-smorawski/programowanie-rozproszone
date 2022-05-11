package Classes.Utils;

import Classes.Node;
import Classes.Queue;

public class dNode {
    public Node node;
    public long mili_time;
    public Queue queue_to_the_node;

    public dNode(Node node, long mili_time, Queue queue_to_the_node) {
        this.node = node;
        this.mili_time = mili_time;
        this.queue_to_the_node = queue_to_the_node;
    }
}