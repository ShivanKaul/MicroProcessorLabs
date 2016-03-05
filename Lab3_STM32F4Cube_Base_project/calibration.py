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
#     print(s[0]//l,s[1]//l,s[2]//l)
#
#     print("]")
res=[[-865,16905,836],
[-16794,-182,223],
[1044.7821080289384,0.7486185897435905,42.88906078525642],
[17095,-18,1128],
[-369,-17207,637],
     [254,-187,17592],
[356,-255,-15806]]

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


offx = (outx0 + outx1 + outx2 + outx3)//4
offy = (outy0 + outy1 + outy2 + outy3)//4
sensx = (outx2 - outx1)//2
cxy =  (outx0 - outx3)//(2*sensx)
sensy = (outy0 - outy3)//2
cyx =  (outy2 - outy1)//(2*sensy)

print(offx,offy) #120 4176
print(sensx,sensy) #8919 8461
print(cxy,cyx) #-2 0

#outx = offx + sensx *