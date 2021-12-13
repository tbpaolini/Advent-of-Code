from urllib.request import Request, urlopen
from pathlib import Path
from time import sleep
import re
from datetime import date

# This script needs to be run from the directory where the years folders are

session = input("Session ID: ").strip()
article_regex = re.compile(r"(?ms)\<article.*?\>(.*)</article.*?\>")
today = date.today()

for year in range(2015, 2022):
    for day in range(1, 26):
        if date(year, 12, day) > today: break
        my_path = Path(f"{year}\Day {day}\puzzle-{year}-{day}.html")
        if my_path.exists(): continue
        
        my_request = Request(
            url = f"https://adventofcode.com/{year}/day/{day}",
            headers = {"cookie": f"session={session}"},
            method = "GET",
        )
        page = urlopen(my_request)
        data = page.read().decode(encoding="utf-8")
        puzzle = article_regex.search(data).group(1)

        with open(my_path, "wt") as file:
            file.write(puzzle)
            print(f"Downloaded day {day} of {year}")
        
        sleep(1)