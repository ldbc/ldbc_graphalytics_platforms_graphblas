import sys

file = sys.argv[1]

tuples = 0

with open(file, mode="r") as file:
    for line in file.readlines():
        components = line.split(' ')
        tuples += len(components) - 1

print("Tuple count: {}".format(tuples))
