import cv2
import numpy as np

a = cv2.imread('2.jpg')


c = cv2.resize(a, (1024,576))

cv2.imwrite('2.png', c)