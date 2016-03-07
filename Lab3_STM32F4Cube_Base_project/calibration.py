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
                initial_values = list((int(i) for i in readings[index + 1]))
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
    return np.dot(np.dot(inv(np.dot(w.transpose(), w)), w.transpose()), Y)


def calibrate(X, Y, Z, filtered_arrays):
    Y0 = np.asarray([(0, -1, 0)] * len(X[0]))
    Y1 = np.asarray([(1, 0, 0)] * len(X[1]))
    Y2 = np.asarray([(-1, 0, 0)] * len(X[2]))
    Y3 = np.asarray([(0, 1, 0)] * len(X[3]))
    Y4 = np.asarray([(0, 0, -1)] * len(X[4]))
    Y5 = np.asarray([(0, 0, 1)] * len(X[5]))

    Y = np.vstack((Y0, Y1, Y2, Y3, Y4, Y5))

    w0 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[0]])
    w1 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[1]])
    w2 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[2]])
    w3 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[3]])
    w4 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[4]])
    w5 = np.asarray([(x, y, z, 1) for (x, y, z) in filtered_arrays[5]])

    w = np.vstack((w0, w1, w2, w3, w4, w5))

    X = solveForX(w, Y)

    print ('X is :')
    print (X)

# Start main loop
run(filename="calibration.txt")
