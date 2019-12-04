# coding: utf-8
"""Python3.6"""
# compatibility: python2.7, python2.6

import time
from optparse import OptionParser

sCurrentVersionScript="v3"
iTime1=time.time()
########################################################################
'''
V3-2019/11/18
Standalone version
Remove working by Sample: work on all sequences, now

V2-2019/10/23
Add SampleId into name
V1-2019/10/22
Split fastq by SampleId and store into specific SampleFolder (All SampleFolder
must be already existing)
Remove linker during process

python SplitReads.py -f FASTQ -r REFFILE -s SAMPLE -p PID -i PAIRID
FASTQ: Fastq with all sequences
REFFILE: ${PID}_Hyper_Identified.tab
SAMPLE: SampleId
PID: Processus Id
PAIRID: id of the pair end file
'''
########################################################################
#CONSTANT
ILLUMINA_PAIR_TAG=":N:0:"

CUT_TAG="cut.fq"

VERBOSE=10000
########################################################################
#Options
parser = OptionParser()
parser.add_option("-f","--fastq", dest="fastq")
parser.add_option("-r","--ref", dest="ref")
# parser.add_option("-s","--sample", dest="sample")
parser.add_option("-p","--pid", dest="pid")
# parser.add_option("-i","--pairid", dest="pairid")

(options, args) = parser.parse_args()

sFastq=options.fastq
if not sFastq:
	exit("Error : no fastq -f defined, process broken")

sRef=options.ref
if not sRef:
	exit("Error : no ref -r defined, process broken")
	
sPid=options.pid
if not sPid:
	exit("Error : no pid -p defined, process broken")
	
# sPairId=options.pairid
# if not sPairId:
	# exit("Error : no pairid -i defined, process broken")

# sSampleId=options.sample
# if not sSampleId:
	# exit("Error : no sample -s defined, process broken")
# sSampleTag=sPid+"_"+sSampleId

########################################################################
#Function 	
def LoadRef(sPath): #,sSample,sPair):
	dResult={}
	for sNewLine in open(sPath):
		sLine=sNewLine.strip()
		tLine=sLine.split("\t")
		sSeqName=tLine[0]
		sSample=tLine[1]
		sEndIndex=tLine[2]
		dResult[sSeqName]=(sSample,int(sEndIndex))
	return dResult

def WriteSplitFastq(sPath,dList): #,sSID):
	FILE=open(sPath+"."+CUT_TAG,"w")
	sSeqName=""
	sContent=""
	sInterline=""
	sQuality=""
	iLineCounter=0
	iSeqCount=0
	dAssociationCount=0
	for sNewLine in open(sPath):
		iLineCounter+=1
		if iLineCounter%4==1:
			iSeqCount+=1
			if iSeqCount%VERBOSE==0:
				print("{} : {} sequences reached...".format(sPath,iSeqCount))
			if sSeqName!="":
				try:
					sSample=dList[sSeqName[1:-1]][0] #remove starting @ and ending \n
					iEndIndex=dList[sSeqName[1:-1]][1] #remove starting @ and ending \n
					# print(iEndIndex)
					# print(sSeqName)
					# print(sContent)
					# print(sInterline)
					# print(sQuality)
					sSeqName=sSeqName.replace("\n"," "+sSample+"\n") 
					FILE.write(sSeqName+sContent[iEndIndex:]+sInterline+sQuality[iEndIndex:])
				except KeyError:
					pass
			sSeqName=sNewLine
			sContent=""
			sInterline=""
			sQuality=""
		elif iLineCounter%4==2:
			sContent+=sNewLine
		elif iLineCounter%4==3:
			sInterline+=sNewLine
		else:
			sQuality+=sNewLine
	if sSeqName!="":
		try:
			sSample=dList[sSeqName[1:-1]][0] #remove starting @ and ending \n
			iEndIndex=dList[sSeqName[1:-1]][1] #remove starting @ and ending \n
			sSeqName=sSeqName.replace("\n"," "+sSample+"\n") 
			FILE.write(sSeqName+sContent[iEndIndex:]+sInterline+sQuality[iEndIndex:])
		except KeyError:
			pass
	FILE.close()
		
########################################################################
#MAIN
if __name__ == "__main__":
	dListOfSeq=LoadRef(sRef) #,sSampleTag)#,sPairId)
	WriteSplitFastq(sFastq,dListOfSeq) #,sSampleId)
	
	
########################################################################    
iTime2=time.time()
iDeltaTime=iTime2-iTime1
print("Script done: "+str(iDeltaTime))

