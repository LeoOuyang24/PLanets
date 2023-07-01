import cv2 as cv
import numpy as np
import sys
import functools as tools
import random


grass = [0,255,0,255]

def isGrass(img, i, j):
    return (i < len(img)) and (i >= 0) and (j < len(img[0])) and (j >= 0) and (list(img[i][j]) == grass)


def main(filename,newName):
    img = cv.imread(filename, cv.IMREAD_UNCHANGED)
    height = int(len(img))
    width = int(len(img[0]))
    copy = []
    for i in range(height):
        for j in range(width):
            if (img[i][j][3] == 255):
                grassNearby = 0;
                for h in range(4):
                    grassNearby += isGrass(img,i - h%3, j - h//3)
                if (random.randint(0,4 - grassNearby) == 0):
                    img[i][j] = grass
            #cv.imwrite(direct + "/" + name + str(i*size + j) + '.png',img[i*width:i*width+width,j*height:(j+1)*height]);
            #cv.imshow("ASD",temp);
            #cv.waitKey(0);
    cv.imwrite(newName,img)
if __name__ == "__main__":
    if (len(sys.argv) < 2):
        print("usage: python grass.py <image path> <new file name optional>")
    else:
        if (len(sys.argv) < 3): #if they didn't provide a new file name, use the old name
            sys.argv.append(sys.argv[1])
        main(sys.argv[1],sys.argv[2])   