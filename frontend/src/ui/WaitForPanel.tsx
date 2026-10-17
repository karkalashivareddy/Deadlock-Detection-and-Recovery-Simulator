import {useState} from 'react';
import {api} from '../api';
import type {Scenario,WaitForResult} from '../types';

export default function WaitForPanel({scenario}:{scenario:Scenario}){
 const[snapshot,setSnapshot]=useState<WaitForResult|null>(null),[error,setError]=useState(''),[busy,setBusy]=useState(false);
 const applicable=scenario.resources.length>0&&scenario.total.length===scenario.resources.length&&scenario.total.every(n=>n===1)&&scenario.request.every(row=>row.every(n=>n===0||n===1));
 async function analyze(){setBusy(true);setError('');try{setSnapshot(await api.waitFor(scenario))}catch(e){setSnapshot(null);setError(e instanceof Error?e.message:'Wait-for analysis failed')}finally{setBusy(false)}}
 return <section className="panel waitfor-panel"><div className="panel-head"><div><span className="eyebrow">SEPARATE GRAPH ALGORITHM</span><h2>Wait-for graph cycle analysis</h2><p>Derives process-to-process waits from pending requests and current resource holders.</p></div><span className="model-tag">SINGLE INSTANCE ONLY</span></div><p className="recovery-foot">A cycle proves deadlock under the single-instance model. This result does not apply to multi-instance resources.</p><button className="button secondary" disabled={!applicable||busy} onClick={()=>void analyze()}>{busy?'Analyzing…':'Analyze wait-for graph'}</button>{!applicable&&<p className="muted">Enable this analysis only when every resource type has exactly one instance and each request is 0 or 1.</p>}{error&&<p className="banker-error" role="alert">{error}</p>}{snapshot&&<div className={`waitfor-result ${snapshot.hasCycle?'is-unsafe':'is-safe'}`} role="status"><b>{snapshot.hasCycle?'Cycle found · deadlock under this model':'No wait-for cycle found'}</b><p>{snapshot.hasCycle?snapshot.cycle.join(' → '):'No directed process cycle was found.'}</p><small>{snapshot.assumption}</small></div>}</section>;
}
