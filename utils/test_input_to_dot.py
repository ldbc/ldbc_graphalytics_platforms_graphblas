import sys

file = sys.argv[1]

print("digraph g {")

with open(file, mode="r") as file:
    for line in file.readlines():
        components = line.split(' ')
        source = components[0]
        for target in components[1:]:
            print("\t{} -> {};".format(source, target.strip()))

print("}")
