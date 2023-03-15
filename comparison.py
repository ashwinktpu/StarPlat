
"""
How to run  :  python name_of_python_script  <filename1> <filename2>
Output  : Count of non-matching rows in both files
"""


import sys

#first file name
file = sys.argv[1] 
#second file name
file1 = sys.argv[2] 
f=open(file,"r")
f1=open(file1,"r")
lines=f.readlines()
lines1=f1.readlines()
#print(lines);
result=[]
result1=[]
for x in lines:
    result.append(x.split("  ")[1])
    
f.close()

#print(result)

for x in lines1:
    result1.append(x.split("  ")[1])
    
f1.close()
#print(result1)

new = []
for sub in result:
    new.append(sub.replace("\n", ""))
#print(new)


new1 = []
for sub in result1:
    new1.append(sub.replace("\n", ""))
#print(new1)
count=0;
 
for i in range(len(result)):
    if abs(round(float(result[i]),3) - round(float((result1[i])),3)) > 0.000001:
        count = count+1

print(count)
