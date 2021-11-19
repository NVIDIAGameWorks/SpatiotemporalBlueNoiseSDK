# Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
#
# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto.  Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.


import sys
import imageio
import numpy as np
from PIL import Image, ImageOps, ImageDraw
import os
import os.path
import pandas as pd
import matplotlib.pyplot as plt
from skimage import exposure

if len(sys.argv) != 3:
    print('Usage: python analyze.py baseFileName count')
    sys.exit()

baseFileName = sys.argv[1]
fileCount = int(sys.argv[2])

# load the images
imagesR=[]
imagesG=[]
imagesB=[]
for index in range(fileCount):
    fileName = baseFileName + "_" + str(index) + ".png"
    image = imageio.imread(fileName)

    if len(image.shape) == 2:
        imagesR.append(image.copy())
        imagesG.append(image.copy())
        imagesB.append(image.copy())
    else:
        imagesR.append(image[:,:,0])
        imagesG.append(image[:,:,1])
        imagesB.append(image[:,:,2])

    # the below is if you want to save them as their actual colors
    #imageR = image.copy()
    #imageR[:,:,1] = 0;
    #imageR[:,:,2] = 0;

    #imageio.imwrite(baseFileName + "_" + str(index) + ".R.png", image[:,:,0])
    #imageio.imwrite(baseFileName + "_" + str(index) + ".G.png", image[:,:,1])
    #imageio.imwrite(baseFileName + "_" + str(index) + ".B.png", image[:,:,2])

# Make XY DFTs
tempfilenames=[]
for index in range(fileCount):
    image=imagesR[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".R.mag.xy.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".R.mag.xy.png")

    image=imagesG[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".G.mag.xy.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".G.mag.xy.png")

    image=imagesB[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".B.mag.xy.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".B.mag.xy.png")

# Average and single XY DFTs
avgDFTR = []
avgDFTG = []
avgDFTB = []
singleDFTR = []
singleDFTG = []
singleDFTB = []
for index in range(fileCount):
    im = np.array(Image.open(baseFileName + "_" + str(index) + ".R.mag.xy.png"),dtype=np.float)
    if len(avgDFTR) == 0:
        avgDFTR = im / float(fileCount)
        singleDFTR = im
    else:
        avgDFTR = avgDFTR + im / float(fileCount)

    im = np.array(Image.open(baseFileName + "_" + str(index) + ".G.mag.xy.png"),dtype=np.float)
    if len(avgDFTG) == 0:
        avgDFTG = im / float(fileCount)
        singleDFTG = im
    else:
        avgDFTG = avgDFTG + im / float(fileCount)

    im = np.array(Image.open(baseFileName + "_" + str(index) + ".B.mag.xy.png"),dtype=np.float)
    if len(avgDFTB) == 0:
        avgDFTB = im / float(fileCount)
        singleDFTB = im
    else:
        avgDFTB = avgDFTB + im / float(fileCount)

# combine vector per axis DFTs into a single DFT
combinedDFT3 = np.sqrt(avgDFTR*avgDFTR + avgDFTG*avgDFTG + avgDFTB*avgDFTB)
combinedDFT2 = np.clip(exposure.equalize_hist(combinedDFT3, 256) * 256, 0, 255) # <- does a histogram equalization on the DFT to better visualize differences
combinedDFT = np.array(np.round(combinedDFT2),dtype=np.uint8)
Image.fromarray(combinedDFT,mode="L").save(baseFileName + ".combinedmag.equalized.xy.png")
combinedDFT4 = np.array(np.round(combinedDFT3),dtype=np.uint8)
Image.fromarray(combinedDFT4,mode="L").save(baseFileName + ".combinedmag.xy.png")
        
avgDFTR=np.array(np.round(avgDFTR),dtype=np.uint8)
avgDFTG=np.array(np.round(avgDFTG),dtype=np.uint8)
avgDFTB=np.array(np.round(avgDFTB),dtype=np.uint8)

#Image.fromarray(avgDFTR,mode="L").save(baseFileName + ".R.mag.xy.png")
#Image.fromarray(avgDFTG,mode="L").save(baseFileName + ".G.mag.xy.png")
#Image.fromarray(avgDFTB,mode="L").save(baseFileName + ".B.mag.xy.png")

avgDFT = np.empty([avgDFTR.shape[0], avgDFTR.shape[1], 3], dtype=np.uint8)
avgDFT[:,:,0] = avgDFTR
avgDFT[:,:,1] = avgDFTG
avgDFT[:,:,2] = avgDFTB
Image.fromarray(avgDFT,mode="RGB").save(baseFileName + ".mag.xy.png")

avgDFT = np.empty([avgDFTR.shape[0], avgDFTR.shape[1], 3], dtype=np.uint8)
avgDFT[:,:,0] = np.clip(exposure.equalize_hist(avgDFTR, 256) * 256, 0, 255)
avgDFT[:,:,1] = np.clip(exposure.equalize_hist(avgDFTG, 256) * 256, 0, 255)
avgDFT[:,:,2] = np.clip(exposure.equalize_hist(avgDFTB, 256) * 256, 0, 255)
Image.fromarray(avgDFT,mode="RGB").save(baseFileName + ".mag.equalized.xy.png")

singleDFTR=np.array(np.round(singleDFTR),dtype=np.uint8)
singleDFTG=np.array(np.round(singleDFTG),dtype=np.uint8)
singleDFTB=np.array(np.round(singleDFTB),dtype=np.uint8)

singleDFT = np.empty([singleDFTR.shape[0], singleDFTR.shape[1], 3], dtype=np.uint8)
singleDFT[:,:,0] = singleDFTR
singleDFT[:,:,1] = singleDFTG
singleDFT[:,:,2] = singleDFTB
#Image.fromarray(singleDFT,mode="RGB").save(baseFileName + ".mag0.xy.png")

# Make XZ DFTs
dftfilenames=[]
imagesR = np.rot90(imagesR, axes=(0,1))
imagesG = np.rot90(imagesG, axes=(0,1))
imagesB = np.rot90(imagesB, axes=(0,1))

for index in range(imagesR.shape[2]):
    image=imagesR[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".R.mag.xz.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".R.mag.xz.png")

    image=imagesG[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".G.mag.xz.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".G.mag.xz.png")

    image=imagesB[index]
    img_c2 = np.fft.fft2(image)
    img_c2 = np.fft.fftshift(img_c2)
    img_c2 = np.abs(img_c2);
    img_c3 = np.log(1+img_c2)
    imageio.imwrite(baseFileName + "_" + str(index) + ".B.mag.xz.png", img_c3)
    tempfilenames.append(baseFileName + "_" + str(index) + ".B.mag.xz.png")

# Average and single XZ DFTs
avgDFTR = []
avgDFTG = []
avgDFTB = []
singleDFTR = []
singleDFTG = []
singleDFTB = []
for index in range(imagesR.shape[2]):
    im = np.array(Image.open(baseFileName + "_" + str(index) + ".R.mag.xz.png"),dtype=np.float)
    if len(avgDFTR) == 0:
        avgDFTR = im / float(imagesR.shape[2])
        singleDFTR = im
    else:
        avgDFTR = avgDFTR + im / float(imagesR.shape[2])

    im = np.array(Image.open(baseFileName + "_" + str(index) + ".G.mag.xz.png"),dtype=np.float)
    if len(avgDFTG) == 0:
        avgDFTG = im / float(imagesR.shape[2])
        singleDFTG = im
    else:
        avgDFTG = avgDFTG + im / float(imagesR.shape[2])

    im = np.array(Image.open(baseFileName + "_" + str(index) + ".B.mag.xz.png"),dtype=np.float)
    if len(avgDFTB) == 0:
        avgDFTB = im / float(imagesR.shape[2])
        singleDFTB = im
    else:
        avgDFTB = avgDFTB + im / float(imagesR.shape[2])

# combine vector per axis DFTs into a single DFT
combinedDFT3 = np.sqrt(avgDFTR*avgDFTR + avgDFTG*avgDFTG + avgDFTB*avgDFTB)
combinedDFT2 = np.clip(exposure.equalize_hist(combinedDFT3, 256) * 256, 0, 255) # <- does a histogram equalization on the DFT to better visualize differences
combinedDFT = np.array(np.round(combinedDFT2),dtype=np.uint8)
Image.fromarray(combinedDFT,mode="L").save(baseFileName + ".combinedmag.equalized.xz.png")
combinedDFT4 = np.array(np.round(combinedDFT3),dtype=np.uint8)
Image.fromarray(combinedDFT4,mode="L").save(baseFileName + ".combinedmag.xz.png")


avgDFTR=np.array(np.round(avgDFTR),dtype=np.uint8)
avgDFTG=np.array(np.round(avgDFTG),dtype=np.uint8)
avgDFTB=np.array(np.round(avgDFTB),dtype=np.uint8)

avgDFT = np.empty([avgDFTR.shape[0], avgDFTR.shape[1], 3], dtype=np.uint8)
avgDFT[:,:,0] = avgDFTR
avgDFT[:,:,1] = avgDFTG
avgDFT[:,:,2] = avgDFTB
Image.fromarray(avgDFT,mode="RGB").save(baseFileName + ".mag.xz.png")

avgDFT = np.empty([avgDFTR.shape[0], avgDFTR.shape[1], 3], dtype=np.uint8)
avgDFT[:,:,0] = np.clip(exposure.equalize_hist(avgDFTR, 256) * 256, 0, 255)
avgDFT[:,:,1] = np.clip(exposure.equalize_hist(avgDFTG, 256) * 256, 0, 255)
avgDFT[:,:,2] = np.clip(exposure.equalize_hist(avgDFTB, 256) * 256, 0, 255)
Image.fromarray(avgDFT,mode="RGB").save(baseFileName + ".mag.equalized.xz.png")

singleDFTR=np.array(np.round(singleDFTR),dtype=np.uint8)
singleDFTG=np.array(np.round(singleDFTG),dtype=np.uint8)
singleDFTB=np.array(np.round(singleDFTB),dtype=np.uint8)

singleDFT = np.empty([singleDFTR.shape[0], singleDFTR.shape[1], 3], dtype=np.uint8)
singleDFT[:,:,0] = singleDFTR
singleDFT[:,:,1] = singleDFTG
singleDFT[:,:,2] = singleDFTB
#Image.fromarray(singleDFT,mode="RGB").save(baseFileName + ".mag0.xz.png")

# Make the graph of the energy csv
if os.path.isfile(baseFileName + ".energy.csv"):
    df = pd.read_csv(baseFileName + ".energy.csv")
    df.plot(xlabel='Percent', ylabel='Energy', legend=None)
    fig = plt.gcf()
    fig.savefig(baseFileName + "energy.png", bbox_inches='tight')
    plt.close(fig)

# Delete temporary files
for filename in tempfilenames:
    os.remove(filename)
