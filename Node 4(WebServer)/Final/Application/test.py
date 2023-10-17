import csv

rows = []

with open("login.csv", 'r') as file:
    csvreader = file.readlines()

header = csvreader[:1]
rows = csvreader[1:]

header = header[0].split(",")[1]
rows = rows[0].split(",")[1]

print(header)
print(rows)