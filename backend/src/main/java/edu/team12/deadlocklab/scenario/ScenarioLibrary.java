package edu.team12.deadlocklab.scenario;
import edu.team12.deadlocklab.domain.Scenario;import java.util.*;
public final class ScenarioLibrary {
 private ScenarioLibrary(){}
 private static Scenario s(String id,List<String>p,List<String>r,List<Integer>t,List<Integer>a,List<List<Integer>>al,List<List<Integer>>q,List<Boolean>pre){return new Scenario(id,p,r,t,a,al,q,pre,List.of());}
 public static List<Scenario> all(){return List.of(noDeadlock(),deadlock(),multiInstance(),zeroHolder(),recoveryCase());}
 public static Scenario byId(String id){return all().stream().filter(s->s.id().equals(id)).findFirst().orElseThrow(()->new NoSuchElementException("scenario not found: "+id));}
 public static Scenario noDeadlock(){return s("no-deadlock",List.of("P1","P2"),List.of("R1","R2"),List.of(2,1),List.of(1,0),List.of(List.of(1,0),List.of(0,1)),List.of(List.of(1,0),List.of(0,0)),List.of(true,true));}
 public static Scenario deadlock(){return s("two-process-deadlock",List.of("P1","P2"),List.of("R1","R2"),List.of(1,1),List.of(0,0),List.of(List.of(1,0),List.of(0,1)),List.of(List.of(0,1),List.of(1,0)),List.of(false,false));}
 public static Scenario multiInstance(){return s("multi-instance",List.of("P1","P2","P3"),List.of("R1"),List.of(3),List.of(0),List.of(List.of(1),List.of(1),List.of(1)),List.of(List.of(1),List.of(1),List.of(0)),List.of(true));}
 public static Scenario zeroHolder(){return s("zero-allocation",List.of("P1","P2"),List.of("R1"),List.of(1),List.of(0),List.of(List.of(0),List.of(1)),List.of(List.of(1),List.of(1)),List.of(true));}
 public static Scenario recoveryCase(){return s("recovery-case",List.of("P1","P2"),List.of("R1","R2"),List.of(1,1),List.of(0,0),List.of(List.of(1,0),List.of(0,1)),List.of(List.of(0,1),List.of(1,0)),List.of(false,false));}
}
