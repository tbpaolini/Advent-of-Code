from random import shuffle

boss_max_hp = 58
boss_damage = 9

player_max_hp = 50
player_max_mp = 500
player_armor = 0

def magic_missile():
    global player_mp, boss_hp, mp_spent
    if player_mp < 53: return False
    player_mp -= 53
    mp_spent += 53
    boss_hp -= 4
    return True

def drain():
    global player_mp, player_hp, boss_hp, mp_spent
    if player_mp < 73: return False
    player_mp -= 73
    mp_spent += 73
    player_hp += 2
    boss_hp -= 2
    return True

def shield():
    global timer, player_mp, mp_spent

    if player_mp < 113: return False
    if timer["shield"] > 0: return False
    player_mp -= 113
    mp_spent += 113
    timer["shield"] = 6
    return True

def poison():
    global timer, player_mp, mp_spent

    if player_mp < 173: return False
    if timer["poison"] > 0: return False
    player_mp -= 173
    mp_spent += 173
    timer["poison"] = 6
    return True

def recharge():
    global timer, player_mp, mp_spent

    if player_mp < 229: return False
    if timer["recharge"] > 0: return False
    player_mp -= 229
    mp_spent += 229
    timer["recharge"] = 5
    return True

def tick_timers():
    global timer, player_armor, boss_hp, player_mp
    
    if timer["shield"] > 0:
        player_armor = 7
        timer["shield"] -= 1
    else:
        player_armor = 0
    
    if timer["recharge"] > 0:
        player_mp += 101
        timer["recharge"] -= 1

    if timer["poison"] > 0:
        boss_hp -= 3
        timer["poison"] -= 1

wins = []
losses = []
spells = [magic_missile, drain, shield, poison, recharge]

while True:
    try:
        mode = int(input("Mode (1) normal - (2) hard: "))
        if mode not in (1, 2): continue
        break
    except ValueError:
        continue

print("Simulating 50000 games...")
for i in range(50000):
    boss_hp = boss_max_hp
    player_hp = player_max_hp
    player_mp = player_max_mp
    mp_spent = 0

    timer = {
        "shield": 0,
        "poison": 0,
        "recharge": 0,
    }

    while True:
        shuffle(spells)

        # Player turn
        if mode == 2:
            player_hp -= 1
            if player_hp <= 0:
                losses += [mp_spent]
                break
        
        tick_timers()

        if boss_hp <= 0:
            wins += [mp_spent]
            break

        cast_spell = False
        for cast in spells:
            cast_spell = cast()
            if cast_spell: break
        
        if boss_hp <= 0:
            wins += [mp_spent]
            break

        if not cast_spell:
            losses += [mp_spent]
            break
        
        # Boss turn
        tick_timers()
        if boss_hp <= 0:
            wins += [mp_spent]
            break

        player_hp -= max(boss_damage - player_armor, 1)
        
        if player_hp <= 0:
            losses += [mp_spent]
            break

print(f"Part {mode}: {min(wins)}")

# 903  - too low
# 2398 - too high