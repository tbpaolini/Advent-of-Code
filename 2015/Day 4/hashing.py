from hashlib import md5

key = "ckczppom"

# Part 1

value = 0
while True:
    code = f"{key}{value}".encode(encoding="ascii")
    my_hash = md5(code).hexdigest()
    if my_hash.startswith("00000"):
        break
    value += 1

print(f"Found: {value}")

# Part 2

value = 0
while True:
    code = f"{key}{value}".encode(encoding="ascii")
    my_hash = md5(code).hexdigest()
    if my_hash.startswith("000000"):
        break
    value += 1

print(f"Found: {value}")