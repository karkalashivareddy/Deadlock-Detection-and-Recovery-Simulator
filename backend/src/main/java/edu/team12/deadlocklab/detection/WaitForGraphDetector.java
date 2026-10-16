package edu.team12.deadlocklab.detection;
import java.util.*;
public final class WaitForGraphDetector {
 public boolean hasCycle(Map<String,? extends Collection<String>>graph){Set<String>visiting=new HashSet<>(),done=new HashSet<>();for(String node:graph.keySet())if(cycle(node,graph,visiting,done))return true;return false;}
 private boolean cycle(String node,Map<String,? extends Collection<String>>g,Set<String>visiting,Set<String>done){if(visiting.contains(node))return true;if(done.contains(node))return false;visiting.add(node);Collection<String>next=g.containsKey(node)?g.get(node):List.of();for(String n:next)if(cycle(n,g,visiting,done))return true;visiting.remove(node);done.add(node);return false;}
}
