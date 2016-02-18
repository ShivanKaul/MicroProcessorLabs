from  ohai import samples
import matplotlib.pyplot as plt 
from  kalman_filter import KalmanFilter

x = xrange(len(samples))
kf = KalmanFilter(p=0.1,r=300,q=0.1,k=0,initial_value=samples[0])
kf2 = KalmanFilter(p=10,r=100,q=0.1,k=0,initial_value=samples[0])
f = [kf.update(i) for i in samples]
f2 = [kf2.update(i) for i in samples]
plt.plot(x,samples,x,f,x,f2)
plt.show()