import {fireEvent,render,screen} from '@testing-library/react';
import {beforeEach,describe,expect,it,vi} from 'vitest';
import type {Scenario} from '../types';
import WaitForPanel from './WaitForPanel';

const analyze=vi.hoisted(()=>vi.fn());
vi.mock('../api',()=>({api:{waitFor:analyze}}));
const scenario:Scenario={id:'deadlock',processes:['P1','P2'],resources:['R1','R2'],total:[1,1],available:[0,0],allocation:[[1,0],[0,1]],request:[[0,1],[1,0]],preemptible:[false,false],terminated:[]};

describe('single-instance wait-for panel',()=>{
 beforeEach(()=>analyze.mockReset());
 it('sends applicable states and labels a returned process cycle',async()=>{analyze.mockResolvedValue({algorithm:'single_instance_wait_for_cycle',assumption:'exactly one instance',hasCycle:true,cycle:['P1','P2','P1']});render(<WaitForPanel scenario={scenario}/>);fireEvent.click(screen.getByRole('button',{name:'Analyze wait-for graph'}));expect(await screen.findByText('Cycle found · deadlock under this model')).toBeInTheDocument();expect(screen.getByText('P1 → P2 → P1')).toBeInTheDocument();expect(analyze).toHaveBeenCalledWith(scenario);});
 it('disables the single-instance detector for multiple instances',()=>{render(<WaitForPanel scenario={{...scenario,total:[2,1]}}/>);expect(screen.getByRole('button',{name:'Analyze wait-for graph'})).toBeDisabled();expect(screen.getByText(/every resource type has exactly one instance/)).toBeInTheDocument();});
});
