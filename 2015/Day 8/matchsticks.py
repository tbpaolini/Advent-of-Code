my_list = []
code_count = 0
with open("input.txt", "rt") as file:
    for line in file:
        code_count += len(line.strip())
        my_list.append(eval(line))

char_count = 0
for entry in my_list:
    char_count += len(entry)

# Part 1
print(code_count - char_count)

# Part 2
with open("input.txt", "rt") as file:
    raw_list = file.read()

escape_count = raw_list.count("\"") + raw_list.count("\\") + 2*len(my_list)
print(escape_count)