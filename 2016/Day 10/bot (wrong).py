# This script gives the wrong answer, and it's here for archiving purposes.
# 'bot2.py' is the script that gives the correct answer

with open("input.txt") as file:
    raw_commands = [tuple(line.split()) for line in file.readlines()]

output_bin:dict[str,list[str]] = {}
bot_army:dict[str,list[str,str]] = {}

def check_part1(bot) -> bool:
    if bot in bot_army:
        if "17" in bot_army[bot] and "61" in bot_army[bot]:
            print(f"Part 1: {bot}")

while raw_commands:
    for command in raw_commands:
        if command[0] == "value":
            chip = command[1]
            bot = command[-1]
            
            if bot in bot_army:
                bot_army[bot].append(chip)
                assert len(bot_army[bot]) <= 2
            else:
                bot_army[bot] = [chip]
            
            raw_commands.remove(command)
            check_part1(bot)
            break
        
        if command[0] == "bot":
            bot = command[1]
            if bot not in bot_army or len(bot_army[bot]) < 2: continue
            assert len(bot_army[bot]) == 2
            check_part1(bot)
            
            low, high = sorted(bot_army[bot])
            low_destination = output_bin if command[5] == "output" else bot_army
            low_index = command[6]
            high_destination = output_bin if command[10] == "output" else bot_army
            high_index = command[11]

            actions = ((low, low_destination, low_index), (high, high_destination, high_index))
            
            for chip, destination, index in actions:
                if index in destination:
                    destination[index].append(chip)
                else:
                    destination[index] = [chip]
            
            bot_army[bot].clear()
            raw_commands.remove(command)
            break

# 189 - too high