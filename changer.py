IN_FILE = "Lab2_STM32F4Cube_Base_project/samples.txt"
OUT_FILE = "sam.py"

ifile = open(IN_FILE,"rU")
ofile = open(OUT_FILE,"w+")
olist = []
for i in ifile.read().splitlines():
  try:
    olist.append(float(i.strip()))
  except:
    pass

print >>ofile,"samples=", olist
