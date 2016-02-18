import ohai.samples as samples
import matplotlib.pyplot as plt 
import kalmanfilter.KalmanFilter

x = xrange(len(samples))
kf = KalmanFilter()

filtered = [kf.update(i) for i in samples]

plt.plot(x,samples,x,filtered)
plt.show()