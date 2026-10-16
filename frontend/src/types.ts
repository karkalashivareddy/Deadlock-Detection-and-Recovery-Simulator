export type Scenario={id:string;processes:string[];resources:string[];total:number[];available:number[];allocation:number[][];request:number[][];preemptible:boolean[];terminated:string[]};
export type TraceStep={process:string;workBefore:number[];request:number[];componentFits:boolean[];eligible:boolean;releasedAllocation:number[];workAfter:number[];decision:string};
export type Detection={deadlocked:string[];trace:TraceStep[];finalWork:number[]};
