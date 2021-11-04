import os

files = ["MMult0", "MMult1", "MMult2"]
for i in range(3,10):
    files.append("MMult_1x4_"+str(i))
for i in range(3,16):
    files.append("MMult_4x4_"+str(i))

for file in files:
    os.system(f"make run NEW={file}")
    print(f"{file}: done")