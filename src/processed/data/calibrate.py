import cv2 
import numpy as np 
import os 
import glob 
from tqdm import tqdm

# Folder where the calibration images are stored
folder = '1/test'
#folder = '0/2023-05-04_10-37'

# --------------------------------------------

# Define the dimensions of checkerboard 
CHECKERBOARD = (6, 9) 

# stop the iteration when specified 
# accuracy, epsilon, is reached or 
# specified number of iterations are completed. 
criteria = (cv2.TERM_CRITERIA_EPS +
			cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001) 


# Vector for 3D points 
objpoints = [] 

# Vector for 2D points 
imagepoints = [] 


# 3D points real world coordinates 
objectp3d = np.zeros((1, CHECKERBOARD[0] 
					* CHECKERBOARD[1], 
					3), np.float32) 
objectp3d[0, :, :2] = np.mgrid[0:CHECKERBOARD[0], 
							0:CHECKERBOARD[1]].T.reshape(-1, 2) 
prev_img_shape = None

# Gather all images of tiff format in the folder
images = glob.glob(f'{folder}/*.tiff') 

#cv2.namedWindow("img", cv2.WINDOW_NORMAL)
#cv2.resizeWindow("img", 2256, 960)
count = 0

for filename in tqdm(images): 
	image = cv2.imread(filename) 
	grayColor = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY) 

	# Find the chess board corners 
	# If desired number of corners are 
	# found in the image then ret = true 
	ret, corners = cv2.findChessboardCorners( 
					grayColor, CHECKERBOARD, 
					cv2.CALIB_CB_ADAPTIVE_THRESH 
					+ cv2.CALIB_CB_FAST_CHECK +
					cv2.CALIB_CB_NORMALIZE_IMAGE) 

	# If desired number of corners can be detected then, 
	# refine the pixel coordinates and display 
	# them on the images of checker board 
	if ret == True: 
		objpoints.append(objectp3d) 
		count += 1
		print(filename, "Valid images: ", count)

		# Refining pixel coordinates 
		# for given 2d points. 
		corners2 = cv2.cornerSubPix( 
			grayColor, corners, (11, 11), (-1, -1), criteria) 

		imagepoints.append(corners2) 

		# Draw and display the corners 
		image = cv2.drawChessboardCorners(image, 
										CHECKERBOARD, 
										corners2, ret) 
		cv2.imwrite(f"calibrated/{filename}", image)
		#cv2.imshow('img', image) 
		#cv2.waitKey(0) 

cv2.destroyAllWindows() 

# Perform camera calibration by 
# passing the value of above found out 3D points (objpoints) 
# and its corresponding pixel coordinates of the 
# detected corners (imagepoints) 
ret, matrix, distortion, r_vecs, t_vecs = cv2.calibrateCamera( 
	objpoints, imagepoints, grayColor.shape[::-1], None, None) 

# Displaying required output 
print(" Camera matrix:") 
print(matrix) 

print("\n Distortion coefficient:") 
print(distortion) 


# undistort
image = cv2.imread(images[50])
image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY) 
h, w = image.shape[:2] 

newcameramtx, roi = cv2.getOptimalNewCameraMatrix(matrix, distortion, (w,h), 1, (w,h))
dst = cv2.undistort(image, matrix, distortion, None, newcameramtx)

# cropping
x, y, w, h = roi
crp = dst[y:y+h, x:x+w]
cv2.imwrite('undistorted.png', dst)
cv2.imwrite('cropped.png', crp)

# reprojection errors
mean_error = 0
for i in tqdm(range(len(objpoints))):
    imagepoints2, _ = cv2.projectPoints(objpoints[i], r_vecs[i], t_vecs[i], matrix, distortion)
    error = cv2.norm(imagepoints[i], imagepoints2, cv2.NORM_L2)/len(imagepoints2)
    mean_error += error

mean_error = (mean_error/len(objpoints))
print(f"mean reprojection error: {mean_error}")

