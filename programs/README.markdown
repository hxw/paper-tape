# Paper Tape Programs

These programs tke either 8bit (.hex8) or 5 bit (.hex5) files
and convert to another form:

Programs:
* elliott2ascii.c - convert hex5 file to H-Code form i.e., with < > →
* hex8.c - converts hex8 file to palin ASCII
* splitter.c - split hex file into multiples used to split legible leaders from code
* trim-runout.c - the capture program run from `tip` to write hex files
* vis.c - to view hex files to see legible leaders and manually check for misreads
