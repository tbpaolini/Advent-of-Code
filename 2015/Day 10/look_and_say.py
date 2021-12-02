my_input = "1321131112"

result = my_input
for i in range(50):
    
    new_input = ""
    cur_char = result[0]
    char_count = 1

    if i == 40: print(len(result))  # Part 1

    for j in range(1, len(result)):
        
        if result[j] == cur_char:
            char_count += 1
        else:
            new_input += str(char_count) + cur_char
            char_count = 1
            cur_char = result[j]
    
    new_input += str(char_count) + cur_char
    result = new_input

print(len(result))  # Part 2