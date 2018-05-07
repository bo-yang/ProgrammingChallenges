#!/usr/bin/env python

import os
import sys
import math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec

from getopt import getopt
from getopt import GetoptError

# Global variables
d = 1.0 # inner dimension of the cone
D = 4.5 # outer dimension of the cone
C = 241 # columns
L = 1024 # record points per probe
header = 1240
theta = math.pi/3
totalFrames = 32
videoDims = (512,512) # video dimensions

def read_file(fname, header=1240):
    """ Read raw probe points and formulate them into Numpy array
    """
    if not os.path.isfile(fname):
        print('Error: cannot find file %s' %fname)
        sys.exit(1)
    with open(fname, 'rb') as bfile:
        data = bfile.read() # read the whole file
    data = data[header:] # drop the header
    # convert binary data to numbers
    data = np.fromstring(data, dtype=np.uint8)
    # reshape data to frames
    data = data.reshape((totalFrames, C, L))
    return data

def raw_frame_size(L, d, D, theta):
    """ Calculate the raw size of each frame.
        L - points per probe
        d - inner dimension of the cone
        D - outer dimension of the cone
        theta - angle of the cone, in radians
    """
    h = L*D / (D-d)
    w = 2 * D * math.sin(theta/2) * L / (D-d)
    return (math.ceil(w), math.ceil(h))

def draw_frame(frame):
    """ Draw one frame
        frame - CxL matrix
    """
    (W,H) = raw_frame_size(L, d, D, theta) # image size(width, height) of each frame
    image = np.zeros((W, H))
    basePoints = math.ceil(L*d / (D-d)) # 293 - L=1024, d=1.0, D=4.5
    for c in range(C-1, -1, -1):
        rad = (math.pi-theta)/2 + c*theta/(C-1)
        for l in range(0, L):
            w = W/2 + (basePoints+l) * math.cos(rad) # width coordinate
            h = (basePoints+l) * math.sin(rad) - (basePoints/2) # height coordinate
            image[math.floor(h), math.floor(w)] = frame[C-1-c, l] # paint pixel
    return image

def save_frame(data, fname, sizes=tuple(), cm='gray', interp='bicubic'):
    """ Save frame into an image.
        data - image data
        fname - image name
        sizes - tuple of image dimensions
        cm - cmap
        interp - interpolation method
    """
    if (len(sizes) <= 0):
        sizes = np.shape(data)
    height = float(sizes[0])
    width = float(sizes[1])
     
    fig = plt.figure()
    fig.set_size_inches(width/height, 1, forward=False)
    ax = plt.Axes(fig, [0., 0., 1., 1.])
    ax.set_axis_off()
    fig.add_axes(ax)

    ax.imshow(data, cmap=cm, interpolation=interp)
    plt.savefig(fname, dpi = height) 
    plt.close()

def show_frames():
    """ Show all frames in one figure
    """
    frames = read_file()
    plt.close('all')
    nrows = 8
    ncols = int(totalFrames / nrows)
    fig,axes=plt.subplots(nrows, ncols, sharex=True, sharey=True)
    gs = gridspec.GridSpec(nrows, ncols)
    gs.update(wspace=0.05, hspace=0.05) # set the spacing between axes. 
    for n in range(0, totalFrames):
        ax = plt.subplot(gs[n])
        plt.axis('off')
        ax.set_xticklabels([])
        ax.set_yticklabels([])
        image = draw_frame(frames[n])
        plt.imshow(image, cmap='gray', interpolation = 'bicubic')
        ax.set_title('%d' %(n+1))
    plt.subplots_adjust(wspace=0, hspace=0)
    plt.tight_layout()
    fig.show()

def gen_video(ifile='carotid.cine', vname='carotid_video', imageDir='./frames'):
    """ Generate video based on raw probe data.
    """
    frames = read_file(fname=ifile, header=header)
    # draw frames
    if not os.path.isdir(imageDir):
        os.mkdir(imageDir)
    for n in range(0, totalFrames):
        image = draw_frame(frames[n])
        if (image.max() > 0): # enhance the contrast
            image = image/image.max()*255
        imageName = "%s/%04d.png" %(imageDir,n+1)
        save_frame(image, imageName, sizes=videoDims)
    # create video
    cmd = "ffmpeg -f image2 -r 10 -i %s/%%04d.png -vcodec mpeg4 -y %s.mp4" %(imageDir, vname)
    os.system(cmd)

def usage(msg=''):
    if len(msg) > 0:
        print('Error: %s\n' %msg)
    print('Usage: %s [-i input] [-o video_name] [-w image_dir]' %sys.argv[0])

if __name__ == '__main__':
    try:
        opts, args = getopt(sys.argv[1:], "hi:o:w:",["input=","output=","workspace="])
    except GetoptError as e:
        raise usage(e.msg)

    ifile = 'sample.data'
    ofile = 'sample_video'
    workspace = '/tmp/frames'
    for option, value in opts:
        if option in ("-i", "--input"):
            ifile = value
        elif option in ("-o", "--output"):
            ofile = value
        elif option in ("-w", "--workspace"):
            workspace = value
        elif option == "-h":
            usage()
            sys.exit(0)
        else:
            usage("Unsupported argument %s!" % option)
            sys.exit(1)

    gen_video(ifile=ifile, vname=ofile, imageDir=workspace)
