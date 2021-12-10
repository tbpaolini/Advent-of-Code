target_row = 2947
target_column = 3029

def next_code(prev):
    return (prev * 252533) % 33554393

def sum_integer_range(start:int, end:int) -> int:
    return (start + end) * (end - start + 1) // 2

def get_code(row, column):
    row_value = sum_integer_range(1, column)
    column_value = row_value + sum_integer_range(column, column + (row - 2))
    result = 20151125
    for _ in range(column_value-1):
        result = next_code(result)
    return result

code = get_code(target_row, target_column)
print(f"Code: {code}")