import os

import cv2
import numpy as np
import apriltag

'''
small script that works through image in a folder and detects AprilTags
in each image. output is a text file for each image indicating AprilTag
detections and their position in the image
'''

#general configuration parameters
scale = 0.33  #image scale - typically helps to reduce large (4k and greater) images to 0.25 - 0.5x original scale
visualize = False  #show detections in display window

#AprilTag detector configuration parameters
family = 'tag36h11'
border = 1
nthreads = 4
quad_decimate = 1.0  #image scale factor
quad_blur = 0.0  # gaussian blur sd (pixels)
refine_edges = True   #refine edges of tag based on image gradients
refine_decode = False
refine_pose = False
debug = False
quad_contours = True

options = apriltag.DetectorOptions(families=family, border=border, nthreads=nthreads, quad_decimate=quad_decimate,
                                   quad_blur=quad_blur, refine_edges=refine_edges, refine_decode=refine_decode,
                                   refine_pose=refine_pose, debug=debug, quad_contours=quad_contours)

detector = apriltag.Detector(options)

image_folder = './data'
named_window = 'detections'

image_extensions = ['.jpg', '.JPG']

tags_found = set()
for img in os.listdir(image_folder):
    file_parts = os.path.splitext(img)
    if not file_parts[1] in image_extensions:
        continue

    image = cv2.imread(os.path.join(image_folder, img), cv2.IMREAD_GRAYSCALE)

    h = int(image.shape[0] * scale)
    w = int(image.shape[1] * scale)
    image = cv2.resize(image, (w, h), interpolation=cv2.INTER_LINEAR)
    detections = detector.detect(image)

    outfile_path = os.path.join(image_folder, file_parts[0]+'_tags.txt')
    outfile = open(outfile_path, 'w')
    outfile.write('type\tunique_id\tx_c\ty_c\n')

    for detection in detections:
        center = detection.center  # x (col), y (row)
        outfile.write('{:d}\t{:d}\t{:f}\t{:f}'.format(0, detection.tag_id, center[0]/scale, center[1]/scale))
        tags_found.add(detection.tag_id)

        if visualize:
            print(center)
            center = (int(center[0]), int(center[1]))
            text = str(detection.tag_id)
            cv2.putText(image, text, center, cv2.FONT_HERSHEY_SIMPLEX,
                        fontScale=1, color=(0, 0, 0), thickness=2)

    outfile.close()
    if visualize:
        cv2.imshow(named_window, image)
        cv2.waitKey(0)

print('found tags: {}'.format(tags_found))