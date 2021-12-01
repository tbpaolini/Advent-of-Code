with open("input.txt", "rt") as file:
    text_input = file.readlines()

# Puzzle 1

sonar_data = [int(line.strip()) for line in text_input]
data_size = len(sonar_data)

count = 0
for i in range(1, data_size):
    if sonar_data[i] > sonar_data[i-1]:
        count += 1

print(count)

# Puzzle 2

sliding_window = []

for i in range(2, data_size):
    value = sum(sonar_data[i-2 : i+1])
    sliding_window.append(value)

sw_size = len(sliding_window)

count = 0
for i in range(1, sw_size):
    if sliding_window[i] > sliding_window[i-1]:
        count += 1
 
print(count)