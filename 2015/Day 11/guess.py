from string import ascii_lowercase

my_input = "hepxcrrq"

def alphabet_increment(text):
    
    size = len(text)
    pos = -1
    trail = ""
    
    while True:
        char = text[pos]
        
        if char != "z":
            index = ascii_lowercase.index(char) + 1
            new_char = ascii_lowercase[index]
            return text[0 : size+pos] + new_char + trail
        else:
            pos -= 1
            trail += "a"
        
        if pos < -size:
            return "a" + trail

# Part 1
exclude = ("i", "o", "l")
include1 = tuple(ascii_lowercase[i-2 : i+1] for i in range(2, 26))
include2 = tuple(ascii_lowercase[i]*2 for i in range(26))

next_pass = my_input
found_passes = 0

while found_passes < 2:

    next_pass = alphabet_increment(next_pass)

    found0 = False
    for char in exclude:
        if char in next_pass:
            found0 = True
            break
    
    if found0: continue
    
    found1 = False
    for seq in include1:
        if seq in next_pass:
            found1 = True
            break
    
    if not found1: continue
    
    found2 = 0
    for seq in include2:
        if seq in next_pass:
            found2 += 1
            if found2 == 2:
                print(next_pass)
                found_passes += 1
                break