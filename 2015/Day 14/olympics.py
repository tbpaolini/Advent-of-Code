from collections import namedtuple, Counter

reindeers = []
Reindeer = namedtuple("Reindeer", "name speed fly_time rest_time")
with open("input.txt", "rt") as file:
    for line in file:
        command = line.rstrip(".").split()
        name = command[0]
        speed = int(command[3])
        fly_time = int(command[6])
        rest_time = int(command[-2])
        reindeers.append(Reindeer(name, speed, fly_time, rest_time))

time_limit = 2503

# Part 1

def calculate_distances(time):
    global reindeers
    distances = Counter()

    for reindeer in reindeers:
        cur_time = 0
        my_distance = 0
        flying = True
        
        while cur_time < time:
            if flying:
                my_distance += reindeer.speed * min(reindeer.fly_time, time - cur_time)
                cur_time += reindeer.fly_time
                flying = False
            else:
                cur_time += reindeer.rest_time
                flying = True
        
        distances.update({reindeer.name: my_distance})
    
    return distances

dist = calculate_distances(time_limit)
(name, top_distance), = dist.most_common(1)
print(f"{name}: {top_distance}")

# Part 2

def calculate_scores(time):
    global reindeers
    scoreboard = {
        reindeer.name: dict(
            score = 0,
            distance = 0,
            fly = reindeer.fly_time,
            rest = 0,
            max_fly = reindeer.fly_time,
            max_rest = reindeer.rest_time,
            speed = reindeer.speed
        )
        for reindeer in reindeers
    }

    for cur_time in range(time):
        
        for name, stat in scoreboard.items():
            
            if stat["fly"] > 0:
                stat["distance"] += stat["speed"]
                stat["fly"] -= 1
                if stat["fly"] == 0:
                    stat["rest"] = stat["max_rest"]
            
            elif stat["rest"] > 0:
                stat["rest"] -= 1
                if stat["rest"] == 0:
                    stat["fly"] = stat["max_fly"]
            
        standings = sorted(
            scoreboard.items(),
            key = lambda f: f[1]["distance"],
            reverse = True,
        )

        for count, (name, stat) in enumerate(standings):
            top_distance = stat["distance"]
            if (count > 0) and (top_distance != previous_distance):
                break
            scoreboard[name]["score"] += 1
            previous_distance = top_distance
    
    return scoreboard

scores = calculate_scores(time_limit)
final_scores = sorted(
    scores.items(),
    key = lambda f: f[1]["score"],
    reverse = True,
)
name, points = final_scores[0]
print(f"{name}: {points['score']}")