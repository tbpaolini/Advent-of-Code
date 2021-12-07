import random

# I have borrowed this solution from here:
# https://old.reddit.com/r/adventofcode/comments/3xflz8/day_19_solutions/cy4wbst/

start_molecule = "ORnPBPMgArCaCaCaSiThCaCaSiThCaCaPBSiRnFArRnFArCaCaSiThCaCaSiThCaCaCaCaCaCaSiRnFYFArSiRnMgArCaSiRnPTiTiBFYPBFArSiRnCaSiRnTiRnFArSiAlArPTiBPTiRnCaSiAlArCaPTiTiBPMgYFArPTiRnFArSiRnCaCaFArRnCaFArCaSiRnSiRnMgArFYCaSiRnMgArCaCaSiThPRnFArPBCaSiRnMgArCaCaSiThCaSiRnTiMgArFArSiThSiThCaCaSiRnMgArCaCaSiRnFArTiBPTiRnCaSiAlArCaPTiRnFArPBPBCaCaSiThCaPBSiThPRnFArSiThCaSiThCaSiThCaPTiBSiRnFYFArCaCaPRnFArPBCaCaPBSiRnTiRnFArCaPRnFArSiRnCaCaCaSiThCaRnCaFArYCaSiRnFArBCaCaCaSiThFArPBFArCaSiRnFArRnCaCaCaFArSiRnFArTiRnPMgArF"

def day19_part2():
    rules = []
    for line in open("input.txt"):
        try:
            h, r = line.split(' => ')
        except ValueError:
            pass
        rules.append((h, r.strip()))

    steps = 0
    mol = start_molecule[:]
    r = 0
    while r < len(rules):
        head, rep = rules[r]
        i = mol.find(rep)
        if i != -1:
            steps += 1
            mol = mol[:i] + head + mol[i+len(rep):]
            # print(len(mol))
            r = 0
            random.shuffle(rules)
            if mol == 'e':
                print(steps)
                return True
        else:
            r += 1

while not day19_part2():
    pass