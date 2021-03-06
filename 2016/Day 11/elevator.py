from __future__ import annotations
from copy import deepcopy
from functools import lru_cache
from itertools import combinations, chain

RTG = tuple[str,str]
FloorMap = list[set[RTG]]

FAIL = -1
SUCCESS = 0
FINISHED = 1

MAX_INT64 = (1 << 63) - 1

floors:FloorMap = (
    {("promethium", "generator"), ("promethium", "microchip")},
    {("cobalt", "generator"), ("curium", "generator"), ("ruthenium", "generator"), ("plutonium", "generator")},
    {("cobalt", "microchip"), ("curium", "microchip"), ("ruthenium", "microchip"), ("plutonium", "microchip")},
    set()
)

# @lru_cache
def place(item:RTG, source:int, destination:int, floors_state:FloorMap, validate:bool=False) -> tuple[bool,FloorMap]:
    
    floors_state = deepcopy(floors_state)
    
    floors_state[source].remove(item)
    floors_state[destination].add(item)
    element, type_ = item

    if not validate: return (True, floors_state)

    if type_ == "microchip":
        my_generator = (element, "generator")
        for dest_step in range(source, destination+1):
            if my_generator not in floors_state[dest_step]:
                for floor_item in floors_state[dest_step]:
                    if floor_item[1] == "generator": return (False, floors_state)
    
    elif type_ == "generator":
        my_microchip = (element, "microchip")
        for dest_step in range(source, destination+1):
            for floor_item in floors_state[dest_step]:
                if (floor_item[1] == "microchip") and (floor_item != my_microchip):
                    other_generator = (floor_item[0], "generator")
                    if other_generator not in floors_state[dest_step]:
                        return (False, floors_state)

    else:
        raise TypeError()
    
    return (True, floors_state)

# @lru_cache
def move(slot_1:RTG, slot_2:RTG|None, source:int, destination:int, floors_state:FloorMap) -> int:

    floors_state = deepcopy(floors_state)
    
    if slot_2 is not None:
        success, floor_output = place(slot_1, source, destination, floors_state, validate=False)
        success, floor_output = place(slot_2, source, destination, floor_output, validate=True)
    else:
        success, floor_output = place(slot_1, source, destination, floors_state, validate=True)
    
    if not success:
        return (FAIL, floor_output)
    
    else:
        for floor in range(len(floor_output)-1):
            if len(floors_state[floor]) != 0:
                return (SUCCESS, floor_output)
        return (FINISHED, floor_output)

min_steps = 119
counter = 0

# @lru_cache
def run_puzzle(floors_state:FloorMap, steps_count=1, history:list[FloorMap]=None):
    global min_steps, counter

    if steps_count > min_steps: return

    if history is None:
        history = [floors_state]
    else:
        history.append(floors_state)
    
    for floor_num in range(len(floors_state)):
        floor = floors_state[floor_num]
        if len(floor) == 0: continue
        
        other_floors = set(range(len(floors_state))) - {floor_num}
        results:list[tuple[int,FloorMap]] = []
        
        for item in floor:
            results += [(move(item, None, floor_num, destination, floors_state) for destination in other_floors)]
        
        if len(floor) > 1:
            for item_1, item_2 in combinations(floor, 2):
                results += [(move(item_1, item_2, floor_num, destination, floors_state) for destination in other_floors)]
        
        for status, new_state in chain.from_iterable(results):
            if status == FAIL: continue
            
            if status == FINISHED:
                min_steps = min(steps_count, min_steps)
                continue
            
            if new_state not in history:
                run_puzzle(new_state, steps_count+1, history.copy())
        
        counter += 1
        if counter % 100 == 0:
            print(steps_count)

run_puzzle(floors)
print(min_steps)