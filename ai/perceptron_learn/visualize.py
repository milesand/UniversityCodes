import csv
import sys
import os
import os.path

import matplotlib.pyplot as plt
import numpy as np

# For early return
def main():
    if len(sys.argv) != 3:
        print("Usage: {} [TRAINING_DATA] [LOG_FILE]".format(sys.argv[0]))
        return

    tfilename = sys.argv[1]
    lfilename = sys.argv[2]

    tdata = (([], []), ([], []))
    try:
        tfile = open(tfilename, newline='')
    except OSError:
        print("{}: Error: File {} not found".format(sys.argv[0], tfilename, file=sys.stderr))
        return
    rows = csv.reader(tfile)
    for row in rows:
        x, y, o = map(float, row)
        dst = 1 if o > 0.5 else 0
        tdata[dst][0].append(x)
        tdata[dst][1].append(y)
    tfile.close()

    try:
        f = open(lfilename, newline='')
    except OSError:
        print("{}: Error: File {} not found".format(sys.argv[0], lfilename), file=sys.stderr)
        return

    dirname, _ = os.path.splitext(lfilename)
    errors = []
    try:
        os.mkdir(dirname)
    except FileExistsError:
        pass
    
    rows = csv.reader(f)
    for (i, row) in enumerate(rows):
        plt.axhline(0, color='gray', linewidth=0.5)
        plt.axvline(0, color='gray', linewidth=0.5)
        plt.plot(tdata[0][0], tdata[0][1], 'ro')
        plt.plot(tdata[1][0], tdata[1][1], 'go')

        t, w0, w1, e = map(float, row)
        errors.append(e)
        # w0 * x + w1 * y >= t

        if w1 != 0.0:
            x = np.array([-0.5, 1.5])
            y = (t - (w0 * x)) / w1

            plt.plot(x, y, color='green')
            if w1 > 0.0:
                plt.fill_between(x, y, 1.5, facecolor='lightgreen')
            else:
                plt.fill_between(x, y, -0.5, facecolor='lightgreen')
        elif w0 != 0.0:
            x = t / w0
            plt.plot((x, x), (-0.5, 1.5), color='green')
            if w0 > 0.0:
                plt.fill_between((x, 1.5), 1.5, -0.5, color='lightgreen')
            else:
                plt.fill_between((-0.5, x), 1.5, -0.5, color='lightgreen')
        elif t <= 0:
            plt.fill_between((-0.5, 1.5), 1.5, -0.5, color='lightgreen')
        
        axes = plt.gca()
        axes.set_xlim([-0.5, 1.5])
        axes.set_ylim([-0.5, 1.5])

        plt.subplots_adjust(left=0.4)
        plt.text(
            0.08, 0.5, " t = {: .3f}\nw0 = {: .3f}\nw1 = {: .3f}".format(t, w0, w1),
            transform = plt.gcf().transFigure,
            fontfamily = 'monospace',
            fontsize = 'large',
            linespacing = 1.5,
            va = 'center'
        )
        
        plt.savefig('{}/{}.png'.format(dirname, i))
        plt.close()
    f.close()

    plt.plot(errors)
    plt.xlabel('Iterations')
    plt.ylabel('Error')
    plt.savefig('{}/errors.png'.format(dirname))
    plt.close()

main()
