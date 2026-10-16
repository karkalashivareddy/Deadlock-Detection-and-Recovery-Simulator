package edu.team12.deadlocklab.api;
import org.springframework.http.*;import org.springframework.web.bind.annotation.*;import java.util.*;
import org.springframework.http.converter.HttpMessageNotReadableException;
@RestControllerAdvice public class ApiErrors {
 @ExceptionHandler(IllegalArgumentException.class) public ResponseEntity<Map<String,Object>>invalid(IllegalArgumentException e){return ResponseEntity.badRequest().body(Map.of("error","validation_error","message",Objects.toString(e.getMessage(),"Invalid request")));}
 @ExceptionHandler(NoSuchElementException.class) public ResponseEntity<Map<String,Object>>missing(NoSuchElementException e){return ResponseEntity.status(HttpStatus.NOT_FOUND).body(Map.of("error","not_found","message",e.getMessage()));}
 @ExceptionHandler(HttpMessageNotReadableException.class) public ResponseEntity<Map<String,Object>>unreadable(HttpMessageNotReadableException e){return ResponseEntity.badRequest().body(Map.of("error","invalid_json","message","Request body is missing or does not match the API schema"));}
}
