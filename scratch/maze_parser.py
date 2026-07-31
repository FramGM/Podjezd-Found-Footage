import cv2
import numpy as np

def process_maze(image_path, output_path):
    print(f"Processing {image_path}...")
    img = cv2.imread(image_path)
    if img is None:
        print("Failed to load image.")
        return

    # 1. Resize to a manageable size (e.g., width 800)
    h, w = img.shape[:2]
    scale = 800.0 / w
    img = cv2.resize(img, (800, int(h * scale)))

    # 2. Convert to HSV to isolate the blue pen (and dark lines)
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    # Blue pen typically has hue around 100-140
    # Or just use adaptive threshold on grayscale since pen is dark
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Enhance contrast
    gray = cv2.equalizeHist(gray)
    
    # Adaptive threshold
    thresh = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 21, 10)

    # 3. Morphological operations to clean up noise and close gaps
    kernel = np.ones((3,3), np.uint8)
    thresh = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel, iterations=1)
    thresh = cv2.morphologyEx(thresh, cv2.MORPH_CLOSE, kernel, iterations=2)

    # 4. Thinning (Skeletonization)
    try:
        skeleton = cv2.ximgproc.thinning(thresh)
    except AttributeError:
        # Fallback if ximgproc is not installed
        size = np.size(thresh)
        skeleton = np.zeros(thresh.shape, np.uint8)
        ret, thresh = cv2.threshold(thresh, 127, 255, 0)
        element = cv2.getStructuringElement(cv2.MORPH_CROSS, (3,3))
        done = False
        while(not done):
            eroded = cv2.erode(thresh, element)
            temp = cv2.dilate(eroded, element)
            temp = cv2.subtract(thresh, temp)
            skeleton = cv2.bitwise_or(skeleton, temp)
            thresh = eroded.copy()
            zeros = size - cv2.countNonZero(thresh)
            if zeros == size:
                done = True

    # 5. Hough Line Transform to get segments
    lines = cv2.HoughLinesP(skeleton, 1, np.pi/180, threshold=20, minLineLength=15, maxLineGap=10)

    # 6. Draw lines on a new image for verification
    result = np.ones_like(img) * 255
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            cv2.line(result, (x1, y1), (x2, y2), (0, 0, 255), 2)
    
    cv2.imwrite(output_path, result)
    print(f"Saved result to {output_path}. Found {len(lines) if lines is not None else 0} lines.")

process_maze(r'C:\Users\vlade\.gemini\antigravity\brain\f98d681e-f8d6-4de2-ac12-476e98998e9d\.user_uploaded\media__1785273187259.jpg', 'maze_detected.png')
