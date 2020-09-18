# FilterImageGUI - 簡易影像處理軟體

## Features

### 基本GUI功能(Basic GUI features)
 Qt介面之功能(toolbar)，包括縮放(zoom in/out)、放大某區域(zomm x30)並移動檢視(move)、
 儲存圖片、button及Trackerbar等。
 
### 開發功能
(1) 色彩空間(color space)之轉換 (BGR、Gray、HSV、HSL)

(2) 圖片濾鏡：lomo效果、卡通化效果、直方圖均衡、素描圖效果

(3) 邊緣檢測：可顯示對影像進行Canny、Sobel、Laplace檢測之效果。

(4) 對比度(contrast)及明亮度(brightness)調整：可透過圖片下方之Trackerbar進行調整。

(5) 人臉偵測(face detection)：使用CascadeClassifer進行偵測。


## Demo
### Color space convertion
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/hsv.png)

### Lomography effect


### Sketch effect
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/sketch.png)

### Cartonize effect
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/cartonize.png)

### Edge detect (Laplace operator)
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/laplacian.png)

### Contrast and brightness adjustment
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/contrast.png)

### Face detection
![image](https://github.com/sfwang20/FilterImageGUI/blob/master/demo/faceDetect.png)

