import {fireEvent,render,screen} from '@testing-library/react';
import {beforeEach,describe,expect,it,vi} from 'vitest';
import type {ExperimentResult} from '../types';
import ExperimentPanel from './ExperimentPanel';

const experiment=vi.hoisted(()=>vi.fn());
vi.mock('../api',()=>({api:{experiment}}));
const result:ExperimentResult={seed:73012,algorithmVersion:'xorshift32-v1',scenario:{id:'experiment-73012',processes:['P1'],resources:['R1'],total:[1],available:[1],allocation:[[0]],request:[[0]],preemptible:[true],terminated:[]},detection:{deadlocked:[],trace:[],finalWork:[1]},metrics:{detectionEvaluations:1,unfinishedProcesses:0}};

describe('seeded experiment panel',()=>{
 beforeEach(()=>experiment.mockReset());
 it('calls the real experiment API and can load the returned scenario',async()=>{experiment.mockResolvedValue(result);const onLoad=vi.fn();render(<ExperimentPanel onLoad={onLoad}/>);fireEvent.click(screen.getByRole('button',{name:'Generate & detect'}));expect(await screen.findByText(/xorshift32-v1/)).toBeInTheDocument();expect(experiment).toHaveBeenCalledWith(73012,5,3);fireEvent.click(screen.getByRole('button',{name:'Load into scenario editor'}));expect(onLoad).toHaveBeenCalledWith(result.scenario);});
 it('rejects out-of-bounds dimensions before making a request',()=>{render(<ExperimentPanel onLoad={vi.fn()}/>);fireEvent.change(screen.getByLabelText('Experiment process count'),{target:{value:'13'}});fireEvent.click(screen.getByRole('button',{name:'Generate & detect'}));expect(screen.getByRole('alert')).toHaveTextContent('1–12 processes');expect(experiment).not.toHaveBeenCalled();});
});
