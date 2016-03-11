import csv
# from statistics import mean, pstdev, pvariance
import numpy as np
import matplotlib.pyplot as plt
from kalman_filter import KalmanFilter
from numpy.linalg import inv


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

    plt.show()


def run(filename):
    arrays = []
    filtered_arrays = []
    sums = []
    current = None

    with open(filename, "rU") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
        readings = list(spamreader)
        for index, row in enumerate(readings):
            if "#" in row[0]:
                arrays.append([])
                filtered_arrays.append([])
                sums.append((0, 0, 0))
                current = arrays[-1]
                current_filtered = filtered_arrays[-1]
                initial_values = list((float(i) for i in readings[index + 1]))
                x_Kalman = KalmanFilter(p=1000, r=50, q=0.1, k=0,
                                        initial_value=initial_values[0])
                y_Kalman = KalmanFilter(p=1000, r=50, q=0.1, k=0,
                                        initial_value=initial_values[1])
                z_Kalman = KalmanFilter(p=1000, r=50, q=0.1, k=0,
                                        initial_value=initial_values[2])

            else:
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

        display_graphs(arrays, filtered_arrays, sums)


def show_means(i, sums, x):
    print("[")
    l = len((list(x)))
    s = sums[i]
    print(s[0]//l, s[1]//l, s[2]//l)
    print("]")


def display_graphs(arrays, filtered_arrays, sums):
    X, Y, Z = [], [], []
    # print(len(arrays))
    for i, x in enumerate(arrays):
        x = [tup[0] for tup in arrays[i]]
        y = [tup[1] for tup in arrays[i]]
        z = [tup[2] for tup in arrays[i]]

        x_filt = [tup[0] for tup in filtered_arrays[i]]
        y_filt = [tup[1] for tup in filtered_arrays[i]]
        z_filt = [tup[2] for tup in filtered_arrays[i]]

        plot(i, x, x_filt, y, y_filt, z, z_filt)

        # show_means(i, sums, x)

        X.append(x_filt)
        Y.append(y_filt)
        Z.append(z_filt)

    calibrate(X, Y, Z, filtered_arrays)


def solveForX(w, Y):
    # Check following link for formula:
    # https://www.dropbox.com/s/qnzay1dfxjkmftk/Screen%20Shot%202016-03-07%20at%202.09.35%20PM.png?dl=0
    return np.dot(np.dot(inv(np.dot(w.transpose(), w)), w.transpose()), Y)


def calibrate(X, Y, Z, filtered_arrays):
    Y0 = np.asarray([(0, -1, 0)]) #* len(X[0]))
    Y1 = np.asarray([(1, 0, 0)]) #* len(X[1]))
    Y2 = np.asarray([(-1, 0, 0)]) #* len(X[2]))
    Y3 = np.asarray([(0, 1, 0)])# * len(X[3]))
    Y4 = np.asarray([(0, 0, -1)])# * len(X[4]))
    Y5 = np.asarray([(0, 0, 1)])# * len(X[5]))

    Y = np.vstack((Y0, Y1, Y2, Y3, Y4, Y5))
    sum0=sum1=sum2=sum3=sum4=sum5=(0,0,0)
    w0 = np.asarray(xyzmeans(filtered_arrays[0][200:]))
    w1 = np.asarray(xyzmeans(filtered_arrays[1]))
    w2 = np.asarray(xyzmeans(filtered_arrays[2]))
    w3 = np.asarray(xyzmeans(filtered_arrays[3][200:350]))
    w4 = np.asarray(xyzmeans(filtered_arrays[4][250:]))
    w5 = np.asarray(xyzmeans(filtered_arrays[5][:250]))

    w = np.vstack((w0, w1, w2, w3, w4, w5))

    X = solveForX(w, Y)

    print ('X is :')
    print (X)
def xyzmeans(array):
    xs=0
    ys=0
    zs=0
    for x,y,z in array:
        xs+=x
        ys+=y
        zs+=z
    return (xs/len(array),ys/len(array),zs/len(array),1)
# Start main loop
run(filename="calrepeat.txt")
#with means
#[[ -9.67223868e-04   5.94644222e-06  -3.68072277e-05]
# [  2.13216819e-05  -9.60147769e-04  -1.24902097e-05]
# [  5.51935007e-06  -7.84617414e-07  -9.76906248e-04]
# [  2.81318335e-02   5.23487466e-04   3.71370632e-02]]

#[[ -9.66493725e-04   8.99609236e-06  -4.04356587e-05]
# [  5.34716323e-05  -9.57953021e-04   1.80659093e-05]
# [ -1.14160279e-05   8.36236388e-06  -9.84442844e-04]
# [  3.16895602e-02   8.24420378e-03   6.24030831e-02]]


#with all
#[[ -3.79226121e-06  -1.62022318e-08  -2.70411389e-08]
# [  9.33379973e-07  -9.59929934e-04   5.29055515e-05]
# [ -2.69424607e-05  -2.24065231e-05  -9.78024138e-04]
# [  3.56638753e-02   6.31073844e-03   5.08594589e-02]]

