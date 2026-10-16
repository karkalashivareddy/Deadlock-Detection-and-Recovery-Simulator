package edu.team12.deadlocklab.api;
import edu.team12.deadlocklab.domain.*;import edu.team12.deadlocklab.detection.*;import edu.team12.deadlocklab.recovery.*;import edu.team12.deadlocklab.scenario.*;import org.springframework.web.bind.annotation.*;import java.util.*;
@RestController @RequestMapping("/api") public class ApiController {
 private final MatrixDetector detector=new MatrixDetector();private final RecoveryService recovery=new RecoveryService();
 @GetMapping("/health") public Map<String,Object>health(){return Map.of("status","ok","version","1.0.0");}
 @GetMapping("/scenarios") public List<Scenario>scenarios(){return ScenarioLibrary.all();}
 @GetMapping("/scenarios/{id}") public Scenario scenario(@PathVariable String id){return ScenarioLibrary.byId(id);}
 @PostMapping("/detect") public DetectionResult detect(@RequestBody Scenario s){return detector.detect(s);}
 @PostMapping("/recover") public Map<String,Object>recover(@RequestBody RecoveryRequest r){Scenario updated=switch(r.action()){case "terminate"->recovery.terminate(r.scenario(),r.process());case "preempt"->recovery.preempt(r.scenario(),r.process(),r.resource());case "rollback"->recovery.rollback(r.checkpoint());default->throw new IllegalArgumentException("action must be terminate, preempt, or rollback");};return Map.of("scenario",updated,"detection",detector.detect(updated));}
 public record RecoveryRequest(String action,Scenario scenario,String process,String resource,Scenario checkpoint){}
}
