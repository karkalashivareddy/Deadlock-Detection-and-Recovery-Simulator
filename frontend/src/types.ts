export type Scenario={id:string;processes:string[];resources:string[];total:number[];available:number[];allocation:number[][];request:number[][];preemptible:boolean[];terminated:string[]};
export type TraceStep={process:string;workBefore:number[];request:number[];componentFits:boolean[];eligible:boolean;releasedAllocation:number[];workAfter:number[];decision:string};
export type Detection={deadlocked:string[];trace:TraceStep[];finalWork:number[]};
export type BankerScenario={id:string;processes:string[];resources:string[];total:number[];available:number[];allocation:number[][];maximum:number[][]};
export type BankerTraceStep={process:string;workBefore:number[];need:number[];eligible:boolean;workAfter:number[]};
export type BankerResult={algorithm:'banker_safety';safe:boolean;interpretation:string;available:number[];sequence:string[];unfinished:string[];trace:BankerTraceStep[]};
