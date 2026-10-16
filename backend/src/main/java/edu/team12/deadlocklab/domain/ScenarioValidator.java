package edu.team12.deadlocklab.domain;
import java.util.*;
public final class ScenarioValidator {
 private ScenarioValidator(){}
 public static List<String> errors(Scenario s){List<String> e=new ArrayList<>();if(s==null)return List.of("scenario is required");int p=s.processes().size(),r=s.resources().size();
  if(new HashSet<>(s.processes()).size()!=p||s.processes().stream().anyMatch(x->x==null||x.isBlank()))e.add("process identifiers must be non-empty and unique");
  if(new HashSet<>(s.resources()).size()!=r||s.resources().stream().anyMatch(x->x==null||x.isBlank()))e.add("resource identifiers must be non-empty and unique");
  if(s.total().size()!=r||s.available().size()!=r||s.preemptible().size()!=r)e.add("resource vector dimensions must equal resource count");if(s.allocation().size()!=p||s.request().size()!=p)e.add("matrix row count must equal process count");
  checkVector(s.total(),"total",e);checkVector(s.available(),"available",e);checkMatrix(s.allocation(),r,"allocation",e);checkMatrix(s.request(),r,"request",e);
  if(s.terminated().stream().anyMatch(x->!s.processes().contains(x))||new HashSet<>(s.terminated()).size()!=s.terminated().size())e.add("terminated identifiers must be unique and active");
  if(e.isEmpty())for(int j=0;j<r;j++){int sum=s.available().get(j);for(int i=0;i<p;i++)if(!s.terminated().contains(s.processes().get(i)))sum+=s.allocation().get(i).get(j);if(sum!=s.total().get(j))e.add("resource accounting invariant failed for "+s.resources().get(j));}return List.copyOf(e);}
 private static void checkVector(List<Integer>v,String label,List<String>e){if(v.stream().anyMatch(x->x==null||x<0))e.add(label+" counts must be non-negative integers");}
 private static void checkMatrix(List<List<Integer>>m,int r,String label,List<String>e){for(List<Integer>row:m){if(row.size()!=r)e.add(label+" column count must equal resource count");if(row.stream().anyMatch(x->x==null||x<0))e.add(label+" counts must be non-negative integers");}}
}
