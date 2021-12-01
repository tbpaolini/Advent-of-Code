from urllib.request import Request, urlopen
from pathlib import Path
from time import sleep

# This script needs to be run from the directory where the years folders are

session = input("Session ID: ").strip()

for year in range(2015, 2021):
    for day in range(1, 26):
        my_path = Path(f"{year}\Day {day}\input.txt")
        if my_path.exists(): continue
        
        my_request = Request(
            url = f"https://adventofcode.com/{year}/day/{day}/input",
            headers = {"cookie": f"session={session}"},
            method = "GET",
        )
        page = urlopen(my_request)
        data = page.read()

        with open(my_path, "wb") as file:
            file.write(data)
            print(f"Downloaded day {day} of {year}")
        
        sleep(1)