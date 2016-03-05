import csv
from statistics import mean, pstdev,pvariance
import numpy as np
# arrays=[]
# sums =[]
# current = None
# with open("calibration.txt","rU") as csvfile:
#     spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
#     for row in spamreader:
#         if "#" in row[0]:
#             arrays.append([])
#             sums.append((0,0,0))
#             current = arrays[-1]
#         else:
#             current.append(list(float(i) for i in row))
#             x,y,z=sums[-1]
#             x+= current[-1][0]
#             y+= current[-1][1]
#             z+= current[-1][2]
#             sums[-1]=(x,y,z)
#
# print(len(arrays))
# for i,x in enumerate(arrays):
#     #print("[")
#     #for y in x:
#     l=len((list(x)))
#     s=sums[i]
#     print(s[0]/l,s[1]/l,s[2]/l)

    #print("]")
res=[[105.58301899065407,1032.4593908847346,136.0305700996885],
[-1022.4595920641407,4.06886006413994,3.2426035043731782],
[1044.7821080289384,0.7486185897435905,42.88906078525642],
[-59.751989731292504,-1048.452440343537,25.557132680272098],
[17.94565776444444,4.200595555555557,1073.2743961511105],
[17.938959349593492,49.095081235772334,-963.5850147804882]]

resn = np.asarray(res)
print(resn)

outx0 = res[0][0]
outx1 = res[1][0]
outx2 = res[2][0]
outx3 = res[3][0]

outy0 = res[0][1]
outy1 = res[1][1]
outy2 = res[2][1]
outy3 = res[3][1]


offx = (outx0 + outx1 + outx2 + outx3)/4
offy = (outy0 + outy1 + outy2 + outy3)/4
sensx = (outx2 - outx1)/2
cxy =  (outx0 - outx3)/(2*sensx)
sensy = (outy0 - outy3)/2
cyx =  (outy2 - outy1)/(2*sensy)

outx = offx + sensx *