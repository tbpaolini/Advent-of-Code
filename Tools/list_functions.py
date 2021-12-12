def is_sublist(list1:list, list2:list) -> bool:
    size = len(list1)
    for i in range(len(list2) - len(list1) + 1):
        if list2[i : size+i] == list1: return True
    else:
        return False

def split_list(my_list:list, value) -> list:
    result = []
    start = 0
    for end, elm in enumerate(my_list):
        if elm == value:
            my_slice = my_list[start:end]
            if my_slice: result += [my_slice]
            start = end+1
    
    remainder = my_list[start:]
    if remainder: result += [my_list[start:]]

    return result