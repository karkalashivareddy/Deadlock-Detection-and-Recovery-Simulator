package edu.team12.deadlocklab;
import org.junit.jupiter.api.Test;import org.springframework.beans.factory.annotation.Autowired;import org.springframework.boot.test.autoconfigure.web.servlet.AutoConfigureMockMvc;import org.springframework.boot.test.context.SpringBootTest;import org.springframework.test.web.servlet.MockMvc;import static org.springframework.test.web.servlet.request.MockMvcRequestBuilders.*;import static org.springframework.test.web.servlet.result.MockMvcResultMatchers.*;
@SpringBootTest @AutoConfigureMockMvc class ApiIntegrationTest {
 @Autowired MockMvc mvc;
 @Test void healthAndFixtureEndpoints()throws Exception{mvc.perform(get("/api/health")).andExpect(status().isOk()).andExpect(jsonPath("$.status").value("ok"));mvc.perform(get("/api/scenarios/two-process-deadlock")).andExpect(status().isOk()).andExpect(jsonPath("$.processes.length()").value(2));}
 @Test void detectsDeadlockAndRejectsMalformedJson()throws Exception{String body="""
 {"id":"case","processes":["P1","P2"],"resources":["R1","R2"],"total":[1,1],"available":[0,0],"allocation":[[1,0],[0,1]],"request":[[0,1],[1,0]],"preemptible":[false,false],"terminated":[]}
 """;mvc.perform(post("/api/detect").contentType("application/json").content(body)).andExpect(status().isOk()).andExpect(jsonPath("$.deadlocked.length()").value(2));mvc.perform(post("/api/detect").contentType("application/json").content("{" )).andExpect(status().isBadRequest()).andExpect(jsonPath("$.error").value("invalid_json"));}
}
