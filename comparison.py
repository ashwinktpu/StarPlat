#please change accordingly
#this program is used to compare two files(fiels have two columns) with second column and try to see their difference is greater than some threshold value or not.

import numpy as np
file="C:\\Users\\ashwina\\Downloads\\prLJ.txt"
file1="C:\\Users\\ashwina\\Downloads\\outputPRMain-input.txt"
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

print(result)

for x in lines1:
    result1.append(x.split("  ")[1])
    
f1.close()
print(result1)

new = []
for sub in result:
    new.append(sub.replace("\n", ""))
print(new)


new1 = []
for sub in result1:
    new1.append(sub.replace("\n", ""))
print(new1)
count=0;
 
for i in range(len(result)):
    if abs(float(result[i]) - float((result1[i]))) > 0.000001:
        count = count+1

print(count)

