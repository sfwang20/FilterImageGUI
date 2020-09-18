import cv2
import numpy as np

file_name = 'faceDetect.png'

input = cv2.imread(file_name)

result = cv2.resize(input, (750,421))

cv2.imwrite(file_name, result)