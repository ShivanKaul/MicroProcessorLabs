#from  ohai import samples
from  sam import samples
import matplotlib.pyplot as plt 
from  kalman_filter import KalmanFilter
from statistics import mean, pstdev,pvariance

samples=samples[:100]
x = range(len(samples))
tens = []
tx= range(0,len(samples),25)
for i in tx:
	tens.append(samples[i:(i+25)])

m=max(samples)
print (m,samples.index(m))
m=min(samples)
print (m,samples.index(m))



t=[mean(i) for i in tens if len(i) != 0]
s=[pstdev(i) for i in tens if len(i) != 0]
v=[pvariance(i) for i in tens if len(i) != 0]
print(s)
print(max(s),sum(s))
print(v)
print(max(v),sum(v))
print(pstdev(samples))
kf = KalmanFilter(p=1000, r=1000, q=0.1, k=0, initial_value=samples[0])


kf2 = KalmanFilter(p=300, r=100, q=0.1, k=0, initial_value=samples[0])
kf3 = KalmanFilter(p=1000, r=100, q=0.1, k=0, initial_value=samples[0])
f = [kf.update(i) for i in samples]
f2 = [kf2.update(i) for i in samples]
f3 = [kf3.update(i) for i in samples]
plt.plot(x,samples,x,f,x,f2,tx,t,x,f3)
plt.show()
