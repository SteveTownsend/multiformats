import sys
import csv

with open(sys.argv[1]) as csvfile:
    reader = csv.reader(csvfile)
    print("std::map<std::string, Varint> table {")
    for row in reader:
        print("    { \"%s\", %s }," % (row[0], row[2].replace(' ', '')))

    print("}")
