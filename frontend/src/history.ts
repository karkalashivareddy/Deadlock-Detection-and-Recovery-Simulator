import type {Detection, Scenario, SimulationEvent, TraceStep} from './types';

const fail=(message:string):never=>{throw new Error(message)};
const record=(value:unknown):value is Record<string,unknown>=>!!value&&typeof value==='object'&&!Array.isArray(value);
const exactKeys=(value:Record<string,unknown>,required:string[],optional:string[]=[]):boolean=>required.every(key=>Object.hasOwn(value,key))&&Object.keys(value).every(key=>required.includes(key)||optional.includes(key));
const identifier=(value:unknown):value is string=>typeof value==='string'&&/^[A-Za-z][A-Za-z0-9_-]{0,62}$/.test(value);
const counts=(value:unknown,length:number):value is number[]=>Array.isArray(value)&&value.length===length&&value.every(item=>Number.isSafeInteger(item)&&item>=0&&item<=2147483647);

export function parseScenario(value:unknown):Scenario{
 if(!record(value))return fail('Scenario must be a JSON object');
 const keys=['id','processes','resources','total','available','allocation','request','preemptible','terminated'];
 if(!exactKeys(value,keys)||!identifier(value.id))return fail('Scenario has missing, unsupported, or invalid fields');
 const processes=value.processes,resources=value.resources;
 if(!Array.isArray(processes)||processes.length>32||!processes.every(identifier)||new Set(processes).size!==processes.length)return fail('Process identifiers are invalid or duplicated');
 if(!Array.isArray(resources)||resources.length>16||!resources.every(identifier)||new Set(resources).size!==resources.length)return fail('Resource identifiers are invalid or duplicated');
 const p=processes.length,r=resources.length;
 const matrix=(input:unknown):input is number[][]=>Array.isArray(input)&&input.length===p&&input.every(row=>counts(row,r));
 if(!counts(value.total,r)||!counts(value.available,r)||!matrix(value.allocation)||!matrix(value.request)||!Array.isArray(value.preemptible)||value.preemptible.length!==r||!value.preemptible.every(item=>typeof item==='boolean')||!Array.isArray(value.terminated)||!value.terminated.every(identifier)||new Set(value.terminated).size!==value.terminated.length||value.terminated.some(id=>!processes.includes(id)))return fail('Scenario matrices or resource fields have invalid dimensions or types');
 for(let i=0;i<p;i++)if(value.terminated.includes(processes[i])&&(value.allocation[i].some(n=>n!==0)||value.request[i].some(n=>n!==0)))return fail('Terminated processes must have zero live allocation and request');
 for(let j=0;j<r;j++){let sum=value.available[j];for(let i=0;i<p;i++)if(!value.terminated.includes(processes[i]))sum+=value.allocation[i][j];if(sum!==value.total[j])return fail(`Resource accounting invariant failed for ${resources[j]}`)}
 return {id:value.id,processes:[...processes],resources:[...resources],total:[...value.total],available:[...value.available],allocation:value.allocation.map(row=>[...row]),request:value.request.map(row=>[...row]),preemptible:[...value.preemptible],terminated:[...value.terminated]};
}

function parseDetection(value:unknown,scenario:Scenario):Detection{
 if(!record(value)||!exactKeys(value,['deadlocked','trace','finalWork']))return fail('Replay detection result has invalid fields');
 if(!Array.isArray(value.deadlocked)||!value.deadlocked.every(id=>typeof id==='string'&&scenario.processes.includes(id))||new Set(value.deadlocked).size!==value.deadlocked.length||!counts(value.finalWork,scenario.resources.length)||!Array.isArray(value.trace))return fail('Replay detection result has invalid dimensions');
 const trace:TraceStep[]=value.trace.map((item)=>{
  if(!record(item)||!exactKeys(item,['process','workBefore','request','componentFits','eligible','releasedAllocation','workAfter','decision']))return fail('Replay trace step has invalid fields');
  const process=item.process;
  if(typeof process!=='string'||!scenario.processes.includes(process)||!counts(item.workBefore,scenario.resources.length)||!counts(item.request,scenario.resources.length)||!Array.isArray(item.componentFits)||item.componentFits.length!==scenario.resources.length||!item.componentFits.every(v=>typeof v==='boolean')||typeof item.eligible!=='boolean'||!counts(item.releasedAllocation,scenario.resources.length)||!counts(item.workAfter,scenario.resources.length)||typeof item.decision!=='string')return fail('Replay trace step has invalid values');
  return {process,workBefore:[...item.workBefore],request:[...item.request],componentFits:[...item.componentFits],eligible:item.eligible,releasedAllocation:[...item.releasedAllocation],workAfter:[...item.workAfter],decision:item.decision};
 });
 return {deadlocked:[...value.deadlocked] as string[],trace,finalWork:[...value.finalWork]};
}

export function parseReplayRecord(value:unknown):{scenarioId:string;events:SimulationEvent[]}{
 if(!record(value)||!exactKeys(value,['schemaVersion','scenarioId','events'])||value.schemaVersion!==1||!identifier(value.scenarioId)||!Array.isArray(value.events)||value.events.length<1||value.events.length>100)return fail('Replay record must be a version 1 object with 1–100 events');
 let previousSequence=0;
 const events:SimulationEvent[]=value.events.map((item)=>{
  if(!record(item)||!exactKeys(item,['sequence','type','scenarioId','label','before','after'],['detection']))return fail('Replay event has missing or unsupported fields');
  if(!Number.isSafeInteger(item.sequence)||Number(item.sequence)<=previousSequence||typeof item.scenarioId!=='string'||!identifier(item.scenarioId)||typeof item.label!=='string'||item.label.length>160||!['scenario_change','detection','recovery'].includes(String(item.type)))return fail('Replay event has invalid sequence, type, or label');
  previousSequence=Number(item.sequence);
  const after=parseScenario(item.after),before=item.before===null?null:parseScenario(item.before);
  if(item.scenarioId!==after.id)return fail('Replay event scenario identity does not match its after-state');
  const detection=item.detection===undefined?undefined:parseDetection(item.detection,after);
  if(item.type==='detection'&&!detection)return fail('Detection events must include their recorded result');
  return {sequence:previousSequence,type:item.type as SimulationEvent['type'],scenarioId:item.scenarioId,label:item.label,before,after,...(detection?{detection}:{})};
 });
 return {scenarioId:value.scenarioId,events};
}

function rejectDuplicateKeys(text:string):void{
 let index=0;
 const white=()=>{while(index<text.length&&/\s/.test(text[index]))index++};
 const string=():string=>{const start=index++;while(index<text.length){if(text[index]==='\\'){index+=2;continue}if(text[index++]==='"')return JSON.parse(text.slice(start,index)) as string}return fail('Replay JSON contains an unterminated string')};
 const value=():void=>{white();if(text[index]==='{'){index++;white();const keys=new Set<string>();if(text[index]==='}'){index++;return}while(index<text.length){white();if(text[index]!=='"')return;const key=string();if(keys.has(key))fail(`Replay JSON contains duplicate object key: ${key}`);keys.add(key);white();index++;value();white();if(text[index]==='}'){index++;return}index++}return}if(text[index]==='['){index++;white();if(text[index]===']'){index++;return}while(index<text.length){value();white();if(text[index]===']'){index++;return}index++}return}if(text[index]==='"'){string();return}while(index<text.length&&!/[\s,}\]]/.test(text[index]))index++};
 value();
}

export function parseReplayJson(text:string):{scenarioId:string;events:SimulationEvent[]}{
 if(text.length>2_000_000)return fail('Replay file exceeds the 2 MB import limit');
 let parsed:unknown;
 try{parsed=JSON.parse(text)}catch{return fail('Replay file is not valid JSON')}
 rejectDuplicateKeys(text);
 return parseReplayRecord(parsed);
}
