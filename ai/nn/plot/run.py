import os
import os.path
import sys

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
import matplotlib.gridspec as gridspec

RES = 64
COORDSV = np.linspace(-0.5, 1.5, num = RES)
INPUT_OUT = np.dstack(np.meshgrid(COORDSV, COORDSV)).reshape(RES, RES, 2, 1)


COLOR_NEG = (0.9, 0.54, 0.0)
COLOR_POS = (0.45, 0.27, 0.9)
COLORS = [COLOR_NEG, (1.0, 1.0, 1.0), COLOR_POS]
COLORMAP = LinearSegmentedColormap.from_list('ASDF', COLORS, 256)

X_PLOT = 2.5
Y_PLOT = 2.5
X_PLOT_GAP = 1
Y_PLOT_GAP = 0.1

X_TEXT = 2
X_COLORBAR = 3
X_BAR = X_TEXT + X_COLORBAR
Y_BAR = 1.5

GRADIENT = np.linspace(0, 1, 256)
GRADIENT = np.vstack((GRADIENT, GRADIENT))

def main():
    epochs = []
    errors = []
    dirname, _ = os.path.splitext(os.path.basename(sys.argv[1]))
    os.makedirs(dirname, exist_ok=True)
    markers = None
    if len(sys.argv) > 2:
        with open(sys.argv[2], 'r') as marker_f:
            markers = [[], [], []]
            for line in marker_f:
                x, y, out = map(float, line.strip().split(','))
                markers[0].append(x)
                markers[1].append(y)
                markers[2].append(COLOR_POS if out > 0.5 else COLOR_NEG)
    with open(sys.argv[1], 'r') as f:
        while True:
            epoch, net, error = parse_entry(f)
            if net is None:
                break
            epochs.append(epoch)
            errors.append(error)
            filename = os.path.join(dirname, "{}.png".format(epoch))
            plot_net(epoch, net, error, markers, filename)
    filename = os.path.join(dirname, "error.png")
    plot_error(epochs, errors, filename)
    

def parse_entry(f):
    net = []
    line = f.readline()
    if line == '':
        return 0, None, 0.0
    epoch = int(line[6:-1])
    line = f.readline()
    while line == '{\n':
        line = f.readline()
        multiplier = []
        bias = []
        while line != '}\n':
            if line[-2] == ',':
                line = line[:-1]
            perceptron = map(float, line[3:-2].split(', '))
            bias.append(next(perceptron))
            multiplier.append(list(perceptron))
            line = f.readline()
        net.append((np.array(multiplier), np.transpose([bias])))
        line = f.readline()
    error = float(line[:-1])
    f.readline()
    return epoch, net, error

def plot_net(epoch, net, error, markers, filename):
    subplots_x = len(net)
    subplots_y = max(layer[1].shape[0] for layer in net)
    out = INPUT_OUT

    fig_y = Y_BAR + max(subplots_y * (Y_PLOT + Y_PLOT_GAP), X_BAR)
    fig_x = max(X_BAR, subplots_x * (X_PLOT + X_PLOT_GAP))
    fig = plt.figure(figsize=(fig_x, fig_y), constrained_layout=True)
    gs = gridspec.GridSpec(2, 1, figure=fig, height_ratios=[Y_BAR, max(X_BAR, subplots_y * (Y_PLOT + Y_PLOT_GAP))])
    
    if X_BAR >= fig_x:
        gs_bar = gs[0].subgridspec(1, 2, width_ratios=[X_TEXT, X_COLORBAR])
    else:
        gs_bar = gs[0].subgridspec(1, 3, width_ratios=[X_TEXT, X_COLORBAR, fig_x - X_BAR])
    
    txt = fig.add_subplot(gs_bar[0])
    txt.set_axis_off()
    txt.text(0.1, 0.5, "Epoch {}".format(epoch), fontsize=24, transform=txt.transAxes)
    txt.text(0.1, 0.3, "Error = {}".format(error), fontsize=15, transform=txt.transAxes)
    
    colorbar = fig.add_subplot(gs_bar[1])
    colorbar.imshow(GRADIENT, aspect=0.05, cmap=COLORMAP, extent=(-1.0, 1.0, 0.0, 1.0))
    colorbar.set_yticks([])
    colorbar.set_xticks([-1.0, 0.0, 1.0])
    

    gs_plot = gs[1].subgridspec(subplots_y, subplots_x, wspace=X_PLOT_GAP/(X_PLOT_GAP+X_PLOT))
    for x, layer in enumerate(net):
        out = np.tanh(np.matmul(layer[0], out) + layer[1])
        normalized_out = (out + 1) / 2
        for y in range(layer[1].shape[0]):
            subplot = fig.add_subplot(gs_plot[y, x])
            if y == 0:
                subplot.set_title('Layer {}'.format(x + 1))
            subplot.imshow(
                normalized_out[:,:,y].reshape(RES, RES),
                cmap = COLORMAP,
                vmin = 0.0, vmax = 1.0,
                origin = 'lower',
                extent = (-0.5, 1.5, -0.5, 1.5)
            )
            subplot.set_xticks([-0.5, 0.0, 0.5, 1.0, 1.5])
            subplot.set_yticks([-0.5, 0.0, 0.5, 1.0, 1.5])
            if x == len(net) - 1 and markers is not None:
                subplot.scatter(markers[0], markers[1], c=markers[2], marker='o', edgecolor='black')
    
    fig.savefig(filename)
    plt.close(fig)

def plot_error(epochs, errors, filename):
    plt.plot(epochs, errors)
    plt.ylim([0,1])
    plt.xlabel('Epoch')
    plt.ylabel('Error')
    plt.savefig(filename)
    plt.close()

main()