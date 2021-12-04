from collections import deque
import numpy as np

# Read the file
with open("input.txt", "rt") as file:
    numbers = [np.uint8(number) for number in file.readline().strip().split(",")]
    raw_sheets = file.readlines()

# Parse the bingo sheets
bingo_sheets = []
for i in range(0, len(raw_sheets), 6):
    temp_sheet = []
    
    for j in range(i+1, i+6):
        temp_sheet.append(
            [np.uint8(number) for number in raw_sheets[j].split()]
        )
    
    bingo_sheets.append(
        np.array(temp_sheet, dtype="uint8")
    )

# Boolean arrays to keep tack of the marked values
bingo_check = [
    np.zeros(shape=(5,5), dtype="bool")
    for _ in range(len(bingo_sheets))
]

# Associate the marks with their respective bingo sheets
bingo_game = {
    ID: (sheet, check)
    for ID, (sheet, check) in enumerate(zip(bingo_sheets, bingo_check))
}

# Simulate the bingo game
bingo = deque()
scores = []

# Draw a number
for number in numbers:
    for ID, (sheet, check) in bingo_game.items():
        
        # Check if the number is in the sheet
        i, j = np.where(sheet == number)
        if i.size > 0:
            check[i, j] = True
            
            # Check if a row or column has been completed
            if (np.sum(check[i, :]) == 5) or (np.sum(check[:, j]) == 5):
                unmarked = np.invert(check)
                bingo.append((ID, sheet, unmarked, number))
    
    while bingo:
        # Calculate the score
        ID, sheet, unmarked, number = bingo.popleft()
        my_score = np.sum(sheet, where=unmarked) * number
        scores.append(my_score)
        del bingo_game[ID]
    
    # End the game
    if len(bingo_game) == 0:
        break

# Part 1
print("Part 1:", scores[0])

# Part 2
print("Part 2:", scores[-1])