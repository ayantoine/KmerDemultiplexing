#! /bin/bash

R1=$1
R2=$2
BARCODE=$3
PID=$4

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

${DIR}/CreateKmerList.dist/CreateKmerList.exe -m ${BARCODE} -o ${BARCODE}.Kmer -p ${PID}
${DIR}/MakeAssignation.dist/MakeAssignation.exe -1 ${R1} -2 ${R2} -k ${BARCODE}.Kmer -p ${PID}
${DIR}/CutReads.dist/CutReads.exe -f ${R1} -r ${PID}_Hyper_Identified.tsv -p ${PID}
${DIR}/CutReads.dist/CutReads.exe -f ${R2} -r ${PID}_Hyper_Identified.tsv -p ${PID}

echo "------ Write output ------"
cat ${PID}_Hyper_Identified.tsv ${PID}_Hypo_2_Identified.tsv ${PID}_Ambiguous_2.tsv ${PID}_Unidentified.tsv > ${PID}_Demultiplexing_Hyper.tsv
cut -f2 ${PID}_Demultiplexing_Hyper.tsv | sort | uniq -c | awk '{print $2"\t"$1}' > ${PID}_Demultiplexing_Hyper_Distribution.tsv
cat ${PID}_Hyper_Identified.tsv ${PID}_Hypo_1_Identified.tsv ${PID}_Ambiguous_1.tsv ${PID}_Unidentified.tsv > ${PID}_Demultiplexing_Global.tsv
cut -f2 ${PID}_Demultiplexing_Global.tsv | sort | uniq -c | awk '{print $2"\t"$1}' > ${PID}_Demultiplexing_Global_Distribution.tsv
wc -l ${PID}_Hyper_Identified.tsv ${PID}_Hypo_1_Identified.tsv ${PID}_Hypo_2_Identified.tsv ${PID}_Ambiguous_1.tsv ${PID}_Unidentified.tsv ${PID}_Demultiplexing_Global.tsv | head -n -1
echo "------ /Write output ------"
