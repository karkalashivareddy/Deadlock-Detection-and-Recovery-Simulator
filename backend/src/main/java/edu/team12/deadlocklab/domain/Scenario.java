package edu.team12.deadlocklab.domain;
import java.util.*;
public record Scenario(String id, List<String> processes, List<String> resources, List<Integer> total, List<Integer> available, List<List<Integer>> allocation, List<List<Integer>> request, List<Boolean> preemptible, List<String> terminated) {
 public Scenario { processes=List.copyOf(processes);resources=List.copyOf(resources);total=List.copyOf(total);available=List.copyOf(available);allocation=allocation.stream().map(List::copyOf).toList();request=request.stream().map(List::copyOf).toList();preemptible=List.copyOf(preemptible);terminated=List.copyOf(terminated); }
 public Scenario withState(List<String> ps,List<Integer> av,List<List<Integer>> al,List<List<Integer>> req,List<String> dead){return new Scenario(id,ps,resources,total,av,al,req,preemptible,dead);}
}
