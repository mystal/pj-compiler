import sys
import re

def main():
    lines = sys.stdin.readlines()
    acts = []
    nums = []
    #print "Read", len(lines), "lines!"
    curact = []
    acts.append(curact)
    curnum = []
    nums.append(curnum)
    for i in range(len(lines)):
        m = re.search(r"\.act.*;", lines[i])
        if m != None:
            m = re.search(r"[0-9]+(?=;)", lines[i])
            if len(curact) == 29:
                curact = []
                acts.append(curact)
            curact.append(int(m.group())-1)
            continue
        m = re.search(r"\.num.*;", lines[i])
        if m != None:
            m = re.search(r"[0-9]+(?=;)", lines[i])
            if len(curnum) == 29:
                curnum = []
                nums.append(curnum)
            curnum.append(int(m.group())-1)
            continue
    sys.stderr.write("acts: %dx%d\n" % (len(acts), len(acts[0])))
    sys.stderr.write("nums: %dx%d\n" % (len(nums), len(nums[0])))
    for i in range(54):
        sys.stdout.write("{")
        for j in range(28):
            sys.stdout.write("{%d,%d}," % (acts[i][j], nums[i][j]))
        sys.stdout.write("{%d,%d}" % (acts[i][28], nums[i][28]))
        sys.stdout.write("},\n")

if __name__ == "__main__":
    main()
