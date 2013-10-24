#Awk based message pipe for translating gcc output to MsDev compatible tags
BEGIN {
	#print "-------- gccpipe.awk - ver 0.10 --(gcc to MSDEV ) -------->>-"
	MakeMsg=""
}

function IsDigit(str) 
{ 
	for(i=1;i<=length(str);i++) 
	{ 
		bit=substr(str,i,1) 
		if(bit<0 || bit>9) return(0) 
	} 
	return(1) 
} 

function CheckmsDir(dir) 
{ 
	##where = match(dir, "\\/firmware\\/")
	##if (where != 0)
	##{
	##	return 3
	##}
	where = match(dir, "\\/ecos-")
	if (where != 0)
	{
		return 2
	}
	where = match(dir, "\\/cygdrive\\/")
	if (where != 0)
	{
		return 1
	}

	return 0
} 

function GetmsDir(dir) 
{ 
	type = CheckmsDir(dir)
	if (type == 0)
	{
		print "awk parse error in GetmsDir() " dir
		return(dir) 
	}

	if (type == 1)
	{
		msDir = gensub("\\/cygdrive\\/", "", 1, dir) 
		msDir = gensub("\\/", "\\\\","g", msDir) 
		msDrive = substr(msDir, 1, 1)
		msDir = substr(msDir, 2)
		msDir = msDrive ":" msDir
	}
	else if (type == 2)
	{
		msDir = gensub("\\/ecos-", "", 1, dir) 
		msDir = gensub("\\/", "\\\\","g", msDir) 
		msDrive = substr(msDir, 1, 1)
		msDir = substr(msDir, 2)
		msDir = msDrive ":" msDir
	}
	else if (type == 3)
	{
		rootdir = ENVIRON["CYGWIN_PATH"]
		msDir = gensub("\\/", "\\\\","g", dir)
		fullDir = rootdir msDir
		return (fullDir)
	}
	
	return(msDir) 
} 

function ExtractmsDirPos(dir) 
{ 
	wherelast = 0
	dirlast = dir
	while (1)
	{
		where = match(dirlast, "\\\\")
		if (where == 0) break
		wherelast += where
		dirlast = substr(dirlast, where + 1)
	}
	
#	print "wherelast : " wherelast
#	print "dirlast : " dirlast
#	print "dir : " substr(dir, 1, wherelast - 1)

	return wherelast
} 

#($1~"-") {
#  next;
#}

#($0~"  ") {
#  next;
#}

($1=="makedepend:") {
# makedepend: warning:  fis.c, line 77: cannot find include file "fis1.h"
# makedepend: warning:  fis.c (reading fis.h, line 58): cannot find include file "fis1.h"
# makedepend: warning:  fis.c (reading /cygdrive/c/dice/diceApp/avs/avs.h, line 38): cannot find include file "fis1.h"
#	printf("*** 0:%s 1:%s 2:%s 3:%s 4:%s 5:%s 6:%s 7:%s\n",$0,$1,$2,$3,$4,$5,$6,$7);

	if (match($2, "warning:") > 0)
	{
		L = length($1)+length($2)+length($3)+length($4)+length($5)
		ending = substr($0,L+6);
		msDir = GetmsDir(DIR) 
		msSrcFile = 0
		msSrcFile = 0
		msFile = $3
		msLine = 0
		if ($4 == "(reading")
		{
			L = length($1)+length($2)+length($3)+length($4)+length($5)+length($6)+length($7)
			ending = substr($0,L+9);
			msSrcFile = msFile
			msSrcDir = msDir
			msFile = $5
			type = CheckmsDir(msFile)
			if (type != 0)
			{
				msDir = GetmsDir(msFile)
				where = ExtractmsDirPos(msDir)
				msFile = substr(msDir, where + 1)
				msDir = substr(msDir, 1, where - 1)
			}
			else
			{
				msDir = GetmsDir(DIR) 
			}
			msLinePos = $7
		}
		else
		{
			msLinePos = $5
		}

		where = match(msFile, ",")
		if (where != 0)
		{
			msFile = substr(msFile, 1, where - 1)
		}
		msLine = 0
		where = match(msLinePos, ":")
		if (where != 0)
		{
			msLine = substr(msLinePos, 1, where - 1)
		}
		where = match(msLinePos, ")")
		if (where != 0)
		{
			msLine = substr(msLinePos, 1, where - 1)
		}

		if (IsDigit(msLine) == FALSE) msLine = 0

		if (msSrcDir != 0 && msSrcFile != 0)
		{
			printf("%s\\%s: warning when reading include file %s\n", msSrcDir, msSrcFile, msFile);
		}
		if (msLine != 0)
		{
			printf("%s\\%s(%s):%s %s %s\n", msDir, msFile, msLine, $1, $2, ending);
		}
		else
		{
			# don't know how to parse yet
			print "awk parse error in makedepend (warning) " $0
		}
	}
	else
	{
		# don't know how to parse yet
		print "awk parse error in makedepend " $0
	}
	next;
}

($1=="not" && $2=="in") {
# make depend following error messages
# not in fis1.h 
# not in /cygdrive/h/dice/kernel/install/include/fis1.h
#	printf("** 0:%s 1:%s 2:%s 3:%s 4:%s 5:%s\n",$0,$1,$2,$3,$4,$5);

# convert to msDir format
#	where = match($3, "\\/cygdrive\\/")
#	if (where != 0)
#	{
#		msDir = GetmsDir($3) 
#		printf(" %s %s %s\n", $1, $2, msDir);
#	}
#	else
#	{
#		print $0
#	}

#	print raw error message
#	print $0

	next;
}

($1!="") {
# handling of source file parsing errors and warnings
# fis.c:116: `int33' undeclared (first use in this function)
# fis.c:115: warning: unused variable `status'

# handling of include file parsing errors
# In file included from avs.c:36:
# avs.h:98: error: parse error before "rxDevType"
# /cygdrive/c/dice/diceApp/dice/diceClock.h:99: error: parse error before "rxDevType"
# /ecos-c/tcat/diceApp/TCKernel/TCSemaphore.h: In function `TCMutexOpen':
# /ecos-c/tcat/diceApp/TCKernel/TCSemaphore.h:41: error: `NO_ERROR' undeclared (first use in this function)
#
# In file included from /cygdrive/c/dice/diceApp/TCKernel/TCTasking.h:30,
#                  from avs.c:27:
# /cygdrive/c/dice/diceApp/TCKernel/TCTypes.h:26: error: syntax error before "typedef"

# handling of linking errors and warnings
# if (LINK==1)
# cyg_main.o(.text.mainInitializeTasks+0x48):/cygdrive/c/dice/diceApp/main/cyg_main.c:203: undefined reference to `myAppInitializeTasks'
# cyg_main.o(.text.mainInitialize+0xf0): In function `mainInitialize':
# /cygdrive/c/dice/diceApp/main/cyg_main.c:274: undefined reference to `avsTestCliInstallTools'
# cyg_main.o(.text.mainInitialize+0x1a4):/cygdrive/c/dice/diceApp/main/cyg_main.c:396: undefined reference to `myAppInitialize'
# collect2: ld returned 1 exit status

#	printf("* 0:%s 1:%s 2:%s 3:%s 4:%s 5:%s\n",$0,$1,$2,$3,$4,$5);

	L = length($1)
	ending = substr($0,L+1);
	msLine = 0

	if (LINK==1) # link specific pre-parsing
	{
		if ($1=="collect2:")
		{
			print $0
			next
		}
		msFile = $1
		type = CheckmsDir(msFile)
		if (type != 0)
		{
			where = match(msFile, ":")
			if (where != 0)
			{
				msFileLink = substr(msFile, 1, where - 1)
				msFile = substr(msFile, where + 1)
				type = CheckmsDir(msFile)
				if (type != 0)
				{		
					type = CheckmsDir(msFileLink)
					endingLink = "from source file:"
					if (type != 0)
					{
						printf("%s: %s\n", GetmsDir(msFileLink), endingLink)
					}
					else
					{
						printf("%s\\%s: %s\n", GetmsDir(DIR), msFileLink, endingLink)
					}
				}
				else
				{
					msFile = $1
				}
			}
		}
	}
	else if ($1=="In" && $2=="file" && $3=="included" && $4=="from")
	{
		msFile = $5
		test = substr(msFile,length(msFile));
		if (test == ",")
		{
			ending = " error: parse error when first included from:"
		}
		else
		{
			ending = " error: parse error in included file:"
		}
	}
	else if ($1=="from")
	{
		msFile = $2
		ending = " error: parse error when including file:"
	}
	else
	{
		msFile = $1
	}

	type = CheckmsDir(msFile)
	if (type != 0)
	{
		msDir = GetmsDir(msFile)
		where = ExtractmsDirPos(msDir)
		msFile = substr(msDir, where + 1)
		msDir = substr(msDir, 1, where - 1)
	}
	else
	{
		msDir = GetmsDir(DIR) 
	}

	where = match(msFile, ":")
	if (where != 0)
	{
		msLine = substr(msFile, where + 1)
		msFile = substr(msFile, 1, where - 1)
		where = match(msLine, ":")
		if (where != 0)
		{
			msLine = substr(msLine, 1, where - 1)
		}
		where = match(msLine, ",")
		if (where != 0)
		{
			msLine = substr(msLine, 1, where - 1)
		}
		if (IsDigit(msLine) == FALSE) msLine = 0
	}
	
	if (msLine != 0)
	{
		where = match(ending, "error")
		if (where != 0)
		{
			MakeMsg="*** ERROR - Make aborted"
		}
		printf("%s\\%s(%s):%s\n", msDir, msFile, msLine, ending);
	}
	else
	{
		printf("%s\\%s%s\n", msDir, msFile, ending);
	}
	next;
}

END {
	#print "-<<----- gccpipe.awk - ver 0.10 --------------------------------"
	if(MakeMsg!="")
	{
		print MakeMsg;
		exit 2;
	}
}

