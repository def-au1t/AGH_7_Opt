import os

iterations = 3
# iterations = 1

if __name__ == '__main__':
    # for comp in ['gcc', 'gcc-11']:
    for comp in ['gcc-11']:
        # for comp_option in ['-O0', '-O1', '-O2', '-O3']:
        for comp_option in ['-O0']:
            for i in range(0,8):
                for size in range(100, 2001, 100):
                    os.system(f"gcc-11 -o out {comp_option} lu{i+1}.c")
                    os.system(f"./out {i+1},{comp_option} {size} {iterations} >> results.txt")
