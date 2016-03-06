import csv
# from statistics import mean, pstdev, pvariance
# import numpy as np
import matplotlib.pyplot as plt
from kalman_filter import KalmanFilter


arrays = []
filtered_arrays = []
sums = []

current = None


def plot(plot_number, x, filtered_x, y, filtered_y, z, filtered_z):
    # Vertical distance between subplots
    HSPACE = 0.7

    # Figure
    plt.figure(1)
    plt.suptitle('Accelerometer: Position ' + str(plot_number))

    # X
    plt.subplot(311)
    plt.title('x vs filtered x')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer output')
    plt.plot(x, 'r')
    plt.plot(filtered_x, 'b')

    # Y
    plt.subplot(312)
    plt.title('y vs filtered y')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer output')
    plt.plot(y, 'r')
    plt.plot(filtered_y, 'b')

    # Z
    plt.subplot(313)
    plt.title('z vs filtered z')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer output')
    plt.plot(z, 'r')
    plt.plot(filtered_z, 'b')

    plt.subplots_adjust(hspace=HSPACE)
    # Maximize window

    plt.show()


with open("calibration.txt", "rU") as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    readings = list(spamreader)
    for index, row in enumerate(readings):
        if "#" in row[0]:
            arrays.append([])
            filtered_arrays.append([])
            sums.append((0, 0, 0))
            current = arrays[-1]
            current_filtered = filtered_arrays[-1]
            print (current)
            initial_values = list((int(i) for i in readings[index + 1]))
            x_Kalman = KalmanFilter(p=1000, r=100, q=0.5, k=0,
                                    initial_value=initial_values[0])
            y_Kalman = KalmanFilter(p=300, r=100, q=0.1, k=0,
                                    initial_value=initial_values[1])
            z_Kalman = KalmanFilter(p=1000, r=100, q=0.1, k=0,
                                    initial_value=initial_values[2])

        else:
            print (index)
            x, y, z = tuple(float(i) for i in row)
            current.append(tuple((x, y, z)))
            x_filt = x_Kalman.update(x)
            y_filt = y_Kalman.update(y)
            z_filt = z_Kalman.update(z)
            current_filtered.append(tuple((x_filt, y_filt, z_filt)))

            x += sums[-1][0] + x_filt
            y += sums[-1][1] + y_filt
            z += sums[-1][2] + z_filt

            sums[-1] = (x, y, z)

print(len(arrays))
for i, x in enumerate(arrays):
    x = [tup[0] for tup in arrays[i]]
    y = [tup[1] for tup in arrays[i]]
    z = [tup[2] for tup in arrays[i]]

    x_filt = [tup[0] for tup in filtered_arrays[i]]
    y_filt = [tup[1] for tup in filtered_arrays[i]]
    z_filt = [tup[2] for tup in filtered_arrays[i]]

    plot(i, x, x_filt, y, y_filt, z, z_filt)

    print("[")
    # for y in x:
    l = len((list(x)))
    s = sums[i]
    print(s[0]//l, s[1]//l, s[2]//l)

    print("]")







#res=[[-865,16905,836],
#[-16794,-182,223],
#[1044.7821080289384,0.7486185897435905,42.88906078525642],
#[17095,-18,1128],
#[-369,-17207,637],
#     [254,-187,17592],
#[356,-255,-15806]]
#
#resn = np.asarray(res)
#print(resn)
#
#outx0 = res[0][0]
#outx1 = res[1][0]
#outx2 = res[2][0]
#outx3 = res[3][0]
#
#outy0 = res[0][1]
#outy1 = res[1][1]
#outy2 = res[2][1]
#outy3 = res[3][1]
#
#
#offx = (outx0 + outx1 + outx2 + outx3)//4
#offy = (outy0 + outy1 + outy2 + outy3)//4
#sensx = (outx2 - outx1)//2
#cxy =  (outx0 - outx3)//(2*sensx)
#sensy = (outy0 - outy3)//2
#cyx =  (outy2 - outy1)//(2*sensy)
#
#print(offx,offy) #120 4176
#print(sensx,sensy) #8919 8461
#print(cxy,cyx) #-2 0
#
##outx = offx + sensx *
