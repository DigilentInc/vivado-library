COMPILER=
ARCHIVER=
CP=cp
COMPILER_FLAGS=
EXTRA_COMPILER_FLAGS=
LIB=libxil.a

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-I./. -I${INCLUDEDIR}

INCLUDEFILES=*.h
LIBSOURCES=*.c DEIPcK/*.cpp DEIPcK/utility/*.c DEWFck/*.cpp MRF24G/*.c MRF24G/*.cpp MRF24G/utility/*.c
OUTS = *.o

libs:
	echo "Compiling PmodWIFI..."
	$(COMPILER) $(COMPILER_FLAGS) $(EXTRA_COMPILER_FLAGS) -Wno-attributes $(INCLUDES) $(LIBSOURCES)
	$(ARCHIVER) -r ${RELEASEDIR}/${LIB} ${OUTS}
	make clean

include:
	${CP} $(INCLUDEFILES) $(INCLUDEDIR)
	if not exist ..\..\..\include\DEIPcK md ..\..\..\include\DEIPcK
	if not exist ..\..\..\include\DEIPcK\utility md ..\..\..\include\DEIPcK\utility
	if not exist ..\..\..\include\DEWFcK md ..\..\..\include\DEWFcK
	if not exist ..\..\..\include\MRF24G md ..\..\..\include\MRF24G
	if not exist ..\..\..\include\MRF24G\utility md ..\..\..\include\MRF24G\utility
	if not exist ..\..\..\include\MRF24G\utility\ud_inc md ..\..\..\include\MRF24G\utility\ud_inc
	if not exist ..\..\..\include\MRF24G\utility\ud_inc\internal md ..\..\..\include\MRF24G\utility\ud_inc\internal
	if not exist ..\..\..\include\MRF24G\utility\ud_inc\shared md ..\..\..\include\MRF24G\utility\ud_inc\shared
	cp DEIPcK/*.h $(INCLUDEDIR)/DEIPcK
	cp DEIPcK/utility/*.h $(INCLUDEDIR)/DEIPcK/utility
	cp DEWFck/*.h $(INCLUDEDIR)/DEWFcK
	cp MRF24G/*.h $(INCLUDEDIR)/MRF24G
	cp MRF24G/utility/*.h $(INCLUDEDIR)/MRF24G/utility
	cp MRF24G/utility/ud_inc/internal/*.h $(INCLUDEDIR)/MRF24G/utility/ud_inc/internal
	cp MRF24G/utility/ud_inc/shared/*.h $(INCLUDEDIR)/MRF24G/utility/ud_inc/shared

clean:
	rm -rf ${OUTS}
