import csv
# from statistics import mean, pstdev, pvariance
import numpy as np
import matplotlib.pyplot as plt
from kalman_filter import KalmanFilter
from numpy.linalg import inv


def plot(plot_number, x, filtered_x, y, filtered_y, z, filtered_z,xm,ym,zm):
    # Vertical distance between subplots
    HSPACE = 0.7
    # Figure
    plt.figure(1)

    # X
    plt.subplot(311)
    plt.title('x vs filtered x')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer output')
    plt.plot(x, 'r')
    plt.plot([i[0] for  i in filtered_x], 'b')
    plt.plot(xm, 'g')
    plt.plot([i[1] for  i in filtered_x],'k')
    plt.plot([i[2] for  i in filtered_x],'c')



    # Y
    plt.subplot(312)
    plt.title('y vs filtered y')
    plt.xlabel('Time')  
    plt.ylabel('Accelerometer output')
    plt.plot(y, 'r')
    plt.plot([i[0] for  i in filtered_y], 'b')
    plt.plot(ym, 'g')
    plt.plot([i[1] for  i in filtered_y],'k')
    plt.plot([i[2] for  i in filtered_y],'c')

    # Z
    plt.subplot(313)
    plt.title('z vs filtered z')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer output')
    plt.plot(z, 'r')
    plt.plot([i[0] for  i in filtered_z], 'b')
    plt.plot(zm, 'g')
    plt.plot([i[1] for  i in filtered_z],'k')
    plt.plot([i[2] for  i in filtered_z],'c')

    plt.subplots_adjust(hspace=HSPACE)

    plt.show()
START = 0
END = 200

def run(filename):

    current_filtered = []
    sums = []
    current = []
    x_Kalman = KalmanFilter(p=1000, r=100, q=1, k=0)
    x_Kalmanq=KalmanFilter(p=1000, r=100, q=10, k=0)
    x_Kalmanr=KalmanFilter(p=1000, r=5000, q=1, k=0)
    y_Kalman = KalmanFilter(p=1000, r=100, q=10, k=0)
    y_Kalmanq = KalmanFilter(p=1000, r=100, q=10, k=0)
    y_Kalmanr = KalmanFilter(p=1000, r=500, q=1, k=0)
    z_Kalman = KalmanFilter(p=1000, r=100, q=1, k=0)
    z_Kalmanq = KalmanFilter(p=1000, r=100, q=10, k=0)
    z_Kalmanr = KalmanFilter(p=1000, r=500, q=10, k=0)


    with open(filename, "rU") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
        readings = list(spamreader) 

        x_Kalman.x,y_Kalman.x,z_Kalman.z=[float(i) for i in readings[0]]
        x_Kalman.x=x_Kalmanr.x=x_Kalman.x
        y_Kalman.x=y_Kalmanr.x=y_Kalman.x
        z_Kalman.x=z_Kalmanr.x=z_Kalman.x
        for index, row in enumerate(readings): 
            if(index >= START and index <= END):
                x, y, z = tuple(float(i) for i in row)
                current.append(tuple((x, y, z)))
                x_filt = x_Kalman.update(x)
                x_filtq = x_Kalmanq.update(x)
                x_filtr = x_Kalmanq.update(x)

                y_filt = y_Kalman.update(y)
                z_filt = z_Kalman.update(z)
                
                y_filtq = y_Kalmanq.update(y)
                y_filtr = y_Kalmanq.update(y)
                
                z_filtq = z_Kalmanq.update(z)
                z_filtr = z_Kalmanq.update(z)
                current_filtered.append(tuple(((x_filt,x_filtq,x_filtr), (y_filt,y_filtq,y_filtr), (z_filt,z_filtq,z_filtr))))

        display_graphs(current, current_filtered)
def movingaverage(interval, window_size):
    window = np.ones(int(window_size))/float(window_size)
    return np.convolve(interval, window, 'same')




def display_graphs(arrays, filtered_arrays, ):
    # X, Y, Z = [], [], []
    # print(len(arrays))
        x = [tup[0] for tup in arrays]
        y = [tup[1] for tup in arrays]
        z = [tup[2] for tup in arrays]
        x_mov=movingaverage(x,25)
        y_mov=movingaverage(y,25)
        z_mov=movingaverage(z,25)

        x_filt = [tup[0] for tup in filtered_arrays]
        y_filt = [tup[1] for tup in filtered_arrays]
        z_filt = [tup[2] for tup in filtered_arrays]

        plot(0, x, x_filt, y, y_filt, z, z_filt,x_mov,y_mov,z_mov)

        # X.append(x_filt)
        # Y.append(y_filt)
        # Z.append(z_filt)

    #calibrate(X, Y, Z, filtered_arrays)



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
run(filename="graphs.txt.txt")
#with means
#[[ -9.67223868e-04   5.94644222e-06  -3.68072277e-05]
# [  2.13216819e-05  -9.60147769e-04  -1.24902097e-05]
# [  5.51935007e-06  -7.84617414e-07  -9.76906248e-04]
# [  2.81318335e-02   5.23487466e-04   3.71370632e-02]]

#[[ -9.66493725e-04   8.99609236e-06  -4.04356587e-05]
# [  5.34716323e-05  -9.57953021e-04   1.80659093e-05]
# [ -1.14160279e-05   8.36236388e-06  -9.84442844e-04]
# [  3.16895602e-02   8.24420378e-03   6.24030831e-02]]
#[[ -9.66498919e-04   8.98969238e-06  -4.04285049e-05]
# [  5.30385665e-05  -9.57997244e-04   1.82331830e-05]
# [ -1.14033047e-05   8.36593441e-06  -9.84449750e-04]
# [  3.18412356e-02   8.25963175e-03   6.23445509e-02]]



#with all
#[[ -3.79226121e-06  -1.62022318e-08  -2.70411389e-08]
# [  9.33379973e-07  -9.59929934e-04   5.29055515e-05]
# [ -2.69424607e-05  -2.24065231e-05  -9.78024138e-04]
# [  3.56638753e-02   6.31073844e-03   5.08594589e-02]]

