from collections import Counter, deque

with open("input.txt", "rt") as file:
    raw_data = [line.strip() for line in file]

# Part 1
counters = [Counter() for _ in range(12)]
for i in range(12):
    for line in raw_data:
        counters[i].update((line[~i],))

gamma = 0
epsilon = 0
for j in range(12):
    most_common = counters[j].most_common()
    gamma |= int(most_common[0][0]) << j
    epsilon |= int(most_common[1][0]) << j

print(gamma * epsilon)

# Part 2

data_dict = {ID: value for ID, value in enumerate(raw_data)}

def process_data(data:dict, mode:str) -> int:    
    my_data = data.copy()

    for i in range(12):
        bit_count = Counter()
        to_remove = (deque(), deque())
        
        for ID, value in my_data.items():
            bit = int(value[i])
            bit_count.update((bit,))
            to_remove[bit].append(ID)
        
        if mode == "most":
            most = 1 if bit_count[1] >= bit_count[0] else 0
            while to_remove[most]:
                index = to_remove[most].popleft()
                del my_data[index]
                if len(my_data) == 1: break

        elif mode == "least":
            least = 0 if bit_count[0] <= bit_count[1] else 1
            while to_remove[least]:
                index = to_remove[least].popleft()
                del my_data[index]
                if len(my_data) == 1: break
        
        if len(my_data) == 1:
            (ID, value), = my_data.items()
            return int(value, 2)

O2_value = process_data(data_dict, "most")
CO2_value = process_data(data_dict, "least")

print(O2_value * CO2_value)