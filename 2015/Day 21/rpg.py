from collections import namedtuple
from itertools import combinations, product
from math import ceil

Stats = namedtuple("Stats", "hp dmg arm")
Item = namedtuple("Item", "gold dmg arm")
boss = Stats(hp=103, dmg=9, arm=2)

weapons = dict(
    Dagger = Item(gold=8, dmg=4, arm=0),
    Shortsword = Item(gold=10, dmg=5, arm=0),
    Warhammer = Item(gold=25, dmg=6, arm=0),
    Longsword = Item(gold=40, dmg=7, arm=0),
    Greataxe = Item(gold=74, dmg=8, arm=0)
)

armors = dict(
    nothing = Item(gold=0, dmg=0, arm=0),
    Leather = Item(gold=13, dmg=0, arm=1),
    Chainmail = Item(gold=31, dmg=0, arm=2),
    Splintmail = Item(gold=53, dmg=0, arm=3),
    Bandedmail = Item(gold=75, dmg=0, arm=4),
    Platemail = Item(gold=102, dmg=0, arm=5),
)

rings = dict(
    nothing = Item(gold=0, dmg=0, arm=0),
    Damage_p1 = Item(gold=25, dmg=1, arm=0),
    Damage_p2 = Item(gold=50, dmg=2, arm=0),
    Damage_p3 = Item(gold=100, dmg=3, arm=0),
    Defense_p1 = Item(gold=20, dmg=0, arm=1),
    Defense_p2 = Item(gold=40, dmg=0, arm=2),
    Defense_p3 = Item(gold=80, dmg=0, arm=3),
)

rings_combos = list(combinations(rings, 2)) + [("nothing", "nothing")]
equips_combos = tuple(product(weapons, armors, rings_combos))

min_cost = 1000000
max_cost = 0
for weapon, armor, (ring1, ring2) in equips_combos:
    cost = 0
    damage = 0
    defense = 0
    equips = (weapons[weapon], armors[armor], rings[ring1], rings[ring2])
    for item in equips:
        cost += item.gold
        damage += item.dmg
        defense += item.arm
    player = Stats(hp=100, dmg=damage, arm=defense)
    
    player_hit = max(player.dmg - boss.arm, 1)
    boss_hit = max(boss.dmg - player.arm, 1)

    if ceil(boss.hp / player_hit) <= ceil(player.hp / boss_hit):
        min_cost = min(min_cost, cost)
    else:
        max_cost = max(max_cost, cost)

    # player_life = player.hp
    # boss_life = boss.hp
    # while True:
    #     boss_life -= player_hit
    #     if boss_life <= 0: break
    #     player_life -= boss_hit
    #     if player_life <= 0: break
    
    # if player_life > 0:
    #     min_cost = min(min_cost, cost)

print(f"Part 1: {min_cost}")
print(f"Part 2: {max_cost}")