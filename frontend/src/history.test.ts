import {describe,expect,it} from 'vitest';
import {parseReplayJson,parseReplayRecord} from './history';

const scenario={id:'sample',processes:['P1'],resources:['R1'],total:[1],available:[1],allocation:[[0]],request:[[0]],preemptible:[true],terminated:[]};
const event={sequence:9,type:'scenario_change',scenarioId:'sample',label:'Fixture loaded',before:null,after:scenario};
const record={schemaVersion:1,scenarioId:'sample',events:[event]};

describe('versioned replay import',()=>{
 it('accepts a valid exported event history without changing its stored sequence',()=>{expect(parseReplayRecord(record)).toEqual({scenarioId:'sample',events:[event]})});
 it('rejects unsupported versions, unknown fields, and unordered events',()=>{
  expect(()=>parseReplayRecord({...record,schemaVersion:2})).toThrow(/version 1/);
  expect(()=>parseReplayRecord({...record,extra:true})).toThrow(/version 1/);
  expect(()=>parseReplayRecord({...record,events:[event,{...event,sequence:9}]})).toThrow(/sequence/);
 });
 it('rejects invalid state accounting and mismatched scenario identity',()=>{
  expect(()=>parseReplayRecord({...record,events:[{...event,after:{...scenario,available:[0]}}]})).toThrow(/accounting/);
  expect(()=>parseReplayRecord({...record,events:[{...event,scenarioId:'other'}]})).toThrow(/identity/);
 });
 it('requires a result on detection events and validates trace dimensions',()=>{
  expect(()=>parseReplayRecord({...record,events:[{...event,type:'detection'}]})).toThrow(/include their recorded result/);
  const detection={deadlocked:[],finalWork:[1],trace:[{process:'P1',workBefore:[1],request:[0],componentFits:[true],eligible:true,releasedAllocation:[0],workAfter:[1],decision:'eligible'}]};
  expect(parseReplayRecord({...record,events:[{...event,type:'detection',detection}]}).events[0].type).toBe('detection');
  expect(()=>parseReplayRecord({...record,events:[{...event,type:'detection',detection:{...detection,finalWork:[]}}]})).toThrow(/dimensions/);
 });
 it('rejects duplicate JSON keys, including escaped-equivalent names and oversized files',()=>{
  expect(()=>parseReplayJson('{"schemaVersion":1,"schemaVersion":1}')).toThrow(/duplicate object key/);
  expect(()=>parseReplayJson('{"a":1,"\\u0061":2}')).toThrow(/duplicate object key/);
  expect(()=>parseReplayJson(' '.repeat(2_000_001))).toThrow(/2 MB/);
 });
});
