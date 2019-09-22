CPP = bcc32
CPPFLAGS = -WM -O2

TARGET	= ABKCaster
OBJ1	= ABKCaster.obj
OBJ2	= charDat.obj charDat_di.obj inputData.obj inputDataSub.obj
OBJ3	= mainDat.obj mainDat_func.obj mainDat_mainRoop.obj mainDat_th075.obj mainDat_thread.obj
OBJ4	= mainDat_main.obj mainDat_manipMenu.obj mainDat_main_sub.obj mainDat_mainStep.obj syncData.obj syncDataSub.obj
LIB	= dinput.lib cw32mt.lib winmm.lib zlib.lib shlwapi.lib


all: $(TARGET)
$(TARGET) : $(OBJ1) mbcaster.res
$(TARGET) : $(OBJ2)
$(TARGET) : $(OBJ3)
$(TARGET) : $(OBJ4)
	$(CPP) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4) $(LIB)

.cpp.obj:
	$(CPP) $(CPPFLAGS) -c $<

# cleanup
clean:
	-del *.obj
	-del *.exe
	-del *.tds

th075Caster.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
charDat.obj: charDatClass.h etc.h const.h
charDat_di.obj: charDatClass.h etc.h const.h
inputData.obj: etc.h const.h inputDataClass.h syncDataClass.h
inputDataSub.obj: etc.h const.h inputDataClass.h syncDataClass.h
mainDat.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_func.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_mainRoop.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_th075.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_thread.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
#booster/boosterDatClass.h
mainDat_main.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_manipMenu.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_main_sub.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h
mainDat_mainStep.obj: mainDatClass.h charDatClass.h etc.h const.h inputDataClass.h syncDataClass.h

