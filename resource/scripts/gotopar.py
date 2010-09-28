import sys
import re

def main():
    lines = sys.stdin.readlines()
    gotox = []
    curgoto = []
    gotox.append(curgoto)
    for i in range(len(lines)):
        m = re.search(r"gotox.*;", lines[i])
        if m != None:
            m = re.search(r"-?[0-9]+(?=;)", lines[i])
            if len(curgoto) == 12:
                curgoto = []
                gotox.append(curgoto)
                #sys.stdout.write("\n")
            #sys.stdout.write("%s " % (m.group()));
            state = int(m.group())
            if state != -1:
                state = state - 1
            #sys.stdout.write("(%d), " % state);
            curgoto.append(state)
    sys.stderr.write("gotox: %dx%d\n" % (len(gotox), len(gotox[0])))
    for i in range(54):
        sys.stdout.write("{")
        for j in range(11):
            sys.stdout.write("%d, " % (gotox[i][j]))
        sys.stdout.write("%d" % (gotox[i][11]))
        sys.stdout.write("},\n")

if __name__ == "__main__":
    main()
