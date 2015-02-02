# disk-based-mergesort
A program that performs the following merge sort techniques on records for large files when input buffer in memory is limited:

<ol>
<li> Basic merge sort </li>
<li> <p> Mutli-step merge sort wherein the file is divided into chunks, merge sort is applied to produce "super runs" and the "super runs" are merged to produce the sorted file </p> </li>
<li> Replacement selection technique using min heap </li>
</ol>
