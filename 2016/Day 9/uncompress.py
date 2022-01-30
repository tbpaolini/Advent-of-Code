with open("input.txt", "rt") as file:
    raw_data = file.read().strip()

# Part 1

length_p1 = 0
pos = 0
file_size = len(raw_data)

while pos < file_size:
    char =  raw_data[pos]
    pos += 1
    
    if char == "(":
        char_count = ""
        while raw_data[pos].isnumeric():
            char_count += raw_data[pos]
            pos += 1
        char_count = int(char_count)

        assert raw_data[pos] == "x"
        pos += 1

        char_repeat = ""
        while raw_data[pos].isnumeric():
            char_repeat += raw_data[pos]
            pos += 1
        char_repeat = int(char_repeat)

        assert raw_data[pos] == ")"
        pos += 1

        pos += char_count
        length_p1 += char_count * char_repeat
        
    else:
        length_p1 += 1

print(f"Part 1: {length_p1}")

# Part 2

def get_size(raw_data):

    data_size = len(raw_data)
    pos = 0
    length_p2 = 0

    while pos < data_size:
        char =  raw_data[pos]
        pos += 1
        
        if char == "(":
            char_count = ""
            while raw_data[pos].isnumeric():
                char_count += raw_data[pos]
                pos += 1
            char_count = int(char_count)

            assert raw_data[pos] == "x"
            pos += 1

            char_repeat = ""
            while raw_data[pos].isnumeric():
                char_repeat += raw_data[pos]
                pos += 1
            char_repeat = int(char_repeat)

            assert raw_data[pos] == ")"
            pos += 1

            length_p2 += get_size(raw_data[pos : pos+char_count]) * char_repeat
            pos += char_count
            
        else:
            length_p2 += 1
    
    return length_p2

print(f"Part 2: {get_size(raw_data)}")