import re

f = open('main', 'rb+')
originalBinary = f.read()

# 
dumpedFunctions = ['clEnqueueWriteBuffer']

libOpenCL = 'libOpenCL.so'

# Looking for the position of the end of 'libOpenCL.so' in the binary.
pos1 = originalBinary.find(libOpenCL)+len(libOpenCL)
# Taking the first dot is not relevant: the a call may be on libOpenCL.X... or not. Let's take the position of the first openCL function instead.
pos2 = pos1 + originalBinary[pos1:].find('cl')
# The next dot will fit with a new library call.
pos3 = pos2 + originalBinary[pos2:].find('.')

# the list of OpenCL used, plus the beginning of the next library name
Sfunctions = originalBinary[pos2:pos3]
# Delete the next library name
Sfunctions = Sfunctions[:Sfunctions.rfind('\0')]
print(Sfunctions)
Afunctions = Sfunctions.split('\0')

#Afunctions.remove('clCreateCommandQueue')
Afunctions = ['cp'+x[2:] for x in Afunctions]
SfunctionsNew = '\0'.join(Afunctions)

newBinary = originalBinary

newBinary = originalBinary.replace(Sfunctions, SfunctionsNew)

#newBinary = newBinary.replace('libOpenCL.so', 'libOpenCP.so')
myFile = open('newMain', 'wb+')
myFile.write(newBinary)
myFile.close()

f.close()
