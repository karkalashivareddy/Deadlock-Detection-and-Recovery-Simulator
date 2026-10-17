import {useRef,type ChangeEvent} from 'react';
import {parseReplayJson} from '../history';
import type {SimulationEvent} from '../types';

export default function ReplayImport({onImport,onError}:{onImport:(scenarioId:string,events:SimulationEvent[])=>void;onError:(message:string)=>void}){
 const input=useRef<HTMLInputElement>(null);
 const read=(file:File)=>new Promise<string>((resolve,reject)=>{const reader=new FileReader();reader.onload=()=>resolve(String(reader.result??''));reader.onerror=()=>reject(new Error('Unable to read replay file'));reader.readAsText(file)});
 async function load(event:ChangeEvent<HTMLInputElement>){
  const file=event.target.files?.[0];event.target.value='';if(!file)return;
  try{const text=await read(file);const replay=parseReplayJson(text);onImport(replay.scenarioId,replay.events)}catch(error){onError(error instanceof Error?error.message:'Replay import failed')}
 }
 return <><button className="button secondary" onClick={()=>input.current?.click()}>Import replay</button><input ref={input} aria-label="Import replay JSON" type="file" accept="application/json,.json" hidden onChange={load}/></>;
}
