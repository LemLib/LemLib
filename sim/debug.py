with open("pose", "r", encoding= "utf-16") as f:
    line = f.readline()
print(line)   # Should print: -39.17, -19.28, -7.52\n
print(line.split(','))  # ['-39.17', ' -19.28', ' -7.52\n']
